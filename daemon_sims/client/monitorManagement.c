//*******************************************************************************
//*                                                                             *
//*  PETBox Acquisition and Management Graphical User Interface                 *
//*                                                                             *
//*  Copyright - 2009                                                           *
//*                                                                             *
//*  This code is the property of the Regents of UCLA.                          *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Richard Taschereau                                              *
//*                                                                             *
//*  Created: September 2009                                                    *
//*                                                                             *
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************

#include "windows.h"
#include "toolbox.h"
//#include <analysis.h>
#include <utility.h>
#include <time.h>
#include <userint.h>
#include "constants.h"
#include "typedefs.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "cameraManagement.h"
#include "utilities.h"
#include "logManagement.h"
#include <ansi_c.h>

#define NUM_BUF                  4096 // number of entries in the circular buffer

#define NUM_LOCAL_BUF             600 // number of entries in the "local" buffer to calculate intervals.
                                      // Should be large enough to hold at the very least two breaths
 									  // at the slowest breathing rate.  E.g. 12 bpm is one breath
 									  // every 5 secs, hence 2 breaths is 10 sec, which
 									  // at 50 fps is 500 entries

#define BREATH_MON_DISPLAY_DELAY  1.5 // in secs   (was 2.0 then 1.0)
#define BREATH_MON_BLACKOUT      10.0 // in secs   (should be ~ NUM_LOCAL_BUF/fps)
#define SECS_PER_MIN             60.0
#define DISPLAY_DIVIDER             1 // Display image every DISPLAY_DIVIDER frame 


#define MICROSECOND      1
#define SECOND           1000000 
#define HUNDRETH_SECOND  (SECOND/100)

#define MAX_ERROR_NUM               5
#define MAX_CONSISTENCY_ERROR_NUM 120



struct bufferEntry{
	unsigned int value;
	unsigned int timeStamp;   // in 1/100 of seconds
};

/*						 
	A circular buffer is a buffer of n elements
	which wraps around itself.  Each element 
	has a unique ID and so as long as it has
	not been overlaid, it can be retrieved.
	
	Accessors are appendValue to append one value at
	the next available entry, and readValues, which
	reads from a given element ID to the latest 
*/
struct circularBuffer{
	 struct bufferEntry *bufferStart;
	 struct bufferEntry *bufferEnd;
	 struct bufferEntry *nextToWrite;
	 CmtThreadLockHandle lock;
	 int wrap;
	 int n;
};


static  struct circularBuffer rawDataBuffer;

static  struct bufferEntry *localBuffer;
static	double *localValues;           // transferred (local) values from the circular buffer 
static  double *localValuesBis;
static	double *localTimeStamps;       

/*
	Local Functions
*/

static void   readLatestValues(struct circularBuffer *b, struct bufferEntry *destBuffer, int n);
static void   appendValue( struct bufferEntry entry, struct circularBuffer *b);
static void   subtractBackground(double *array, int n, double* arrayLessBackground, double* background);
static double calculatePower(double *array, int n);
static void   cyclicConvolve(double *array, int Na, double *kernel, int Nk, int Kx, double *result);
static void   nonCyclicConvolve(double *array, int Na, double *kernel, int Nk, int Kx, double *result);
static void   clipFilter(double *array, int n, double value);
static double stdDev(double *array, int n);
static int    zeroCrossings(double *array, int n, int *peakLocations);
static double calculateAverageInterval(int *peaks, double *timeStamps, int n); 
static void   acquireSideSnapshot(struct taskList* tl);

int CVICALLBACK monitoringThread(void *functionData);


// ----------------------------------------------------------------------------------------------

