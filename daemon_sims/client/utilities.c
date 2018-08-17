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


// That's windows XP (http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx)
#define _WIN32_WINNT 0x0501

#include "windows.h"
#include <formatio.h>
#include "winbase.h"
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "constants.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "utilities.h"
#include "logManagement.h"
#include "typedefs.h"
#include "timeRing.h"

#include <stdlib.h>
#include <stdarg.h>


void localTimeWithLock( struct tm *tm, const time_t *dateTime){
	CmtGetLock(sprintfLock);
		struct tm *sys = localtime(dateTime);
		memcpy(tm, sys, sizeof(struct tm) );
	CmtReleaseLock(sprintfLock);
 }


// Generate a unique ID for studies and protocols
void makeUniqueID(char* theID){
	time_t theTime   = time(NULL);
	clock_t theClock = clock();
	
	sprintfWithLock(32, theID, "T%uC%u", (unsigned int) theTime, (unsigned int) theClock);

}

void sprintfWithLock(unsigned __int64 bufLen, char * string, char *fmt, ...){
	
	CmtGetLock(sprintfLock);

  	va_list argPtr;
  	va_start(argPtr, fmt);

  	char * buf=string;
  	for(int k=0; k<bufLen;k++) buf[k]=0;
  
  	int    intVar;
	unsigned int uintVar;
  	double floatVar;
 	char * stringVal;
 	char   c;
	unsigned char * ucharPtr;

	for (char* p=fmt; *p; p++){
    
	    if (*p == '%'){  // if we have a format												

	      switch (*++p){

			  case 'd':
				intVar = va_arg(argPtr, int);
				buf += sprintf(buf, "%d",intVar);
				break;
 
				// That's for time display (h, m, s) with 2 digits
			  case 't':
				intVar = va_arg(argPtr, int);
				buf += sprintf(buf, "%02d",intVar);
				break;

				// That's for generating image numbers (3 digits)
			  case 'm':
				intVar = va_arg(argPtr, int);
				buf += sprintf(buf, "%03d",intVar);
				break;
				
 			  case 'u':
				uintVar = va_arg(argPtr, unsigned int);
				buf += sprintf(buf, "%u",uintVar);
				break;

			  case 'f':
				floatVar = va_arg(argPtr, double);
				buf += sprintf(buf, "%f",floatVar);
				break;

			  case 'w':
				floatVar = va_arg(argPtr, double);
				buf += sprintf(buf, "%3.1f",floatVar);
				break;

			  case 's':
				stringVal = va_arg(argPtr, char*);
				while ((c=*(stringVal++))) *(buf++)=c;	   // Expect warning
				break;

			  case 'X':
				ucharPtr = va_arg(argPtr, unsigned char * );
				buf += sprintf(buf, "%02X",ucharPtr);
				break;

			  default:
				*(buf++)='%';	
				*(buf++)=*p;
				break;
	
	      }

	    } else  *(buf++)=*p; // simply copy char
    
  }

	va_end(argPtr);
	CmtReleaseLock(sprintfLock);
}


int createTaskList(struct taskList** tl){
	// Create the task list
	(*tl) = malloc(sizeof(struct taskList));
	if ((*tl) == NULL){
		writeToLog("utilities::createTaskList - ERROR, no memory to create task list");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return 8;
	}
	memset((*tl), 0, sizeof(**tl));
	(*tl)->version = CURRENT_TASKLIST_VERSION; // Current task list version number
	return 0;
}

void printTaskList(struct taskList *tl){
	printf("\nTask List for %s\n", tl->study.name);
	printf("    tag:%s\n    version:%d\n    prot name:%s\n    acq:%d\n    hist:%d\n    recon:%d\n", 
		tl->study.tag, tl->version, tl->prot.name, tl->prot.acq, tl->prot.hist, tl->prot.recon  );
	printf("    ACQ   start:%I64d, end:%I64d, comp:%d\n", tl->acqInfo.startTime, tl->acqInfo.endTime, tl->acqInfo.completionCode);
	printf("    HIST  start:%I64d, end:%I64d, comp:%d\n", tl->histInfo.startTime, tl->histInfo.endTime, tl->histInfo.completionCode);
	printf("    RECON start:%I64d, end:%I64d, comp:%d\n", tl->reconInfo.startTime, tl->reconInfo.endTime, tl->reconInfo.completionCode);
	
}

