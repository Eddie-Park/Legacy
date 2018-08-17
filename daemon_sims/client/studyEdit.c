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
#include <formatio.h>
#include <ansi_c.h>
#include <utility.h>
#include <userint.h>
#include "constants.h"
#include "typedefs.h"
#include "globalDecls.h"
#include "genisys4GUI.h"

#include "studyEdit.h"
#include "studyTreeManagement.h"
#include "utilities.h"
#include "logManagement.h"
#include "timeRing.h"


struct timeControl adminTimeControl={STUDY_EDIT_adminTimeValue, 
									 STUDY_EDIT_adminTimeYear, 
									 STUDY_EDIT_adminTimeMonth, 
									 STUDY_EDIT_adminTimeDay,   
									 STUDY_EDIT_adminTimeHour, 
									 STUDY_EDIT_adminTimeMin};

struct timeControl assayTimeControl={STUDY_EDIT_assayTimeValue, 
									 STUDY_EDIT_assayTimeYear, 
									 STUDY_EDIT_assayTimeMonth, 
									 STUDY_EDIT_assayTimeDay,   
									 STUDY_EDIT_assayTimeHour, 
									 STUDY_EDIT_assayTimeMin};

// Validates that the assay date/time is anterior to injection time
static int validateDates(void){
	int adminValue, assayValue;
	GetCtrlVal(studyEditPanel, STUDY_EDIT_adminTimeValue, &adminValue);
	GetCtrlVal(studyEditPanel, STUDY_EDIT_assayTimeValue, &assayValue);
	return (assayValue>adminValue); 	
  
}

int studyEditInitialization(void){

	initializeTimeRings(studyEditPanel, 
		                STUDY_EDIT_yearRing, 
						STUDY_EDIT_monthsRing, 
		 				STUDY_EDIT_dayRing, 
						STUDY_EDIT_hourRing, 
						STUDY_EDIT_minRing, 
						&validateDates );

	// Initialize the assay time field
	initializeTimeField(STUDY_EDIT_assayTimeYear,  &assayTimeControl, &yearRingControl);
	initializeTimeField(STUDY_EDIT_assayTimeMonth, &assayTimeControl, &monthsRingControl);
	initializeTimeField(STUDY_EDIT_assayTimeDay,   &assayTimeControl, &dayRingControl);
	initializeTimeField(STUDY_EDIT_assayTimeHour,  &assayTimeControl, &hourRingControl);
	initializeTimeField(STUDY_EDIT_assayTimeMin,   &assayTimeControl, &minRingControl);

	// Initialize the administration time field
	initializeTimeField(STUDY_EDIT_adminTimeYear,  &adminTimeControl, &yearRingControl);
	initializeTimeField(STUDY_EDIT_adminTimeMonth, &adminTimeControl, &monthsRingControl);
	initializeTimeField(STUDY_EDIT_adminTimeDay,   &adminTimeControl, &dayRingControl);
	initializeTimeField(STUDY_EDIT_adminTimeHour,  &adminTimeControl, &hourRingControl);
	initializeTimeField(STUDY_EDIT_adminTimeMin,   &adminTimeControl, &minRingControl);

	
	return 0;
 	
}

void studyEditTermination(void){
}

void studyDataToPanel(struct studyData *s ){
	SetCtrlVal(studyEditPanel,STUDY_EDIT_NAME,         s->name);
	SetCtrlVal(studyEditPanel,STUDY_EDIT_TAG,          s->tag );
	
	SetCtrlVal(studyEditPanel, STUDY_EDIT_adminTimeValue, s->adminTime);
  	displayTime(&adminTimeControl);
	
	SetCtrlVal(studyEditPanel, STUDY_EDIT_assayTimeValue, s->assayTime);
  	displayTime(&assayTimeControl);
	
	float activity = s->assayActivity / localeArray[locale]->activityToBqFactor;
	SetCtrlVal(studyEditPanel,STUDY_EDIT_ASSAY_ACTIVITY,   activity);
	
	SetCtrlVal(studyEditPanel,STUDY_EDIT_INVESTIGATOR, s->investigator);
	SetCtrlVal(studyEditPanel,STUDY_EDIT_SUBJECT_ID,   s->subject);
	SetCtrlVal(studyEditPanel,STUDY_EDIT_COMPOUND,     s->compound);
}