static int CVICALLBACK grabberThread(void *functionData){
	struct taskList *tl = (struct taskList*) functionData;
	fc2Error   error;
	fc2Image   image;
	fc2TimeStamp ts;
	int bitMapID;
	int fc2ErrorCount            = 0;
	int fc2ConsistencyErrorCount = 0;

	int numPixelsInImage;

	struct bufferEntry entry;
	char studyPath[MAX_FILE_NAME_LENGTH];
	char signalFileName[ MAX_FILE_NAME_LENGTH ];

	readCameraSettings();
	
	// Override ROI definition
	sideView.displayArea.height = 300;
	sideView.displayArea.width  = 500;
	sideView.displayArea.top    = 100;
	sideView.displayArea.left   =  48;
	
	if (0 != openView(contextSideCam, &sideView)){
		SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_RED);
		SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_CANVAS, ATTR_PEN_STYLE, VAL_SOLID);
		SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_CANVAS, ATTR_PEN_WIDTH, 2);
		SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_TRANSPARENT);
		CanvasDrawText(acquisitionPanel, ACQ_PANEL_CANVAS, "Camera not connected", 
			VAL_APP_META_FONT, MakeRect(40, 10, 40, 150), VAL_CENTER);
		breathingMonitoringRequested = 0;
		return -1;
	}
	
    error      = fc2CreateImage( &image );
	error      = fc2RetrieveBuffer (contextSideCam, &image);
	if ( error != FC2_ERROR_OK ) {
		
		if (error == FC2_ERROR_IMAGE_CONSISTENCY_ERROR){
				writeToLog("monitorManagement::grabberThread - Image consistency error at grabber startup.  Ignored.");
		} else {
			char temp[512];
		 	sprintfWithLock(512, temp,"monitorManagement::grabberThread - Camera error %d, in initial fc2RetrieveBuffer",error);
			writeToLog(temp); 
			return 8;
		}
	}
	
	// Let's get the juice
	SetSleepPolicy(VAL_SLEEP_NONE);

	// Keep raw breathing signal for eventual post-acquisition analysis/debugging analysis
	if ( 0 != makeStudyPathName(tl->study.tag, studyPath)) {
		sprintfWithLock(MAX_FILE_NAME_LENGTH,signalFileName, "monitorManagement::grabberThread - Can't get study path from tag %s",tl->study.tag);
		writeToLog(signalFileName);
	}
	sprintfWithLock(MAX_FILE_NAME_LENGTH,signalFileName, "%s\\breathingRawSignal.bin",studyPath);
	FILE* fout = fopen(signalFileName, "wb"); 
	
	// Keep 10 sec movie (600 frames) for further analysis (18MB/sec or 18MB per 60 frames)
	const int numMovieFrames = 2000;
	sprintfWithLock(MAX_FILE_NAME_LENGTH,signalFileName, "%s\\breathingMovie.bin",studyPath);
	FILE* fmov = NULL;
	if (physicsMode) fmov = fopen(signalFileName, "wb"); 
	
 	numPixelsInImage =  sideView.displayArea.width * sideView.displayArea.height;

	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_CANVAS, ATTR_DRAW_POLICY,    VAL_DIRECT_TO_SCREEN);
	if (0>NewBitmap(sideView.displayArea.width, 8, sideView.displayArea.width, sideView.displayArea.height, colorTable, image.pData, NULL, &bitMapID)){
		writeToLog("monitorManagement::grabberThread - Error creating bitMap");
		return 8;
	}
	CanvasDrawBitmap(acquisitionPanel, ACQ_PANEL_CANVAS, bitMapID, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
	
	int* thisColumnSum = malloc( sizeof(int)* sideView.displayArea.width);
	int* prevColumnSum = malloc( sizeof(int)* sideView.displayArea.width);
	
	int loopCounter=0;
	
	// Breathing signal is defined as the sum of absolute differences in column sums between two successive frames
	while (breathingMonitoringRequested){
		
		// Grab an image
		error     = fc2RetrieveBuffer(contextSideCam, &image);
        ts        = fc2GetImageTimeStamp( &image);
			
		if ( error != FC2_ERROR_OK  && error != FC2_ERROR_IMAGE_CONSISTENCY_ERROR) {
			// Error that is not a consistency error
			fc2ErrorCount++;
			char temp[512];
		 	sprintfWithLock(512, temp,"monitorManagement::grabberThread - Camera error %d, in fc2RetrieveBuffer (# %d)",error, fc2ErrorCount);
			writeToLog(temp); 
			if (fc2ErrorCount>=MAX_ERROR_NUM) {
				writeToLog("monitorManagement::grabberThread - Maximum number of errors reached for camera.  Abandoning monitoring");
				break; // break the loop
			}
		}
		if ( error != FC2_ERROR_OK  && error == FC2_ERROR_IMAGE_CONSISTENCY_ERROR) {
			// Consistency error
			fc2ConsistencyErrorCount++;
			char temp[512];
		 	sprintfWithLock(512, temp,"monitorManagement::grabberThread - Camera consistency error in fc2RetrieveBuffer (# %d)", fc2ConsistencyErrorCount);
			writeToLog(temp); 
			if (fc2ConsistencyErrorCount>=MAX_CONSISTENCY_ERROR_NUM) {
				writeToLog("monitorManagement::grabberThread - Maximum number of consistency errors reached for camera.  Abandoning monitoring");
				break; // break the loop
			}
		}
		
		
		// summ pixels in each column
		for (int i=0; i<sideView.displayArea.width; i++){
			thisColumnSum[i]=0;
			for(int j=0; j<sideView.displayArea.height; j++) thisColumnSum[i] +=image.pData[j*sideView.displayArea.width+i];
		}
		
		// Add absolute differences
		entry.value = 0;
		for (int i=0; i<sideView.displayArea.width; i++) entry.value += fabs(thisColumnSum[i]-prevColumnSum[i]);
		int* temp     = prevColumnSum;
		prevColumnSum = thisColumnSum;
		thisColumnSum = temp;
		
		// Get timeStamp and add observation to the buffer
		entry.timeStamp  = (int) (ts.seconds*SECOND) + (ts.microSeconds*MICROSECOND);
		entry.timeStamp /= HUNDRETH_SECOND;
		appendValue( entry, &rawDataBuffer);
		
		// Record raw signal
		if (fout != NULL ) fwrite(&entry, sizeof(entry), 1, fout);
		
		// Record short movie
		if (fmov != NULL && loopCounter<numMovieFrames) fwrite(image.pData, sizeof(unsigned char), numPixelsInImage, fmov); 
		
		// Display what we see
		if (++loopCounter%DISPLAY_DIVIDER == 0){
			SetBitmapData(bitMapID, sideView.displayArea.width, 8, colorTable, image.pData, NULL);
			CanvasDrawBitmap(acquisitionPanel, ACQ_PANEL_CANVAS, bitMapID, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
			ProcessSystemEvents();
		}
		
		if (loopCounter == numMovieFrames) fclose(fmov);
		
	}
	
	closeView(contextSideCam);
    error = fc2DestroyImage( &image );
	
	fclose(fout);
	fclose(fmov);
	free(thisColumnSum);
	free(prevColumnSum);

	
	return 0;
}


static int  createCircularBuffer( int n, struct circularBuffer *b){
	b->wrap        = 0;
	b->n           = n;
	b->bufferStart = malloc(n * sizeof(*(b->bufferStart)  ) );
	if (b->bufferStart == NULL){
		writeToLog("monitorManagement::createCircularBuffer - Error, couldn't allocate memory");
		return 8;
	}
	memset(b->bufferStart, 0, n*sizeof(  *(b->bufferStart) ) );
	b->bufferEnd   = b->bufferStart + b->n;
	b->nextToWrite = b->bufferStart;
	
	int retCode = CmtNewLock(NULL, 0, &b->lock);
	
	if (retCode<0){
		char temp[512];
		sprintfWithLock(512, temp,"monitorManagement::createCircularBuffer - Failed to create lock, retcode %d", retCode); 
		writeToLog(temp);
		return -1;
	}
	
	return 0;
}

static void deleteCircularBuffer(struct circularBuffer *b){
	CmtDiscardLock(b->lock);
	free(b->bufferStart);
}

// Append an entry to a circular buffer
static void appendValue( struct bufferEntry entry, struct circularBuffer *b){
			//printf("::appendValue - %d\n",entry.value);
	CmtGetLock(b->lock);
		*(b->nextToWrite) = entry;
		b->nextToWrite++;
		if ( b->nextToWrite == b->bufferEnd ){
			 b->nextToWrite  = b->bufferStart;
		     b->wrap++;
		}
	CmtReleaseLock(b->lock);
}


/* 
	Reads (transfers) n latest value from the circular buffer
	to a destination buffer, in chronilogical order
	(i.e. destbuffer[0] = oldest, destBuffer[n-1] = newest)
*/
static void readLatestValues(struct circularBuffer *b, struct bufferEntry *destBuffer, int n){
	
	CmtGetLock(b->lock);
	
		struct bufferEntry *dest    = destBuffer+n-1;
		struct bufferEntry *source;
		
		if (b->nextToWrite > b->bufferStart) {
			source = b->nextToWrite-1;
			while(dest>=destBuffer && source>=b->bufferStart) {
				*dest=*source;
				//printf("::readLatest 1/2 - %d (%d)\n", dest->value, dest);
				dest--;
				if (source > b->bufferStart) source--;
				else break;
			}
		}
	
		// This is the good old compact way; but LabWindows' debugger complains ...
		/*
		while(dest>=destBuffer && source>=b->bufferStart) *dest-- = *source--;
		source = b->bufferEnd-1;
		while(dest>=destBuffer && source>=b->bufferStart) *dest-- = *source--;
		*/
		
		source = b->bufferEnd-1; 
		while(dest>=destBuffer && source>=b->bufferStart) {
			*dest=*source;
				//printf("::readLatest 2/2 - %d (%d)\n", dest->value, dest);
			dest--;
			if (source > b->bufferStart) source--;
			else break;
		}
		
		
	CmtReleaseLock(b->lock);
}


// Simple derivative (IN PLACE)
static inline void derivate(double* array, const int n){
	for (register int k=0; k<n-1; k++) array[k] = array[k+1] - array[k] ;
}

//
static inline void bubbleSort(double numbers[], int array_size)
{
  register int i, j;
  double temp;
 
  for (i = (array_size - 1); i > 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (numbers[j-1] > numbers[j])
      {
        temp = numbers[j-1];
        numbers[j-1] = numbers[j];
        numbers[j] = temp;
      }
    }
  }
}