void shutdown(void){
	acquisitionRequested         = 0;
	breathingMonitoringRequested = 0;    // Early notification
	
	int retryCount=20;
	while(acquisitionRunning && retryCount--) Delay(0.200); // wait until acquisition is done

	QuitUserInterface(0);
}

/* 
	Create/Free Directory List
*/

char ** createDirList(char *startPath, int dir, int *size){
	char **nameArray;
	int result;
	int maxEntries = 500;   // This is the max. number of entries per level
	
	(*size)=0;
	nameArray           = malloc( (1+maxEntries)*sizeof(nameArray)) ;
	if (nameArray == NULL){
		writeToLog("createDirList - failed to allocate memory for array");
		return NULL;
	}
	
	// Prime the loop
	nameArray[*size]    = malloc(MAX_PATHNAME_LEN);
	if (nameArray[*size] == NULL){
		writeToLog("utilities::createDirList - Failed to allocate memory for entry");
		return NULL;
	}
	(*nameArray[*size]) = 0;
	
	/* GetFirstFile returns the base name only */
	result = GetFirstFile(startPath, 1, 0, 0, 0, 0, dir, nameArray[*size]); 
	while (0 == result && (*size)<maxEntries) {
		void* buf = malloc(MAX_PATHNAME_LEN);
		if (buf==NULL){
			writeToLog("utilities::createDirList - Failed to allocate memory for array");
			return NULL;
		}
		nameArray[++(*size)] =  buf;
		(*nameArray[*size]) = 0;
		result = GetNextFile(nameArray[*size]);
	}
	
	return nameArray;
}

void freeDirList(char **nameArray, int size){
	int i;
	for (i=0; i<=size; i++) free(nameArray[i]);
	free(nameArray);
}


char * removeExtension(char *string){
	char* theDot =  strrchr(string,'.');
	if (theDot) (*theDot)=0;
	return string;
}

// Chomp to remove the new line character left by fgets
char* chomp(char* str){
	__int64  offset = strlen(str) -1;
	*(str+offset)=0;
	return str;
}

void hmsToDuration(unsigned short int hour, unsigned short int min, unsigned short int sec, unsigned int *duration){
	(*duration) = (3600*hour) + (60*min) + sec; 	
}
																							 
void durationToHms(unsigned short int* hour, unsigned short int* min, unsigned short int* sec, unsigned int duration){
	(*hour) =  duration / 3600;
	(*min)  = (duration - (*hour)*3600)/60;
	(*sec)  =  duration - (*hour)*3600 - (*min)*60;
}

// 
int isDirectoryEmpty(char *startPath){
	char fileName[MAX_FILE_NAME_LENGTH];
	int result;
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH,fileName,"%s\\*",startPath);
	result = GetFirstFile(fileName, 1, 0, 0, 0, 0, 1, fileName);
	if (result == 0 ) return 0;
	else              return 1;
}


// Actually only deletes files from a directory (does not delete subdirs);
int emptyDirectory(char *startPath){
	char fileName[MAX_FILE_NAME_LENGTH];
	sprintfWithLock(MAX_FILE_NAME_LENGTH,fileName,"%s\\*.*",startPath);
	return DeleteFile(fileName);
}

/*
	Deletes a non-empty directory
*/
int forceDeleteDir(char *dirName){
	char dicomDir[16+MAX_FILE_NAME_LENGTH];
	sprintfWithLock(16+MAX_FILE_NAME_LENGTH,dicomDir,"%s\\dicom",dirName);
	
	emptyDirectory(dicomDir);
	DeleteDir(dicomDir);
	
	emptyDirectory(dirName); // non zero return codes are OK here, directory may be empty already
	return DeleteDir(dirName);
}

