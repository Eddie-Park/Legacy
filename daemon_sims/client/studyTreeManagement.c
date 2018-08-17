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
#include <formatio.h>
#include <ansi_c.h>
#include <utility.h>
#include <userint.h>

#include "constants.h"
#include "utilities.h"
#include "logManagement.h"
#include "studyTreeManagement.h"
#include "menuManagement.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "studyEdit.h"

static char tempString [2048];

static void recursiveInsert(char *path, int parentIndex);
static void setStudyStatusInStudyTree(void);
static void addColumn(char *text, int colWidth);

void makeFilePathFromProtocol(char *protocolName, char *result);
void prepareProtocolForDisplay(char *protocolName);

extern const char *monthNameTable[12];

/* 
	Study Management Initialization
*/
int studyTreeManagementInitialization(void){
  	int histIndex;
	
	SetTreeColumnAttribute(studyTabHandle, TAB_STUDY_TREE, 0,ATTR_COLUMN_WIDTH, 265);
	SetTreeColumnAttribute(studyTabHandle, TAB_STUDY_TREE, 0,ATTR_VERTICAL_GRID_VISIBLE,1); 
	SetCtrlAttribute      (studyTabHandle, TAB_STUDY_TREE, ATTR_SCROLL_BAR_COLOR, VAL_LT_GRAY); 
	SetTreeCellAttribute  (studyTabHandle, TAB_STUDY_TREE, VAL_DFLT_FOR_NEW_OBJECTS, 0, ATTR_LABEL_TEXT,   "");
	
	addColumn("ACQ",   60);
	addColumn("SINO",  60);
	addColumn("STACK",  2);
	addColumn("IMAGE", 60);

	histIndex = createStudyTree();
	SetActiveTreeItem(studyTabHandle,TAB_STUDY_TREE,histIndex,VAL_REPLACE_SELECTION_WITH_ITEM);
	return 0;
}

void studyTreeManagementTermination(void){
	// nothing to do
}

static void addColumn(char *text, int colWidth){
	int colIndex;
	
	colIndex = InsertTreeColumn(studyTabHandle, TAB_STUDY_TREE, -1, text);
	if (colIndex>=0){
	 	SetTreeColumnAttribute(studyTabHandle,TAB_STUDY_TREE, colIndex, ATTR_COLUMN_WIDTH,  colWidth);   
	 	SetTreeColumnAttribute(studyTabHandle,TAB_STUDY_TREE, colIndex, ATTR_LABEL_COLOR,   VAL_BLACK);
	 	SetTreeColumnAttribute(studyTabHandle,TAB_STUDY_TREE, colIndex, ATTR_VERTICAL_GRID_VISIBLE,1);
	}
}

/* 
	Create study tree
*/
int createStudyTree(void){
	int allIndex;

	/* Get top index and color attributes */
	GetTreeItemFromTag  (studyTabHandle,TAB_STUDY_TREE,"all",&allIndex);
	GetTreeItemAttribute(studyTabHandle,TAB_STUDY_TREE, allIndex,ATTR_LABEL_BGCOLOR,&theBGColor);
	GetTreeItemAttribute(studyTabHandle,TAB_STUDY_TREE, allIndex,ATTR_LABEL_COLOR,  &theColor);
	
	// setup search mask and populate tree
	sprintfWithLock(1024, tempString,"%s\\*",studyFolder);
	recursiveInsert(tempString, allIndex);
	setStudyStatusInStudyTree();
	
	return allIndex;
}