static inline double myMedian(double *array, int n){
	double *localArray = malloc( n*sizeof(double) );
	double median;
	
	for(register int k=0; k<n; k++) localArray[k]=array[k];
	bubbleSort(localArray, n);
	median = localArray[n/2];
	free (localArray);
	return median;
	
}

static inline double myMean(double *array, int n){
	double sum=0;
	for (register int k=0; k<n; k++) sum += array[k];
	return sum/n;
}
	
// Applies a median filter of halfWidth hw
static inline void medianFilterRT(double* output, double* input, int n, int hw){
	register int k;
	
	for (k=0;    k<hw;   k++) output[k] = input[k]; 
	for (k=hw;   k<n-hw; k++) output[k] = myMedian(input+k-hw, 2*hw+1); 
	for (k=n-hw; k<n;    k++) output[k] = input[k]; 
}


// Applies a mean filter of halfWidth hw
static inline void meanFilterRT(double* output, double* input, int n, int hw){
	register int k;
	for (k=0;    k<hw;   k++) output[k] = input[k]; 
	for (k=hw;   k<n-hw; k++) output[k] = myMean(input+k-hw, 2*hw+1);
	for (k=n-hw; k<n;    k++) output[k] = input[k];  
}

/*
// Applies a median filter of halfWidth hw
static inline void medianFilterNI(double* output, double* input, int n, int hw){
	register int k;
	for (k=0;    k<hw;   k++) output[k]=input[k]; 
	for (k=hw;   k<n-hw; k++){
		int rc = Median( input+k-hw, 2*hw+1, &output[k] );
		if (rc!=0) printf("monitorManagement::medianFilter - non-zero rc:%d\n", rc);
	}
	for (k=n-hw; k<n;    k++) output[k]=input[k]; 
	
	
}
*/