/*

	Get the path of the entry identified by tag
	
	(in) tag         : the tag of the entry (study or protocol) 
	(out) pathName   : the full path to the entry
	(int) keepLowest : keep (1) or not (0) the lowest level of the path

*/

int myGetTreeItemPathFromIndex(int panel, int control, int index, char *finalPathName, int keepLowest){
	
	char rawPathName[MAX_FILE_NAME_LENGTH]; 
	char *pastFirstLevel;
	int  retCode;
	
	retCode = GetTreeItemPath(panel, control, index, "\\", rawPathName);
	if (retCode<0)	return 8;
	
	// Position past the '\' after "ALL XXXXX\...."
	pastFirstLevel = strchr(rawPathName, '\\' );
	if (pastFirstLevel == NULL) return 8;
	pastFirstLevel++; 
	
	if ( keepLowest){}
	else{
		char * lastSeparator = strrchr(pastFirstLevel, '\\');
		if (lastSeparator != 0) (*lastSeparator) = 0;
		else                    (*pastFirstLevel)  = 0;
	}
		
	myStrcpy(finalPathName, pastFirstLevel);
	return 0;
	
}


int myGetTreeItemPathFromTag(int panel, int control, char *tag, char *finalPathName, int keepLowest){

	int  index;
	int  retCode = GetTreeItemFromTag(panel, control, tag, &index);
	if (retCode<0)	return 8; 
	
	return  myGetTreeItemPathFromIndex(panel, control, index, finalPathName, keepLowest);
	
}

/*
	From a tag in the protocol tree, returns the
	full path name of the entry (including entry name)and adds the
	optional extension at the end.
	
	(in) tag: the tag of the protocol
	(out) fullPath: the full file path of the protocol
	
	returns 0: if valid index
*/

int makeProtocolPathName(char* tag, char *fullPath, char *extension){

	char protPathName[MAX_FILE_NAME_LENGTH]; 
	myGetTreeItemPathFromTag(protocolTabHandle,TAB_PROT_TREE, tag, protPathName, 1);
	sprintfWithLock(MAX_FILE_NAME_LENGTH, fullPath,"%s\\%s%s", protFolder, protPathName, extension);
	
	return 0;

}

/*

	From a tag in the study tree, returns
	the full path name (including entry name)
	
*/

int makeStudyPathName(char *studyTag, char *studyPathName){
	char *studyFileName;
	char  pathName[MAX_FILE_NAME_LENGTH];
	int retCode;
	int index;

	retCode = GetTreeItemFromTag(studyTabHandle, TAB_STUDY_TREE, studyTag, &index);
	if (retCode<0) return 8;
	
	retCode = GetTreeItemPath(studyTabHandle, TAB_STUDY_TREE, index, "\\", pathName);
	if (retCode<0) return 8;
	
	// Position past the "ALL STUDIES\..."
	studyFileName = strchr(pathName, '\\' ); // Locate top level (the dummy ALL STUDIES)   
	if (studyFileName != NULL){
		studyFileName ++;   // Get past the '\' to point to the file full path per se 
		sprintfWithLock(MAX_FILENAME_LEN, studyPathName,"%s\\%s", studyFolder, studyFileName);
		retCode = 0;
	} else retCode = 8;
	
	return retCode;
}

/*
	For this to work smoothly,
	the width of the small and large control 
	must be the same
*/


void fadeInPanel(struct fadingPanel *pan){
	unsigned int i;
	unsigned char *alpha;
	
	DeleteImage(animationPanel,FADE_PANEL_PICTURE);
	DisplayPanel(animationPanel);
	for(i=1; i<pan->numSteps; i++){
			alpha = pan->alphaArray[i];
			SetBitmapDataEx(pan->bitMapID, -1, 0, NULL, NULL, NULL, alpha);
			SetCtrlBitmap(animationPanel, FADE_PANEL_PICTURE, 0, pan->bitMapID);
			Delay(0.010);
	}
	
	DisplayPanel(pan->panel);
}


