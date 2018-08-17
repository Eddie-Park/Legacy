#include <ansi_c.h>

/*#include <cvirte.h>     
#include <userint.h>
#include "constants.h"
#include "globalDecls.h"
#include "toolbox.h"
*/
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

//  LOCAL VARS

static int panelHandle= (int) NULL;
static int (*validDate) (void);


// Two functions to calculate a 1D index in an array
// depending on which of the rows or columns vary fastest 

static int rowFastest(int row, int col, const struct ringControl* rc){
	return  row+col*rc->numRow;
}
static int colFastest(int row, int col, const struct ringControl* rc){
	return  row*rc->numCol+col;
}

static const struct timeControl* currentlyUpdatedControl;

//
// GLOBAL VARS
//
const char *monthNameTable[12]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

struct ringControl yearRingControl   = {NULL,    3, 4, 11, 111,  offsetof(struct tm, tm_year), &rowFastest};
struct ringControl monthsRingControl = {NULL,    3, 4, 11,   0,  offsetof(struct tm, tm_mon),  &rowFastest};
struct ringControl dayRingControl    = {NULL,    6, 6, 30,   1,  offsetof(struct tm, tm_mday), &colFastest};
struct ringControl hourRingControl   = {NULL,    6, 4, 23,   0,  offsetof(struct tm, tm_hour), &colFastest};
struct ringControl minRingControl    = {NULL,   10, 6, 59,   0,  offsetof(struct tm, tm_min),  &colFastest};


void initializeTimeRings(int ph, int yy, int mm, int dd, int hh, int min, int (*f) (void) ){
	panelHandle               = ph;
	yearRingControl.control   = yy;
	monthsRingControl.control = mm;
	dayRingControl.control    = dd;
	hourRingControl.control   = hh;
	minRingControl.control    = min;
	validDate                 = f;
	
	// Set personnalized callback data so that all controls share the same callback functions
	SetCtrlAttribute(panelHandle, yy,   ATTR_CALLBACK_DATA, &yearRingControl  );
	SetCtrlAttribute(panelHandle, mm,   ATTR_CALLBACK_DATA, &monthsRingControl);
	SetCtrlAttribute(panelHandle, dd,   ATTR_CALLBACK_DATA, &dayRingControl   );
	SetCtrlAttribute(panelHandle, hh,   ATTR_CALLBACK_DATA, &hourRingControl  );
	SetCtrlAttribute(panelHandle, min,  ATTR_CALLBACK_DATA, &minRingControl  );
}

void initializeTimeField(int fieldID, const struct timeControl* tc, const struct ringControl* rc){
	struct fieldAssoc* fa= malloc( sizeof(struct fieldAssoc) );
	fa->fieldID = fieldID;
	fa->tc = tc;
	fa->rc = rc;
	SetCtrlAttribute(panelHandle, fieldID, ATTR_CALLBACK_DATA, fa);
	
}

void hideAllRings(void){
	// Hide all panels
	SetCtrlAttribute(panelHandle, yearRingControl.control,   ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, monthsRingControl.control, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, dayRingControl.control,    ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, hourRingControl.control,   ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, minRingControl.control,    ATTR_VISIBLE, 0);
}

const size_t hourOffset = offsetof(struct tm, tm_hour);

void displayTime(const struct timeControl *tc){
	
	time_t tt;
	GetCtrlVal(panelHandle, tc->controlValue, &tt);
	
	CmtGetLock(sprintfLock);
		struct tm* ts = localtime(&tt);
		char strYear[5];
		sprintf(strYear, "%d", ts->tm_year+1900);
		char strDay[4];
		sprintf(strDay, "%d", ts->tm_mday+100);
		char strHour[4];
		sprintf(strHour, "%d", ts->tm_hour+100);
		char strMin[4];
		sprintf(strMin, "%d", ts->tm_min+100);
 	CmtReleaseLock(sprintfLock);

	SetCtrlVal(panelHandle ,tc->year,   strYear);
	SetCtrlVal(panelHandle ,tc->month,  monthNameTable[ts->tm_mon]);
 	SetCtrlVal(panelHandle ,tc->day,    strDay+1);
	SetCtrlVal(panelHandle ,tc->hour,   strHour+1);
 	SetCtrlVal(panelHandle ,tc->min,    strMin+1);

}