/*
// Applies a mean filter of halfWidth hw
static inline void meanFilterNI(double* output, double* input, int n, int hw){
	register int k;
	for (k=0;    k<hw;   k++) output[k]=input[k]; 
	for (k=hw;   k<n-hw; k++){
		int rc = Mean( input+k-hw, 2*hw+1, &output[k] );
		if (rc!=0) printf("monitorManagement::meanFilter - non-zero rc:%d\n", rc);
	}
	for (k=n-hw; k<n;    k++) output[k]=input[k];  
}

*/

/*
	Finds negative zero crossings (i.e. transitions from
	negative to positive) allowing for intervening zeroes.
	
	Returns the number of crossings found.
	(those are peaks when the input is a differrentiated signal);
*/
static int negativeZeroCrossings(double *array, int n, double *peakLocations){
	bool arm = 0;
	register int k;
	int pc=0; // peak counter
	
	for (k=0; k<n; k++){
		if (array[k]>0){
			arm=1;
		} else
		if (array[k]<0 && arm){
			peakLocations[pc++]=k;
			arm=0;
		}
	}
	return pc;
}


// Compress low values (noise) in the signal.  Output values are [0,1]
// UPDATE IN PLACE
static inline void compressNoise(double* array, const int n, const double sigma){
	for (register int k=0; k<n; k++){
		double x = array[k]/sigma;
		array[k] = 1.0 - exp(- x*x/2.0);
	}
	
}