void fadeOutPanel(struct fadingPanel *pan){
	unsigned int i;
	unsigned char *alpha;
	
	HidePanel(pan->panel);
	for(i=pan->numSteps-1; i>0; i--){
			alpha = pan->alphaArray[i];
			SetBitmapDataEx(pan->bitMapID, -1, 0, NULL, NULL, NULL, alpha);
			SetCtrlBitmap(animationPanel, FADE_PANEL_PICTURE, 0, pan->bitMapID);
			Delay(0.010);
	}
								
	HidePanel(animationPanel);
}

int fadingPanelInitialization(struct fadingPanel *pan, int numSteps, int panelID){
	unsigned int i, j;
	int imageSize, alphaSize;
	unsigned char *alpha;
	unsigned char calpha;
	int bytesPerPixel, pixelDepth;

	pan->numSteps      = numSteps;
	pan->panel         = panelID;

	GetPanelDisplayBitmap(pan->panel,    VAL_VISIBLE_AREA, VAL_ENTIRE_OBJECT, &pan->bitMapID);
	GetBitmapData        (pan->bitMapID, NULL, &pixelDepth, NULL, NULL, NULL, NULL, NULL); 
	GetBitmapInfoEx      (pan->bitMapID, NULL, &imageSize, NULL, NULL); // Obtain imageSize
	pan->alphaArray     = malloc(pan->numSteps*sizeof(void*));
	if ( pan->alphaArray == NULL){
		writeToLog("utilities::fadingPanelInitialization - Can't allocate memory for array");
		return 8;
	}
	bytesPerPixel       = pixelDepth/8;
	alphaSize = 0;
	if (bytesPerPixel>0){
		alphaSize       = imageSize/bytesPerPixel;
	} else{
		writeToLog("utilities::fadingPanelInitialization - Error, bytes per pixel is zero");
		return 8;
	}
	
	for(i=0; i<pan->numSteps; i++){
		calpha = (unsigned char)  ((i*255)/(pan->numSteps-1)); 
		alpha  = pan->alphaArray[i] = malloc(alphaSize);
		if (alpha==NULL){
			writeToLog("utilities::fadingPanelInitialization - Can't allocate memory for array element");
			return 8;
		} else  
			for(j=0; j<alphaSize; j++) alpha[j]   =  calpha;
	}
	
	return 0;
	
}

void fadingPanelTermination(struct fadingPanel *pan){
	unsigned int i;
	for(i=0; i<pan->numSteps; i++) free(pan->alphaArray[i]);
	free(pan->alphaArray);
	
}


// Put an "image" in a transparent button
void putImageInTransparentButton(int panel, int control){
	
	const int border=2;
	Rect buttonArea;
	int bitMapID;
	
	GetCtrlBitmap(panel, control, 0, &bitMapID);  
	if (bitMapID==0){
		GetCtrlAttribute(panel, control, ATTR_HEIGHT, &buttonArea.height);
		GetCtrlAttribute(panel, control, ATTR_LEFT,   &buttonArea.left);
		GetCtrlAttribute(panel, control, ATTR_TOP,    &buttonArea.top);			
		GetCtrlAttribute(panel, control, ATTR_WIDTH,  &buttonArea.width);
		
		buttonArea.left   +=  border;
		buttonArea.top    +=  border;
		buttonArea.height -=2*border;
		buttonArea.width  -=2*border;

		GetPanelDisplayBitmap(panel,     VAL_FULL_PANEL, buttonArea, &bitMapID);
		SetCtrlBitmap(panel, control, 0, bitMapID);
	}

}


// Update task list on disk
int writeTaskList(struct taskList *tl){
	char   studyPath   [MAX_FILE_NAME_LENGTH];
	char   taskListFileName [MAX_FILE_NAME_LENGTH];
	FILE  *fileTaskList;
 	char tempString [2048];
	
	if ( 0!=makeStudyPathName(tl->study.tag, studyPath)){
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		sprintfWithLock(2048, tempString, "utilities::writeTaskList - Invalid tag:%s, name:%s", tl->study.tag, tl->study.name);
		writeToLog  (tempString);
		return 8;
	}
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH, taskListFileName,"%s\\taskList.bin",studyPath);
	
	fileTaskList = fopen(taskListFileName, "wb");
	
	if (tl->version != CURRENT_TASKLIST_VERSION) {
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		writeToLog  ("utilities::writeTaskList - ERROR, assert task list version");
	}

	if (fileTaskList) fwrite(tl, sizeof(*tl), 1, fileTaskList);
	else {
		sprintfWithLock(2048,tempString,"Error writing task list, tag %s, fileName %s", tl->study.tag,taskListFileName);
		writeToLog(tempString);
		return 8;
	}
	fclose(fileTaskList);
	return 0;
}