/* 
	Insert studies(they are directories) and regular directories
*/
static void recursiveInsert(char *path, int parentIndex){
	register int i;
	int size, newIndex;
	char nextLevel   [MAX_PATHNAME_LEN];
	char currentLevel[MAX_PATHNAME_LEN];
	char dirName     [MAX_DIRNAME_LEN];
	char fullyQualifiedName [MAX_FILENAME_LEN];
	char studyDataName     [MAX_FILENAME_LEN];
	char driveName   [10];
	char **nameArray;
	char *lastLevelPtr;
	struct studyData s;

	// Insert Directory names
	nameArray = createDirList(path,1,&size);
	if (nameArray ==NULL) return;
	
	SplitPath(path, driveName, dirName, 0);
	for(i=0; i<size; i++){
		sprintfWithLock(MAX_FILENAME_LEN, fullyQualifiedName,"%s%s%s",driveName, dirName, nameArray[i]);
		
 		if( isDirectory(fullyQualifiedName)) {
			
			newIndex=-1;
			
			// Directory is a study
			if ( isStudy(fullyQualifiedName) ) {
				if ( isYoungEnough(fullyQualifiedName, maxStudyAge) ){
					sprintfWithLock(MAX_FILENAME_LEN, studyDataName, "%s\\studyData.bin", fullyQualifiedName);
					if ( 0!= readStudyData(studyDataName, &s)) {
						sprintfWithLock(2048, tempString,"studyTreeManagement::recursiveInsert - Error reading studyData %s\n",studyDataName); 
						writeToLog(tempString);
					}
					newIndex   = InsertTreeItem(studyTabHandle,TAB_STUDY_TREE,VAL_CHILD, parentIndex, VAL_LAST, nameArray[i], s.tag, 0, 0);
					setItemAttributes(newIndex, theColor, theBGColor, STUDY_IMAGE_INDEX); 
				}
			}
			
			// Directory is a folder of studies
			else {
				char returnedTag[MAX_TREE_ITEM_TAG_LEN];
				newIndex   = InsertTreeItem(studyTabHandle,TAB_STUDY_TREE,VAL_CHILD, parentIndex, VAL_LAST, nameArray[i], nameArray[i], returnedTag, 0);
				setItemAttributes(newIndex, theColor, theBGColor, DIR_IMAGE_INDEX); 
			}
			
			
			if (newIndex>=0){
				myStrcpy(currentLevel,path);
				lastLevelPtr = strrchr(currentLevel,'\\');
				if (lastLevelPtr){
					(*lastLevelPtr)=0;  // set the end of string there, leaving the current directory
					sprintfWithLock(MAX_PATHNAME_LEN, nextLevel,"%s\\%s\\*",currentLevel,nameArray[i]);
					recursiveInsert(nextLevel, newIndex);
				}
			}
		} 
		
	}
	freeDirList(nameArray,size);
}

/*
	Set the status of every column for every study
*/
static void setStudyStatusInStudyTree(void){
	int  count;
	int  i;
	char studyPathName    [MAX_FILENAME_LEN];
	char taskListName     [MAX_FILENAME_LEN];
	struct taskList tl;
	char tag[MAX_TREE_ITEM_TAG_LEN];
	 
	if (0<=GetNumTreeItems(studyTabHandle,TAB_STUDY_TREE, VAL_ALL, 0, 0, VAL_NEXT_PLUS_SELF, 0, &count)){
		for (i=1; i<count; i++){
			GetTreeItemTag(studyTabHandle,TAB_STUDY_TREE,i, tag);
			if(0==makeStudyPathName(tag, studyPathName)){
				sprintfWithLock(MAX_FILENAME_LEN, taskListName, "%s\\taskList.bin", studyPathName);
				if (fileExists(taskListName)){
					if (0==readTaskList(taskListName, &tl)){
						
						/* Update status. Don't rely on study index on disk */
						if (tl.prot.acq)   updateTaskStatus(&tl.acqInfo,   i, COLUMN_ACQ);
						if (tl.prot.hist)  updateTaskStatus(&tl.histInfo,  i, COLUMN_HIST);
						if (tl.prot.recon) updateTaskStatus(&tl.reconInfo, i, COLUMN_RECON);
						
					}
				}
			}
		}
	}
}