/*
	Calculate the mean and standard deviation of array
*/
static inline void calculateMeanAndStd(double *array, int n, double* mean, double* std){
	
	register double sumOfSquares = 0;
	register double sum          = 0;
	register double value;
	register int i;
	double meanOfSquares;
			 
	if (n>0){		 
		for (i=0; i<n; i++){
			value = array[i]; 
			sum          += value;
			sumOfSquares += value*value;
		}
		(*mean)       = sum/n;
		meanOfSquares = sumOfSquares/n;
		(*std) = sqrt(meanOfSquares - (*mean)*(*mean) );
	} else (*std) = 0.0;
	
}

// Filter the time series keeping only values above some threshold
// UPDATES IN PLACE
static inline void thresholdAtValue(double *array, const int n, const double value){
	for (register int i=0; i<n; i++) if (array[i]<value) array[i]=0.0;

}

static inline void subtractValue(double* array, const int n, const double value){
	for (register int i=0; i<n; i++)  array[i] -= value;
}



/*
static inline double skewnessNI(double* input, const int n){
	double m2,m3;
	Moment(input, n, 2, &m2);
	Moment(input, n, 3, &m3);
	double sqrtM2 = sqrt(m2);
	
	return m3/(sqrtM2*sqrtM2*sqrtM2);
}

*/

/*
static inline double skewnessRT(double* input, const int n){
	double m1=0;
	double m2=0;
	double m3=0;
	double diff, diff2, diff3;;
	
	//Calculate mean
	for (int k=0; k<n; k++) m1+=input[k];
	m1/=n;
	
	// Calculate second and third central moment
	for (int k=0; k<n; k++) {
		diff  = input[k]-m1;
		diff2 = diff *diff;
		diff3 = diff2*diff;
		m2 += diff2;
		m3 += diff3;
	}
	m2/=n;
	m3/=n;
	
	double sqrtM2 = sqrt(m2);
	
	return m3/(sqrtM2*sqrtM2*sqrtM2);
}
*/

/*

	Process the latest signal values in the circular buffer:
	Calculate breathing intervals and put in interval buffer


*/