// Read the task list; may not be present (not a big deal)
int readTaskList(char *taskListFileName, struct taskList *tl){
	FILE  *fileTaskList;
	fileTaskList = fopen(taskListFileName, "rb");
	int retCode = 0;
	size_t objectsRead=0;
	
	if (fileTaskList){
		objectsRead = fread(tl, sizeof(struct taskList), 1, fileTaskList);
		if (objectsRead != 1 || tl->version != CURRENT_TASKLIST_VERSION) {
			writeToLog("utilities::readTaskList - Error reading task list; possibly wrong version");
			retCode = 8;
		}
		fclose(fileTaskList); 
	} else retCode=8;  // if file couldn't be opened
	
	return retCode;
}

// Determines task (i.e. acq, hist , etc)  status based on completion code and time stamps
int determineTaskStatus(struct taskInfo *ti){
	if      ( 0 == ti->completionCode) return TASK_STATUS_COMPLETED;
	else if ( 0 <  ti->completionCode) return TASK_STATUS_ERROR;
	// here completion code < 0
	else if ( 0 == ti->startTime     ) return TASK_STATUS_SCHEDULED;
	else if ( 0 == ti->endTime       ) return TASK_STATUS_RUNNING;
	else                               return 0;
}

int readStudyData(char *fileName, struct studyData *sd){
	FILE * sdFile = fopen(fileName, "rb");
	int retCode = 0;
	size_t objectsRead=0;
	
	if (sdFile){
		objectsRead = fread(sd, sizeof(*sd), 1, sdFile);
		if (objectsRead != 1 || sd->version != CURRENT_STUDYDATA_VERSION){
			writeToLog("utilities::readStudyData - Error reading study data; possibly wrong version");
			retCode = 8;
		}
		fclose(sdFile); 
	} else retCode=8;

	return retCode;
	
}

int writeStudyData(char *fileName, struct studyData *sd){
	FILE * sdFile = fopen(fileName, "wb");
	sd->version = CURRENT_STUDYDATA_VERSION;
	if (sdFile) {
		fwrite(sd, sizeof(*sd), 1, sdFile);
		fclose(sdFile);
		return 0;
	} else return 8;
}

void updateTaskStatus(struct taskInfo *ti, int row, int col){
	
	switch( determineTaskStatus(ti) ){
	
		case TASK_STATUS_COMPLETED:
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_BGCOLOR, successColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_COLOR,   grayColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_TEXT,    checkMark);
			break;
			
		case TASK_STATUS_ERROR:
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_BGCOLOR, errorColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_COLOR,   redColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_TEXT,    errorMark);
			break;
			
		case TASK_STATUS_SCHEDULED:
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_BGCOLOR, successColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_COLOR,   scheduledColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_TEXT,    circleMark);
			break;
			
		case TASK_STATUS_RUNNING:
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_BGCOLOR, successColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_COLOR,   runningColor);
			SetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, row, col, ATTR_LABEL_TEXT,    circleMark);
			break;
			
		default:
			break;
		
	}
}

void setVector(struct vector *v, double x, double y){
	v->x=x;
	v->y=y;
}

struct vector subVector (struct vector a, struct vector b){
	struct vector res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	return res;
}

struct vector addVector (struct vector a, struct vector b){
	struct vector res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	return res;
}

double modulus(struct vector v){
	return sqrt(v.x*v.x + v.y*v.y);
}