static inline void prepareRingForDisplay(const struct ringControl *rc, int eventData1, int eventData2){
 	
	// Recover current value of the time field
	time_t tt;
	GetCtrlVal(panelHandle, currentlyUpdatedControl->controlValue, &tt);
	
	CmtGetLock(sprintfLock);
		// Convert to struct tm
		struct tm* ts       = localtime(&tt);
	
		// Calculate the index in the ring from the sub-field (day, month ...) value
		char*  fieldCharPtr = (char*)ts + rc->offset ;
		int *  fieldIntPtr  = (int*) fieldCharPtr;
		int    index     = (*fieldIntPtr)  -   rc->origin ;
 	CmtReleaseLock(sprintfLock);
	
	SetCtrlIndex(panelHandle, rc->control, index );
	
	/* eventData1 = mouse vertical pos., */
 	/* eventData2 = mouse horizontal pos. */
	SetCtrlAttribute(panelHandle, rc->control, ATTR_VISIBLE, 1);
	SetActiveCtrl(panelHandle, rc->control);	  // Note: control must be visible for this to work
	SetCtrlAttribute(panelHandle, rc->control, ATTR_TOP,  eventData1);
	SetCtrlAttribute(panelHandle, rc->control, ATTR_LEFT, eventData2);
	SetCtrlAttribute(panelHandle, rc->control, ATTR_ZPLANE_POSITION, 0);
	
}


static int ringControlLeftClickHandler(const struct ringControl *rc, int eventData1, int eventData2){

	int top, left, width, height, col, row, index;  
	
	/* eventData1 = mouse vertical pos., */
 	/* eventData2 = mouse horizontal pos. */
	
	GetCtrlAttribute(panelHandle, rc->control, ATTR_TOP,    &top);
	GetCtrlAttribute(panelHandle, rc->control, ATTR_LEFT,   &left);
	GetCtrlAttribute(panelHandle, rc->control, ATTR_WIDTH,  &width);
	GetCtrlAttribute(panelHandle, rc->control, ATTR_HEIGHT, &height);
	
	// Calculate the row and column where the user clicked
	col=( rc->numCol * (eventData2-left))/width;
	row=( rc->numRow * (eventData1-top ))/height;
	
	// Calculate the 1D index to which these correspond
	index = (*(rc->indexFunc))(row, col, rc); 
	index = (index<=rc->maxIndex)?index:rc->maxIndex;
	
	// Set the corresponding image
	SetCtrlIndex(panelHandle, rc->control, index);
	return index;

	
}

// ---------------------------------

// Callback function when you click on one of the time (sub) fields (year, month, day, hour, min)
// in any of the time fields (assay, administration)

int CVICALLBACK timeField_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	struct fieldAssoc *fa   = callbackData;
	switch(event){
		
		case EVENT_RIGHT_CLICK:
			hideAllRings();			
			// Recover field and control IDS
			currentlyUpdatedControl = fa->tc ;
			prepareRingForDisplay(fa->rc, eventData1, eventData2);
			break;
			
	}
	
	return 0;
}


// Callback function when you click inside one of the picture rings (months, years, etc)
// The function determines where you clicked, and from that updates the picture and
// the actual value of the associated field

int CVICALLBACK ring_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	struct ringControl* rc = callbackData;
	switch (event){

					int index;

		case EVENT_LEFT_CLICK:
			
			index = ringControlLeftClickHandler(rc, eventData1, eventData2);
			
			// Get value of currently update field		
			time_t tt;
			GetCtrlVal(panelHandle, currentlyUpdatedControl->controlValue, &tt);
			
			CmtGetLock(sprintfLock);
				// .. convert to struct tm
				struct tm* ts = localtime(&tt);

				// Calculate offset of subfield and update
				char*  fieldCharPtr = (char*)ts + rc->offset ;
				int *  fieldIntPtr  = (int*) fieldCharPtr;
				(*fieldIntPtr)      = index +  rc->origin ;
	 
				// Make time from that updated structure
				time_t savedtt = tt;
				tt                = mktime(ts);
 			CmtReleaseLock(sprintfLock);
			
			// Tentative update time field and display
			SetCtrlVal(panelHandle, currentlyUpdatedControl->controlValue, tt);
			
			// Validate that admin time is greater or equal to assay time otherwise revert
			if ( (*validDate)() ) SetCtrlVal(panelHandle, currentlyUpdatedControl->controlValue, savedtt);    
			displayTime(currentlyUpdatedControl);
			
 			SetCtrlAttribute(panelHandle, rc->control, ATTR_VISIBLE, 0);

			break;
			
		case EVENT_LEFT_DOUBLE_CLICK:
		case EVENT_LOST_FOCUS:
			SetCtrlAttribute(panelHandle, rc->control, ATTR_VISIBLE, 0);
			break;
	}
	return 0;
}