void setItemAttributes(int newItemIndex, int theColor, int theBGColor, int imageIndex){
	
	SetTreeItemAttribute(studyTabHandle,TAB_STUDY_TREE,newItemIndex,ATTR_LABEL_BGCOLOR,theBGColor);
	SetTreeItemAttribute(studyTabHandle,TAB_STUDY_TREE,newItemIndex,ATTR_LABEL_COLOR,theColor);
	SetTreeItemAttribute(studyTabHandle,TAB_STUDY_TREE,newItemIndex,ATTR_IMAGE_INDEX, imageIndex);
	
	if (imageIndex == STUDY_IMAGE_INDEX) SetTreeItemAttribute(studyTabHandle,TAB_STUDY_TREE,newItemIndex,ATTR_NO_EDIT_LABEL, 1);
	
	for(register int j=1; j<=4; j++){
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_COLOR,   theColor);
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_BGCOLOR, theBGColor);
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_FONT,"Webdings");
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_BOLD,0); // was 1
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_POINT_SIZE, 16); //was 10
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_COLOR,theColor);
		SetTreeCellAttribute(studyTabHandle, TAB_STUDY_TREE, newItemIndex, j, ATTR_LABEL_JUSTIFY,VAL_CENTER_CENTER_JUSTIFIED);
	}
}

/*
	Set dim status of items in the popup menu
	1: means dim item, 0: means enable item
*/
static void setCtrlMenuDim(int new, int rename, int show, int run, int edit, int archive, int delete, int view){
	SetMenuBarAttribute(studyBarHandle,studyPopNew,     ATTR_DIMMED, new);
	SetMenuBarAttribute(studyBarHandle,studyPopRename,  ATTR_DIMMED, rename);
	SetMenuBarAttribute(studyBarHandle,studyPopShow,    ATTR_DIMMED, show);
	SetMenuBarAttribute(studyBarHandle,studyPopRun,     ATTR_DIMMED, run);
	SetMenuBarAttribute(studyBarHandle,studyPopEdit,    ATTR_DIMMED, edit);
	SetMenuBarAttribute(studyBarHandle,studyPopArchive, ATTR_DIMMED, archive);
	SetMenuBarAttribute(studyBarHandle,studyPopDelete,  ATTR_DIMMED, delete);
	SetMenuBarAttribute(studyBarHandle,studyPopView,    ATTR_DIMMED, view);
}

/*
	Determines if the study identified by index is "empty",
	i.e. nothing has run in it yet.
	
	return: 1 - study is empty
	        0 - study is not empty
*/
static int isStudyEmpty(int index){
	int i;
	int color;
	
	for (i=1; i<=4; i++){
		GetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, index, i, ATTR_LABEL_BGCOLOR, &color);
		if(color == theBGColor) continue;
		else return 0;
	}
	return 1;
}