/*
	Determines if a fully qualified file name is a directory
	return:  1 - yes, the file is a directory
	         0 - no, the file is not a directory

*/
int isDirectory(char *pathName){
	char foundName[MAX_FILENAME_LEN];
	int retCode;
	
	retCode = GetFirstFile(pathName, 0, 0, 0, 0, 0, 1, foundName);
	if ( 0 == retCode ) return 1;
	else                return 0;
}

/*
	Determines if the directory specified by pathname
	is a regular directory or if it contains a study
	(i.e. has a studyData.bin file)
	
	return:  1 - yes, the directory is a study directory
	         0 - no, the directory is not a study directory

*/
int isStudy(char *pathName){
	int  retCode;
	char studyDataFileName[MAX_FILENAME_LEN];
	
	sprintfWithLock(MAX_FILENAME_LEN, studyDataFileName,"%s\\studyData.bin",pathName);
	
	retCode = GetFirstFile(studyDataFileName,1,0,0,0,0,0,studyDataFileName);
	if ( 0 == retCode ) return 1;
	else                return 0;
}

/*

	Determines if a study (directory) is "young" enough
	to be shown in the study tree
	
	returns true if the study age is <= than maxDays

*/
int isYoungEnough(const char* fileName, int maxDays){
	
	const int secsInADay = 86400;
	
	CmtGetLock(sprintfLock);
	
		time_t now;
		now = time(&now);
	
		// Get file (directory) date
		struct tm fileDate={0};
		fileDate.tm_isdst = -1;
		GetFileDate(fileName, &fileDate.tm_mon, &fileDate.tm_mday, &fileDate.tm_year);
	
		// Adjust to zero origin and convert to time_t
		fileDate.tm_year  = fileDate.tm_year  - 1900;
		fileDate.tm_mon   = fileDate.tm_mon   -    1;
		time_t then       = mktime(&fileDate);

		__int64 daysOld = (now-then)/secsInADay;

	CmtReleaseLock(sprintfLock);
	
	return daysOld <= maxDays;
	
}



/* 
	Determines if the file specified by a fully qualified 
	name exists.
	
	return:  1 - file exists
	         0 - file does not exists

*/
int fileExists(char *fileName){
	int  retCode;
	char retFileName[MAX_FILENAME_LEN];
	
	retCode = GetFirstFile(fileName, 1, 0, 0, 0, 0, 0,retFileName);
	if ( 0 == retCode ) return 1;
	else                return 0;
}

void putToHex(char* out,void* in){
	int i,j;
	(*out)=0;
	for (j=0; j<16; j+=4)
		for (i=j; i<j+4; i++)
			sprintfWithLock(2, out+2*i, "%X", ((unsigned char*)in)[i]); 
}

void LEDControlInitialization(struct LEDControl *meter, int panelID){
	
	meter->panelID           = panelID;
	meter->valueInterval     = abs( (meter->vmax - meter->vmin)/(meter->numLED-1) );
	meter->lastNumLEDtoLite  = 0;

	for (register int i=0; i<meter->numLED; i++){
			SetCtrlAttribute(meter->panelID, meter->LEDIDarray[i], ATTR_DRAW_POLICY,  VAL_DIRECT_TO_SCREEN);
			SetCtrlAttribute(meter->panelID, meter->LEDIDarray[i], ATTR_PICT_BGCOLOR, meter->bgColor);
	}
	
}

void LEDControlTermination(struct LEDControl *meter){
}

void resetLEDControl(struct LEDControl *meter){
	for(register int i=0; i<meter->numLED; i++) {
		SetCtrlAttribute(acquisitionPanel, meter->LEDIDarray[i], ATTR_PICT_BGCOLOR, meter->bgColor);
		CanvasUpdate(    acquisitionPanel, meter->LEDIDarray[i], VAL_ENTIRE_OBJECT);
	}
}