static const double nominalFrameRate         = 60.00;	  // Frame Rate per second
static const int    medianFilterHalfWidth    = 2;		  // Empirically determined to get rid of the heating strip noise
static const double breathingDutyCycle       = 0.180;	  // Approximate time interval during which the mouse breaths, in secs ( inhale -> exhale)

static int processLatest(void){

	register int i;

	// Read the last NUM_LOCALBUF values 
	readLatestValues(&rawDataBuffer, localBuffer, NUM_LOCAL_BUF);
	for (i=0; i<NUM_LOCAL_BUF; i++){
		localValues[i]=localBuffer[i].value;
		localTimeStamps[i]=localBuffer[i].timeStamp;
	}
	
	// -------------------------------
	//     DO SIGNAL PROCESSING HERE
	// -------------------------------

	// Median filter to get rid of heating strip noise (on some models)
	medianFilterRT(localValuesBis, localValues, NUM_LOCAL_BUF, medianFilterHalfWidth);

	// Calculate skewness to see if we actually have a signal ...
	//double skewnessValue =   skewnessRT(localValuesBis, NUM_LOCAL_BUF) ;
	//if (skewnessValue < 0.5 ) return 0;
	
	// Calculate mean and standard deviation
	double mean, std;
	calculateMeanAndStd(localValuesBis, NUM_LOCAL_BUF, &mean, &std);
	
	// do we have a signal ? (empirical rule of thumb)
	float rho = mean/std;
	if ( rho > 15.0 ) return 0.;

	// Subtract mean and clip to positive values
	subtractValue   (localValuesBis, NUM_LOCAL_BUF, mean);
	thresholdAtValue(localValuesBis, NUM_LOCAL_BUF, 0.);

	// Compress noise (also normalizes the signal in the interval [0,1] )
	compressNoise(localValuesBis, NUM_LOCAL_BUF, std);

	// Apply Mean filter, which acts as a pulse integrator
	const int halfMeanFilter = (int) (0.5 * breathingDutyCycle * nominalFrameRate ); 
	meanFilterRT(localValues, localValuesBis, NUM_LOCAL_BUF, halfMeanFilter);

	// Clip to empirical value (0.1)
	thresholdAtValue(localValues, NUM_LOCAL_BUF, 0.1);

	// Find peaks (derivative then zero crossings) 
	double peakLocations[NUM_LOCAL_BUF];
	derivate(localValues, NUM_LOCAL_BUF);
	int    numPeaks = negativeZeroCrossings(localValues, NUM_LOCAL_BUF-1, peakLocations);
	
	//  Collect time stamps at peaks
	double collectedTimeStamps[NUM_LOCAL_BUF];	   // in 1/100 of a second
	for (i=0; i<numPeaks; i++) collectedTimeStamps[i]=localTimeStamps[ (int) peakLocations[i] ];
	if (numPeaks<=2) return 0;

	// Find median interval between peaks (derivate to find intervals and then select median)
	derivate(collectedTimeStamps, numPeaks);
	double medianInterval=0.;						   // in 1/100 of a second
	medianInterval = myMedian(collectedTimeStamps, numPeaks-1); 
	
	// Convert to breaths-per-minute and return
	double breathingRatePerMin;
	if (medianInterval>0) breathingRatePerMin = SECS_PER_MIN / (medianInterval/100.0);
	else   breathingRatePerMin=0;
 	
	return (int) breathingRatePerMin;
}


struct taskList testTaskList;
int CVICALLBACK test_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			
				
	//

int studyHeight, studyWidth,studyTop, studyLeft, protHeight,protTop,protLeft, protWidth, protRows, studyRows;

GetCtrlAttribute(studyTabHandle, TAB_STUDY_TREE, ATTR_HEIGHT, &studyHeight);
GetCtrlAttribute(studyTabHandle, TAB_STUDY_TREE, ATTR_WIDTH, &studyWidth);
GetCtrlAttribute(studyTabHandle, TAB_STUDY_TREE, ATTR_TOP, &studyTop);
GetCtrlAttribute(studyTabHandle, TAB_STUDY_TREE, ATTR_LEFT, &studyLeft);
GetCtrlAttribute(studyTabHandle, TAB_STUDY_TREE, ATTR_NUM_VISIBLE_ROWS, &studyRows);

GetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE,  ATTR_HEIGHT, &protHeight);
GetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE,  ATTR_WIDTH, &protWidth);
GetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE, ATTR_TOP, &protTop);
GetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE, ATTR_LEFT, &protLeft);
GetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE, ATTR_NUM_VISIBLE_ROWS, &protRows);

SetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE, ATTR_HEIGHT,708);

printf("s(%d,%d,%d,%d,%d), p(%d,%d,%d,%d,%d)\n",studyHeight,studyWidth,studyTop,studyLeft, studyRows,protHeight,protWidth,protTop,protLeft, protRows);	
	
	//

			
			/*
			SetCtrlVal(acquisitionPanel,ACQ_PANEL_breathFreq, 0);
			SetCtrlVal(acquisitionPanel,ACQ_PANEL_hour, 0);
			SetCtrlVal(acquisitionPanel,ACQ_PANEL_min, 0);
			SetCtrlVal(acquisitionPanel,ACQ_PANEL_sec, 0);
			DeleteGraphPlot(acquisitionPanel, ACQ_PANEL_breathingHistory, -1, VAL_IMMEDIATE_DRAW);
			SetPanelAttribute(acquisitionPanel, ATTR_DIMMED, 0);
			fadeInPanel(&acqPanelFading);
	
			strcpy(testTaskList.study.tag , "T3528143859C590184");   // Study s005 on my computer
			testTaskList.prot.duration=1200;
			
			breathingMonitoringRequested = 1;
			CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, monitoringThread, &testTaskList, 0);
			*/
			break;
	}
	return 0;
}

 