static void displayStudyData(void){
	struct studyData s;
	struct taskList tl;
	
	char studyPath[MAX_FILE_NAME_LENGTH];
	char studyDataFileName[MAX_FILE_NAME_LENGTH];
	char taskListFileName[MAX_FILE_NAME_LENGTH];
	char info[1024];
	char strYear[5];
	char strDay[4];
	char strHour[4];
	char strMin[4];
	struct tm* ts;
	
	if (0!=makeStudyPathName(lastTag, studyPath)) return;
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH,studyDataFileName, "%s\\studyData.bin",studyPath);
	if (0!=readStudyData(studyDataFileName, &s))return;

	*(tl.prot.name)  = 0;
	*(tl.prot.frameSequence) = 0;
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH,taskListFileName, "%s\\taskList.bin",studyPath);
	readTaskList(taskListFileName, &tl);
	
	// Nicer formatting than ctime()
	CmtGetLock(sprintfLock);
	
		ts = localtime(&s.assayTime);
		sprintf(strYear, "%d", ts->tm_year+1900);
		sprintf(strDay, "%d", ts->tm_mday+100);
		sprintf(strHour, "%d", ts->tm_hour+100);
		sprintf(strMin, "%d", ts->tm_min+100);
		char cAssayTime[32];
		sprintf(cAssayTime, "%s %s %s - %s:%s", strYear, monthNameTable[ts->tm_mon],  strDay+1, strHour+1,  strMin+1);
 
		ts = localtime(&s.adminTime);
		sprintf(strYear, "%d", ts->tm_year+1900);
		sprintf(strDay,  "%d", ts->tm_mday+100);
		sprintf(strHour, "%d", ts->tm_hour+100);
		sprintf(strMin,  "%d", ts->tm_min+100);
		char cAdminTime[32];
		sprintf(cAdminTime, "%s %s %s - %s:%s", strYear, monthNameTable[ts->tm_mon],  strDay+1, strHour+1,  strMin+1);
 
	CmtReleaseLock(sprintfLock);
	
	char cAcquisitionStartTime[32]="";
	if (tl.acqInfo.startTime) {
		CmtGetLock(sprintfLock);
			ts = localtime(&tl.acqInfo.startTime);
			sprintf(strYear, "%d", ts->tm_year+1900);
			sprintf(strDay,  "%d", ts->tm_mday+100);
			sprintf(strHour, "%d", ts->tm_hour+100);
			sprintf(strMin,  "%d", ts->tm_min+100);
			sprintf(cAcquisitionStartTime, "%s %s %s - %s:%s", strYear, monthNameTable[ts->tm_mon],  strDay+1, strHour+1,  strMin+1);
		CmtReleaseLock(sprintfLock);
	}
	
	char cAcquisitionEndTime[32]="";
	if (tl.acqInfo.endTime) {
		CmtGetLock(sprintfLock);
			ts = localtime(&tl.acqInfo.endTime);
			sprintf(strYear, "%d", ts->tm_year+1900);
			sprintf(strDay,  "%d", ts->tm_mday+100);
			sprintf(strHour, "%d", ts->tm_hour+100);
			sprintf(strMin,  "%d", ts->tm_min+100);
			sprintf(cAcquisitionEndTime, "%s %s %s - %s:%s", strYear, monthNameTable[ts->tm_mon],  strDay+1, strHour+1,  strMin+1);
		CmtReleaseLock(sprintfLock);
	}

	// Clip fields
	*(s.name+28)                = 0;
	*(s.investigator+23)        = 0;
	*(s.subject+25)             = 0;
	*(tl.prot.frameSequence+18) = 0;	  // Not displayed anymore
	
	sprintfWithLock(1024, info, 
		"Name: %s\nInvestigator: %s\nSubject: %s\nCompound: %s\nAssay activity: %w %s\nAssay time:  %s\nAdmin. time: %s\nStart time: %s\nEnd time: %s\nProtocol:%s", 
			s.name, 
			s.investigator, 
			s.subject, 
			s.compound, 
			(s.assayActivity/localeArray[locale]->activityToBqFactor), 
			localeArray[locale]->activityUnits,
			cAssayTime, 
			cAdminTime,
			cAcquisitionStartTime,
			cAcquisitionEndTime,
			tl.prot.name
			);
  
	SetCtrlVal(peekPanel, PEEK_PANEL_STUDY_DATA, info);
	DisplayPanel(peekPanel);
	

}