void panelToStudyData(struct studyData *s){
	GetCtrlVal(studyEditPanel,STUDY_EDIT_NAME,             s->name);
	removeTrailingBlanks(s->name);
	GetCtrlVal(studyEditPanel,STUDY_EDIT_TAG,              s->tag );
	
	GetCtrlVal(studyEditPanel,STUDY_EDIT_adminTimeValue,  &s->adminTime); 
	GetCtrlVal(studyEditPanel,STUDY_EDIT_assayTimeValue,  &s->assayTime); 
	
	double activity;
	GetCtrlVal(studyEditPanel,STUDY_EDIT_ASSAY_ACTIVITY, &activity);
	s->assayActivity = activity * localeArray[locale]->activityToBqFactor;
	
	GetCtrlVal(studyEditPanel,STUDY_EDIT_INVESTIGATOR, s->investigator);
	GetCtrlVal(studyEditPanel,STUDY_EDIT_SUBJECT_ID,   s->subject);
	GetCtrlVal(studyEditPanel,STUDY_EDIT_COMPOUND,     s->compound);
}


	  
// Determine if the save button should be enabled or disabled
int setSaveButtonStatus(void){
	char studyName     [MAX_FILE_NAME_LENGTH]="";
	char investigator  [MAX_FILE_NAME_LENGTH]="";
	char subject       [MAX_FILE_NAME_LENGTH]="";
	char compound      [MAX_FILE_NAME_LENGTH]="";
	
	GetCtrlVal(studyEditPanel, STUDY_EDIT_NAME,          studyName    );
	GetCtrlVal(studyEditPanel, STUDY_EDIT_INVESTIGATOR,  investigator );
	GetCtrlVal(studyEditPanel, STUDY_EDIT_SUBJECT_ID,    subject      );
	GetCtrlVal(studyEditPanel, STUDY_EDIT_COMPOUND,      compound     );

	if ( 
		*studyName     !=0 &&
		*investigator  !=0 &&
		*subject       !=0 &&
		*compound      !=0 
		) {
		// Allow save
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_CTRL_MODE, VAL_HOT);
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_LEFT,     218);  // Move save button in place
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_VISIBLE,    1);  // ... and show
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_saveMask, ATTR_LEFT, 400);         // Move mask to the side to show save button
		return 1;
	} else 	{
		// Don't allow save
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_CTRL_MODE, VAL_INDICATOR);   
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_LEFT,     130);  // Move save button out of place
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_VISIBLE,    0);  //  .. and hide
		SetCtrlAttribute(studyEditPanel, STUDY_EDIT_saveMask,    ATTR_LEFT,     200);  // Move mask on top of save button image 
		return 0;
	}

}

/*
	Invoked by study popup menu

*/

void  showStudy(void){
	
	if (maxStudyAge == 30 ) {
		maxStudyAge = 99999;
		SetMenuBarAttribute(studyBarHandle, studyPopShow, ATTR_ITEM_NAME, "Show Recent" );
		DeleteListItem  (studyTabHandle, TAB_STUDY_TREE, 1, -1);
		createStudyTree();
	}
	else {
		maxStudyAge = 30;
		SetMenuBarAttribute(studyBarHandle, studyPopShow, ATTR_ITEM_NAME, "Show All" );
		DeleteListItem  (studyTabHandle, TAB_STUDY_TREE, 1, -1);
		createStudyTree();
	}
}


void editStudy(void){
	char   studyPath [MAX_FILE_NAME_LENGTH];
	char   studyDataFileName [MAX_FILE_NAME_LENGTH];
	struct studyData s;
	
	editStudyPanelReason = EDIT;
	
	if (0!=makeStudyPathName(lastTag, studyPath)){
		writeToLog("studyEdit::editStudy - ERROR, invalid index making study path name");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH, studyDataFileName, "%s\\studyData.bin",studyPath);
	
	if (0!=readStudyData(studyDataFileName, &s)){
		writeToLog("studyEdit::editStudy - ERROR, I/O error reading study data");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}
	studyDataToPanel(&s);
	
	// Can't change name 
	SetCtrlAttribute(studyEditPanel, STUDY_EDIT_NAME, ATTR_DIMMED,1);
	setSaveButtonStatus();
	SetActiveCtrl(studyEditPanel, STUDY_EDIT_INVESTIGATOR);
	fadeInPanel(&studyEditPanelFading);   
	
}

int CVICALLBACK studyDataSave_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char   studyPath         [MAX_FILE_NAME_LENGTH];
	char   studyDataFileName [MAX_FILE_NAME_LENGTH];
	char   temp[512];
	int    newIndex;
	struct studyData s;

	switch (event)
	{
		
		case EVENT_COMMIT:
			fadeOutPanel(&studyEditPanelFading);
			panelToStudyData(&s);
			
			if ( *(s.name) == 0 ) break;
			switch (editStudyPanelReason){
				case NEW:
						
					// now is a good time to generate a unique ID
					makeUniqueID(s.tag);	
					newIndex   = InsertTreeItem(studyTabHandle,TAB_STUDY_TREE,VAL_CHILD, newStudyParentIndex, VAL_FIRST, s.name, s.tag, 0, 0);
					setItemAttributes(newIndex, theColor, theBGColor, STUDY_IMAGE_INDEX); 
					if (0!=makeStudyPathName(s.tag, studyPath)){
						sprintfWithLock(512, temp, "studyDataSave_CB internal error - Can't make study path from tag(%s)", s.tag);
						writeToLog(temp);
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
						return 8;
					}
					if ( 0!= MakeDir(studyPath)) {
						writeToLog("studyEdit::studyDataSave_CB - ERROR, MakeDir() failed, study not created"); 
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
						return 8;
					}
					break;
					
				case EDIT:
					if (0!=makeStudyPathName(lastTag, studyPath)){
						writeToLog("studyEdit::studyDataSave_CB - ERROR, invalid tag when saving existing study");
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
						return 8;
					}
					break;
			}

			sprintfWithLock(MAX_FILE_NAME_LENGTH, studyDataFileName, "%s\\studyData.bin",studyPath);

			if (0!=writeStudyData(studyDataFileName, &s)){
				writeToLog("studyEdit::studyDataSave_CB - ERROR, I/O error saving study data");
				MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			}
			break;
	}
	return 0;
}


int CVICALLBACK studyDataCancel_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		
		case EVENT_LEFT_CLICK_UP:
			SetCtrlBitmap(panel, control, 0, 0);
			break;

			case EVENT_COMMIT:
			fadeOutPanel(&studyEditPanelFading);
			break;
	}
	return 0;
}




int CVICALLBACK checkCompleteness_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_KEYPRESS:
			setSaveButtonStatus();
			break;
	}
	return 0;
}