int CVICALLBACK monitoringThread(void *functionData){
	struct taskList *tl = (struct taskList*) functionData;
	double breathingRatePerMin;
	double startTime;
	double *x;
	int    *y;
	int    arrayPos=0 ;
	bool   breathingHistory=0;
	char   cBreathFreq[6];
	const  int traceColor   = sofieBlue;
	
	int xyNumEntries = 0.5 + (tl->prot.duration / BREATH_MON_DISPLAY_DELAY );
	x = malloc( xyNumEntries * sizeof(double));
	y = malloc( xyNumEntries * sizeof(int   ));

	if (y==0 || x == 0){
		breathingHistory = 0;
		writeToLog("monitorManagement::monitoringThread - Can't allocate memory for breathing history");
	} else breathingHistory = 1;
	
	SetAxisScalingMode(acquisitionPanel, ACQ_PANEL_breathingHistory, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, tl->prot.duration);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathingHistory, ATTR_DATA_MODE,      VAL_RETAIN);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathingHistory, ATTR_REFRESH_GRAPH,           1);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathingHistory, ATTR_COPY_ORIGINAL_DATA,      0);
	
	startTime = Timer();
	breathingMonitoringThreadRunning = 1;
	
	// Prepare buffers and start the grabber thread
	localBuffer   = malloc(  sizeof(*localBuffer)   * (NUM_LOCAL_BUF)  );
	if (NULL == localBuffer){
		writeToLog("monitorManagement::monitoringThread - Error, no memory for localBuffer.");
		goto Error;
	}
	// OK to use memset, those are unsigned ints
	memset  (localBuffer, 0, sizeof(*localBuffer)   * (NUM_LOCAL_BUF)  );
	
	// Memory for a first buffer of local values
	localValues   = malloc(  sizeof(*localValues)   * (NUM_LOCAL_BUF)  );
	if (NULL == localValues){
		writeToLog("monitorManagement::monitoringThread - Error, no memory for localValues.");
		goto Error;
	}
	for (int k=0; k<NUM_LOCAL_BUF; k++) localValues[k]=0.0;
  	
	// Memory for a second buffer of local values
	localValuesBis   = malloc(  sizeof(*localValuesBis)   * (NUM_LOCAL_BUF)  );
	if (NULL == localValuesBis){
		writeToLog("monitorManagement::monitoringThread - Error, no memory for localValuesBis.");
		goto Error;
	}
	for (int k=0; k<NUM_LOCAL_BUF; k++) localValuesBis[k]=0.0;

	// Memory for time stamps
	localTimeStamps   = malloc(  sizeof(*localTimeStamps)   * (NUM_LOCAL_BUF)  );
	if (NULL == localTimeStamps){
		writeToLog("monitorManagement::monitoringThread - Error, no memory for localTimeStamps.");
		goto Error;
	}
	for (int k=0; k<NUM_LOCAL_BUF; k++) localTimeStamps[k]=0.0;
	
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, grabberThread, tl, 0);
	SetCtrlVal(acquisitionPanel, ACQ_PANEL_breathFreq, "0");
	Delay(BREATH_MON_BLACKOUT);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathFreq, ATTR_DIMMED, 0);

	
	// main loop
	int plotHandles[2]={0,0};
	while(breathingMonitoringRequested){
		
		breathingRatePerMin = processLatest();
		sprintfWithLock(sizeof(cBreathFreq), cBreathFreq, "%d", (int) breathingRatePerMin );
		SetCtrlVal(acquisitionPanel, ACQ_PANEL_breathFreq, cBreathFreq);

		if ( 0 == (int) breathingRatePerMin) SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathFreq, ATTR_DIMMED, 1);
		else                                 SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathFreq, ATTR_DIMMED, 0);

		if (breathingHistory){
			x[arrayPos] = Timer()-startTime;
			
			if( 0 != (int) breathingRatePerMin) y[arrayPos] = breathingRatePerMin;
			else if (arrayPos>1)                y[arrayPos] = y[arrayPos-1];
			else                                y[arrayPos] = 0;
			
			plotHandles[arrayPos%2] = PlotXY(acquisitionPanel, ACQ_PANEL_breathingHistory, x, y, arrayPos+1, VAL_DOUBLE, VAL_INTEGER, VAL_FAT_LINE, VAL_NO_POINT, VAL_SOLID, 1, traceColor);
			if ( arrayPos < (xyNumEntries-1)) arrayPos++;
			if ( plotHandles[arrayPos%2] ) DeleteGraphPlot(acquisitionPanel, ACQ_PANEL_breathingHistory,  plotHandles[arrayPos%2], VAL_NO_DRAW);
		}
		Delay(BREATH_MON_DISPLAY_DELAY);
	}
	
	// Done
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_breathFreq, ATTR_DIMMED, 1);
	
	
	// Save breathing trace for research purposes
	char studyPath[MAX_FILE_NAME_LENGTH];
	char traceFileName[ MAX_FILE_NAME_LENGTH ];
	if ( 0 != makeStudyPathName(tl->study.tag, studyPath)) {
		sprintfWithLock(MAX_FILE_NAME_LENGTH, traceFileName, "monitorManagement::monitoringThread - Can't get study path from tag %s",tl->study.tag);
		writeToLog(traceFileName);
	} else{
		sprintfWithLock(MAX_FILE_NAME_LENGTH, traceFileName, "%s\\breathingHistory.bin",studyPath);
		FILE* ftrace = fopen(traceFileName, "wb");
		if (ftrace){
			fwrite(x, sizeof(double), arrayPos+1, ftrace); 
			fwrite(y, sizeof(int),    arrayPos+1, ftrace); 
			fclose(ftrace);
		}
	}
	
Error:
	
	free(localBuffer);
	free(localValues);
	free(localValuesBis);
	free(localTimeStamps);
	free(x);
	free(y);
	breathingMonitoringThreadRunning = 0;
	
	return 0;
}

int monitorManagementInitialization(){
	if (0==createCircularBuffer(NUM_BUF,           &rawDataBuffer)){
		return 0;
	} else return 8;
}

void monitorManagementTermination(){
	deleteCircularBuffer(&rawDataBuffer);
}