int CVICALLBACK studyTree_CB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	char cellContents[128]={0};
	switch (event)
	{
		
		case EVENT_KEYPRESS:
			if (eventData1 == VAL_F2_VKEY){
				GetActiveTreeItem(panel, control, &lastIndex);
				GetLabelFromIndex(panel, control, lastIndex, oldName);       // save old name  for later
			}
			break;
			
		case EVENT_COMMIT:
			if (eventData1 == LABEL_CHANGE){
				
				// Make sure new name does not contain trailing blanks
				char newName[MAX_PATHNAME_LEN];
				GetLabelFromIndex(panel, control, eventData2, newName);
				removeTrailingBlanks(newName);					
				SetTreeItemAttribute(panel, control, eventData2, ATTR_LABEL_TEXT,  newName); 

				char oldPathName[MAX_PATHNAME_LEN];
				char newPathName[MAX_PATHNAME_LEN];
					
				sprintfWithLock(MAX_PATHNAME_LEN, oldPathName,"%s\\%s", studyFolder, oldName);
				sprintfWithLock(MAX_PATHNAME_LEN, newPathName,"%s\\%s", studyFolder, newName);
				
				if (0 != rename(oldPathName, newPathName) ){
					char temp[1024];
					sprintfWithLock(1024, temp, "studyTreeManagement::studyTree_CB - ERROR, rename study directory failed. old:%s, new:%s", oldPathName, newPathName);
					writeToLog(temp);
					MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
				}
  				
			}
			break;
		
		case EVENT_RIGHT_CLICK:				  
			lastRightClickPoint = MakePoint(eventData2, eventData1);
			GetIndexFromPoint(panel,control, lastRightClickPoint, &lastIndex, &lastArea, &lastColumn);
			if (lastIndex <0) return 1;
			GetTreeItemTag(panel, control, lastIndex, lastTag);
			GetTreeItemLevel(panel,control,lastIndex,&lastImageIndex);
			/* Adjust (dim or not dim) menu items   */
			switch (lastArea){
				/* Close enough to an item */
				case VAL_ITEM_PLUS_MINUS_AREA:
				case VAL_ITEM_MARK_AREA:
				case VAL_ITEM_IMAGE_AREA:
				case VAL_ITEM_LABEL_AREA:
					
					//  new,  rename, show, run,  edit,  archive,  delete,  view
					//  1: means dim (unavailable)
					
					switch (lastImageIndex){
						
						case ALL_CATEGORY_INDEX:
							setCtrlMenuDim(0, 1, 1, 1, 1, 1, 1, 1);
							break;

						case DIR_IMAGE_INDEX:
							setCtrlMenuDim(0, 0, 0, 1, 1, 1, 0, 1);
							break;

						case STUDY_IMAGE_INDEX:
							GetTreeCellAttribute(panel, control, lastIndex, COLUMN_RECON, ATTR_LABEL_TEXT, cellContents);
							if ( 0==strcmp(cellContents,checkMark) )  setCtrlMenuDim(1, 1, 1, 0, 1-isStudyEmpty(lastIndex), 0, 0, 0);
							else                                      setCtrlMenuDim(1, 1, 1, 0, 1-isStudyEmpty(lastIndex), 0, 0, 1);
							break;
					}
					break;
				
				/* Within a cell: enable view actions */
				case VAL_CELL_AREA:
					GetTreeCellAttribute(panel, control, lastIndex, lastColumn, ATTR_LABEL_TEXT, cellContents);
					if      (lastColumn == COLUMN_HIST  && 0==strcmp(cellContents,checkMark)) setCtrlMenuDim(1, 1, 1, 1, 1, 1, 1, 1);
					else if (lastColumn == COLUMN_RECON && 0==strcmp(cellContents,checkMark)) setCtrlMenuDim(1, 1, 1, 1, 1, 1, 1, 0);
					else return 1;
					break;
				
				default:
					break;
			} // end switch lastArea
			
			RunPopupMenu(studyBarHandle, studyTabPopupMenu, studyTabHandle, eventData1, eventData2, 0,0,0,0);
			return 1; // swallow
			//break;
			
		case EVENT_LEFT_CLICK:
			lastRightClickPoint = MakePoint(eventData2, eventData1);
			GetIndexFromPoint(panel,control, lastRightClickPoint, &lastIndex, &lastArea, &lastColumn);
			if (lastIndex <0) return 0;
			GetTreeItemTag(panel, control, lastIndex, lastTag);
			GetTreeItemLevel(panel,control,lastIndex,&lastImageIndex);
			if (lastImageIndex == 2) {
				int tabTop, tabLeft;
				int mainTop, mainLeft;
				GetCtrlAttribute (mainPanel, MAIN_PANEL_mainTAB, ATTR_TOP, &tabTop);    
				GetCtrlAttribute (mainPanel, MAIN_PANEL_mainTAB, ATTR_LEFT, &tabLeft);     
				GetPanelAttribute(mainPanel, ATTR_TOP,  &mainTop);
				GetPanelAttribute(mainPanel, ATTR_LEFT, &mainLeft);
				SetPanelPos(peekPanel, mainTop+tabTop+eventData1, mainLeft+tabLeft+eventData2-300); 
				displayStudyData();
			}
			break;
			
		case EVENT_LEFT_CLICK_UP:
			HidePanel(peekPanel);
			break;

			
			
	}// end switch event
	
	return 0;
}