void setLEDControlValue(struct LEDControl *meter, double value){
	register int i; 
	int numLEDtoLite;
	bool maxLEDred=0;

	numLEDtoLite = ceil( (value - meter->vmin)/meter->valueInterval );	//  valid if 0 <= numLEDtoLite <= numLED+1

	// Clip value and remember
	if (numLEDtoLite>meter->numLED){
		numLEDtoLite=meter->numLED;
		maxLEDred = 1;
	} else
	if (numLEDtoLite<0){
		numLEDtoLite = 0;
	}
	
	// Determine which LEDs need to be turned on/off
	if (meter->lastNumLEDtoLite < numLEDtoLite){
		for (i=meter->lastNumLEDtoLite; i<numLEDtoLite; i++){
			SetCtrlAttribute(meter->panelID, meter->LEDIDarray[i], ATTR_PICT_BGCOLOR, meter->LEDColorArray[i]);
			CanvasUpdate(meter->panelID, meter->LEDIDarray[i], VAL_ENTIRE_OBJECT);
		}
	} else
	if (meter->lastNumLEDtoLite > numLEDtoLite){ 
		for (i=meter->lastNumLEDtoLite-1; i>=numLEDtoLite; i--){
			SetCtrlAttribute(meter->panelID, meter->LEDIDarray[i], ATTR_PICT_BGCOLOR, meter->bgColor);
			CanvasUpdate(meter->panelID, meter->LEDIDarray[i], VAL_ENTIRE_OBJECT);
		}
	} else {
		//	meter->lastNumLEDtoLite == numLEDtoLite; nothing to do
	}
	
	if (numLEDtoLite == meter->numLED){
		if (maxLEDred)	SetCtrlAttribute(meter->panelID, meter->LEDIDarray[meter->numLED-1], ATTR_PICT_BGCOLOR, meter->overflowColor);
		else            SetCtrlAttribute(meter->panelID, meter->LEDIDarray[meter->numLED-1], ATTR_PICT_BGCOLOR, meter->LEDColorArray[meter->numLED-1]);
		CanvasUpdate(meter->panelID, meter->LEDIDarray[meter->numLED-1], VAL_ENTIRE_OBJECT);
	}
	
	meter->lastNumLEDtoLite = numLEDtoLite;
}

	

int CVICALLBACK validWindowsFileName_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char c;
	int retCode=0;  // Assume accept by default	
	switch (event)
	{
		case EVENT_KEYPRESS:
				c = GetKeyPressEventCharacter(eventData2);
				switch (c){
					// Virtual key
					case 0:
						break;
					// Character case	
					default:
						if ( strchr("/\\:*?\"<>|.", c) ) retCode=1;  // Swallow event
						break;
				}
				break;
		case EVENT_GOT_FOCUS:
			hideAllRings();
			break;
	}
	return retCode;
}

int CVICALLBACK promptPopupOK_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			answerGiven=1;
			break;
	}
	return 0;
}

void removeTrailingBlanks(char* s){
	char* blank=0;
	int armed=0;
	
	while( *s ){
		if (*s == ' '){
			if (!armed){
				armed=1;
				blank = s;
			}
		}else armed=0;
		s++;
	}
	if (armed) *blank=0;
}

#define WAIT_IMAGE_PERIOD_TICKS 50

DWORD CALLBACK myProgressRoutine(
    LARGE_INTEGER TotalFileSize,
    LARGE_INTEGER TotalBytesTransferred,
    LARGE_INTEGER StreamSize,
    LARGE_INTEGER StreamBytesTransferred,
    DWORD dwStreamNumber,
    DWORD dwCallbackReason,
    HANDLE hSourceFile,
    HANDLE hDestinationFile,
    LPVOID lpData
    ){
		struct waitPanelControlPair *wpcp = (struct waitPanelControlPair*) lpData;
	
		if (0 == wpcp->tickCounter++%WAIT_IMAGE_PERIOD_TICKS){
			wpcp->imageIndex = (wpcp->imageIndex + 1)%wpcp->imageCount;
			SetCtrlVal(wpcp->panel, wpcp->control, wpcp->imageIndex);
			ProcessSystemEvents();
		} 
		return 0;
	
}


int copyFileWithProgress(char* inFilePath, char* outFilePath, struct waitPanelControlPair *wpcp){
	return CopyFileExA(inFilePath, outFilePath, &myProgressRoutine, wpcp, NULL, 1);
}

