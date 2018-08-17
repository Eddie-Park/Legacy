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

#include "menuManagement.h"
#include "studyTreeManagement.h"
#include "protocolTreeManagement.h"
#include "protocolEdit.h"
#include "runManagement.h"
#include "studyEdit.h"
#include "utilities.h"
#include "logManagement.h"
#include "cameraManagement.h"
#include "preferenceManagement.h"
#include "communicationsManagement.h"
#include "scriptManagement.h"
#include "timeRing.h"

DeclareThreadSafeScalarVar (int, ProtocolHandlerThreadCounter);

#define MAX_SUBMENU 50

static char tempString [2048];

static void recursiveInsertMenu(int index, int menuBarHandle, int menuID);
static void deleteSubMenus(void);
static void addToRecentProtocol(char *tag);
static void saveRecentProtocols(void);
static void createToolTips(void);


void CVICALLBACK studyTabPopupCallback    (int menuBarHandle, int menuItemID, void *callbackData, int panelHandle);
void CVICALLBACK studyTabRunCallback      (int menuBarHandle, int menuItemID, void *callbackData, int panelHandle);
void CVICALLBACK protocolTabPopupCallback (int menuBarHandle, int menuItemID, void *callbackData, int panelHandle);
void CVICALLBACK quit_CB (int menuBar, int menuItem, void *callbackData, int panel);

void CVICALLBACK peekStudyCallback (int menuBarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// in studyEdit.c
extern struct timeControl adminTimeControl;
extern struct timeControl assayTimeControl;

 /* Create  POPUP menus for tabs */

int menuManagementInitialization(void){
	
	readRecentProtocols();
	createToolTips();
	
	/* Keep track of all created submenus */
	subMenuCount=-1;
	subMenuArray  = malloc(MAX_SUBMENU * sizeof(struct protocolSubMenu));
	
	if (subMenuArray == NULL){
		writeToLog("Error in menuManagementInitialization - No memory for subMenuArray");
		return 8;
	}
	
	/* Main POPUP Menu */
	mainMenuBarHandle       = NewMenuBar(0);
	mainMenuBarSettingsMenu = NewMenu(mainMenuBarHandle, "Main Menu", -1);
	if (physicsMode){
		settingsMenuCameraItem  = NewMenuItem(mainMenuBarHandle, mainMenuBarSettingsMenu, "__Camera Settings ...",   -1, VAL_MENUKEY_MODIFIER | 'C', camSettings_CB,     0);
		settingsMenuScriptItem  = NewMenuItem(mainMenuBarHandle, mainMenuBarSettingsMenu, "Run Script ...",          -1, 0,                          scriptExecution_CB, 0);
	}
	settingsMenuExitItem    = NewMenuItem(mainMenuBarHandle, mainMenuBarSettingsMenu, "E__xit",                  -1, VAL_MENUKEY_MODIFIER | 'X', quit_CB,            0);
	
	/* POPUP Menu for Reconnection with Server */
	otherMenuBarHandle         = NewMenuBar(0);
	reconnectMenu              = NewMenu(otherMenuBarHandle, "recon menu", -1);
	reconnectMenuReconnectItem = NewMenuItem(otherMenuBarHandle, reconnectMenu, "Reconnect", -1, 0, reconnect_CB, 0); 
	
	
	/* POPUP Menu for STUDY TAB */
	studyBarHandle      = NewMenuBar(0);	   // No associated panel, POPUP only
	studyTabPopupMenu   = NewMenu(studyBarHandle,"The Study TAB POPUP", -1);
	
	studyPopNew         = NewMenuItem(studyBarHandle, studyTabPopupMenu,"New ...",        -1, 0, studyTabPopupCallback, 0);
	studyPopRename      = NewMenuItem(studyBarHandle, studyTabPopupMenu,"Rename",         -1, 0, studyTabPopupCallback, 0);
	studyPopShow        = NewMenuItem(studyBarHandle, studyTabPopupMenu,"Show All",       -1, 0, studyTabPopupCallback, 0);
	InsertSeparator(studyBarHandle,studyTabPopupMenu,-1);
	
	studyPopRun         = NewMenuItem(studyBarHandle, studyTabPopupMenu,"Run",            -1, 0, studyTabPopupCallback, 0);
	studyPopEdit        = NewMenuItem(studyBarHandle, studyTabPopupMenu,"Edit ...",       -1, 0, studyTabPopupCallback, 0);
	studyPopArchive     = NewMenuItem(studyBarHandle, studyTabPopupMenu,"Export ...",     -1, 0, studyTabPopupCallback, 0);
	studyPopDelete      = NewMenuItem(studyBarHandle, studyTabPopupMenu,"Delete ...",     -1, 0, studyTabPopupCallback, 0);
	InsertSeparator(studyBarHandle,studyTabPopupMenu,-1);
	
	studyPopView        = NewMenuItem(studyBarHandle, studyTabPopupMenu,"View",       -1, 0, studyTabPopupCallback, 0);
	
	runProtocolMenuID   = NewSubMenu(studyBarHandle,studyPopRun);
	
	
	/* POPUP Menu for PROTOCOL TAB  */
	protocolBarHandle     = NewMenuBar(0);	   // No associated panel, POPUP only
	protocolTabPopupMenu  = NewMenu(protocolBarHandle,"The Protocol TAB POPUP", -1);
	
	protocolPopNewDir     = NewMenuItem(protocolBarHandle, protocolTabPopupMenu,"New Directory ...",     -1, 0, protocolTabPopupCallback, 0);
	protocolPopNewProt    = NewMenuItem(protocolBarHandle, protocolTabPopupMenu,"New Protocol ...",      -1, 0, protocolTabPopupCallback, 0);
	InsertSeparator(protocolBarHandle,protocolTabPopupMenu,-1);
	
	protocolPopEdit       = NewMenuItem(protocolBarHandle, protocolTabPopupMenu,"Edit ...",    -1, 0, protocolTabPopupCallback, 0);
	protocolPopRename     = NewMenuItem(protocolBarHandle, protocolTabPopupMenu,"Rename ...",  -1, 0, protocolTabPopupCallback, 0);
	protocolPopDelete     = NewMenuItem(protocolBarHandle, protocolTabPopupMenu,"Delete ...",  -1, 0, protocolTabPopupCallback, 0);

	/* populate sub menu with protocols */
	updateProtocolSubMenu();

	return 0;
						
}


void menuManagementTermination(void){
	saveRecentProtocols();
	deleteSubMenus();
	free(subMenuArray);
	DiscardMenuBar(mainMenuBarHandle);
	DiscardMenuBar(studyBarHandle);
	DiscardMenuBar(protocolBarHandle);
}


static void createToolTips(void){
	SetCtrlToolTipAttribute(studyTabHandle,    TAB_STUDY_TREE,            CTRL_TOOLTIP_ATTR_TEXT, "Right-click to create\nstudies or to act\nupon them.");	
	SetCtrlToolTipAttribute(idleTabHandle,     IDLE_PANEL_PETBox_LOGO,    CTRL_TOOLTIP_ATTR_TEXT, "Left-click for more\ninformation on Genisys");	
	SetCtrlToolTipAttribute(protocolEditPanel, PROT_EDIT_histFrameSeq,    CTRL_TOOLTIP_ATTR_TEXT, "number-of-frames x duration, ...\nExample: 10x30, 6x60");	
	SetCtrlToolTipAttribute(protocolEditPanel, PROT_EDIT_isotope_RING,    CTRL_TOOLTIP_ATTR_TEXT, "Right click to add an isotope");	
	
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_assayTimeYear,  CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");	
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_assayTimeMonth, CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_assayTimeDay,   CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_assayTimeHour,  CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_assayTimeMin,   CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_adminTimeYear,  CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");	
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_adminTimeMonth, CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_adminTimeDay,   CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_adminTimeHour,  CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	SetCtrlToolTipAttribute(studyEditPanel,    STUDY_EDIT_adminTimeMin,   CTRL_TOOLTIP_ATTR_TEXT, "Right-click to change date or time");		
	
}


static void deleteSubMenus(void){
	while (subMenuCount >=0 ) DiscardSubMenu(studyBarHandle, subMenuArray[subMenuCount--].subMenuID);
}


/* This function dynamically populates the "run" submenu from available protocols      */
/* It is invoked at initialization and whenever protocols are added, renamed or deleted */
/* BEWARE: this function gets its info from the current protocol tree not from disk */

void updateProtocolSubMenu(){
	int   i;
	int   index;
	int   protocolInserted=0;
	char  protocolName[MAX_PROTOCOL_NAME_LENGTH];
	
	deleteSubMenus();
	EmptyMenu(studyBarHandle,runProtocolMenuID);
	
	// Insert recent protocols first
	for(i=0; i< MAX_RECENT_PROTOCOLS; i++)
		if (0!=(*recentProtocols[i])) {
			
			// have to find index of this protocol
			GetTreeItemFromTag(protocolTabHandle, TAB_PROT_TREE, recentProtocols[i], &index); 
			if (index == -1) continue;  // protocol no longer there; idexgnore
			GetLabelFromIndex   ( protocolTabHandle, TAB_PROT_TREE, index, protocolName);
			NewMenuItem(studyBarHandle, runProtocolMenuID, protocolName, -1, 0, studyTabRunCallback, (void*) recentProtocols[i]); 
			protocolInserted = 1;
		}
	if (protocolInserted) InsertSeparator(studyBarHandle,runProtocolMenuID, -1);
	
	// Insert now regular protocol struture
	recursiveInsertMenu(0, studyBarHandle, runProtocolMenuID);
	
}

// Insert menu items - starting at index 'index' in tree -  in menuID. Calls itself when item is directory
static void recursiveInsertMenu(int index, int menuBarHandle, int menuID){
 	int  child;
	int  imageIndex;
	char protocolName[MAX_PROTOCOL_NAME_LENGTH];
	int  newItemID;
	int  subMenuID;
	int  numChildren;
	
	// Prime the loop: Get first child
	// (Get a child (VAL_CHILD) of 'index', starting at first child (VAL_FIRST),
	// searching in the forward direction (VAL_NEXT_PLUS_SELF), including the starting point)
	GetTreeItem(protocolTabHandle, TAB_PROT_TREE,  VAL_CHILD, index, VAL_FIRST, VAL_NEXT_PLUS_SELF, 0, &child);

	// Loop through all children
	while (child > -1){
		GetTreeItemAttribute( protocolTabHandle, TAB_PROT_TREE, child, ATTR_IMAGE_INDEX, &imageIndex);
		GetLabelFromIndex   ( protocolTabHandle, TAB_PROT_TREE, child, protocolName);
		
		// This entry is a directory: if not empty, will do a recursive call
		if (imageIndex == DIR_IMAGE_INDEX) {
			GetTreeItemNumChildren(protocolTabHandle, TAB_PROT_TREE, child, &numChildren);
			if (numChildren>0){
				newItemID = NewMenuItem(menuBarHandle, menuID, protocolName, -1, 0, studyTabRunCallback, 0);
				subMenuID = NewSubMenu (menuBarHandle, newItemID);
				recursiveInsertMenu(child, menuBarHandle, subMenuID);
			}
		} else
			
		// This entry is a protocol: insert in menu
		if (imageIndex == PROT_IMAGE_INDEX){
			char *protocolTag = malloc(MAX_TREE_ITEM_TAG_LEN+1);	// That's a small memory leak, yes I know
			GetTreeItemTag(protocolTabHandle, TAB_PROT_TREE, child, protocolTag);
			NewMenuItem(menuBarHandle, menuID, protocolName, -1, 0, studyTabRunCallback, (void*)protocolTag); // child is the index of the protocol to run
		}
		// Get next child
		// (Get a child (VAL_CHILD) of 'index', starting at 'child',
		// searching in the forward direction (VAL_NEXT), excluding the starting point)
		GetTreeItem(protocolTabHandle, TAB_PROT_TREE,  VAL_CHILD, index, child, VAL_NEXT, 0, &child);
	}
}

/* 
	Callback function invoked through a (protocol) menu item selection
	Callback data is the tag to the protocol in the tree
	
	The function creates the tasklist (combination of studyData and
	protocol.

*/

void CVICALLBACK studyTabRunCallback (int menuBarHandle, int menuItemID, void *callbackData, int panelHandle){
	
	char   studyPath         [MAX_FILE_NAME_LENGTH];
	char   studyDataFileName [MAX_FILE_NAME_LENGTH];
	char   taskListFileName  [MAX_FILE_NAME_LENGTH];
	struct taskList  *tl=NULL;
	
	char *protocolTag = (char*) callbackData;
	addToRecentProtocol(protocolTag);

	// Get study path
	if (0!=makeStudyPathName(lastTag, studyPath)){
		writeToLog("menuManagement::studyTabRunCallback - ERROR, internal error: invalid study index");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}
	
	// Try to read an existing task list
	if (0!=createTaskList(&tl)) return;  // Allocate memory for task list
	sprintfWithLock(MAX_FILE_NAME_LENGTH, taskListFileName,"%s\\taskList.bin",studyPath);
	int tlRetCode = readTaskList(taskListFileName, tl); 
	
	if ( tlRetCode == 0 ) {
		
		// Existing tasklist: merge new protocol with existing one
		int mergeRetCode= mergeProtocol(protocolTag, tl);
		if (mergeRetCode >4){
			writeToLog("menuManagement::studyTabRunCallback - I/O error reading protocol data");
			MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			return;
		} 
		else if (mergeRetCode != 0) return;
	} else {
		
		// No preexisting task list, combine study data and protocol
		
		// Read protocol
		if (0!=readProtocolFromTag(protocolTag, &tl->prot)){
			writeToLog("menuManagement::studyTabRunCallback - I/O error reading protocol data");
			MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			return;
		}
		
		// Read study Data
		sprintfWithLock(MAX_FILE_NAME_LENGTH, studyDataFileName,"%s\\studyData.bin",studyPath);
		if (0!=readStudyData(studyDataFileName, &tl->study)){
			writeToLog("menuManagement::studyTabRunCallback - I/O error reading study data");
			MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			return;
		}
		
		// start times and endtimes have been set to zero by memset earlier
		if (tl->prot.calibrationConstant == 0) tl->prot.calibrationConstant = defaultCalibrationConstant; 
		tl->acqInfo.completionCode  = -1;
		tl->histInfo.completionCode = -1;
		tl->reconInfo.completionCode= -1;
	}

	// Early check
	// If acquisition requested, make sure no acquisition is running and that connection has been established
	if (tl->prot.acq && tl->acqInfo.completionCode<0){
		int gotLock;
		CmtTryToGetLock(acquisitionLock, &gotLock);
		if ( gotLock ==1 ){
			CmtReleaseLock(acquisitionLock);
		} else{
			free(tl);
			MessagePopup("Alert", "Acquisition is already running!");
			return;
		}
		
		if (TCPServerHandle<0) {
			free(tl);
			MessagePopup("Alert", "Can't acquire data without a connection to Genisys.");
			return;
		}
	}
	
	// All clear, Schedule task handler
	if (0!=CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, protocolHandlerThread, tl, 0)){
		writeToLog("menuManagement::studyTabRunCallback - ERROR, can't schedule thread");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}
}


/* This function handles POPUP menu selection in the Study TAB */
void CVICALLBACK studyTabPopupCallback (int menuBarHandle, int menuItemID, void *callbackData, int panelHandle){
	
	char studyPath[MAX_FILE_NAME_LENGTH];
	char itemName [MAX_PROTOCOL_NAME_LENGTH];
	int  newIndex;
	char newTag[MAX_TREE_ITEM_TAG_LEN];
	char errorMessage[1024];
	
	GetTreeItemAttribute(studyTabHandle, TAB_STUDY_TREE, lastIndex, ATTR_LABEL_TEXT, itemName);

	if (menuItemID == studyPopNew){
		
		// New directory case
		if (lastImageIndex == 0) {
			
			newIndex   = InsertTreeItem(studyTabHandle, TAB_STUDY_TREE, VAL_CHILD, lastIndex, VAL_LAST, "",  "study dir", newTag, 0);
 			setItemAttributes(newIndex, theColor, theBGColor, DIR_IMAGE_INDEX); 
  			SetTreeItemAttribute(studyTabHandle, TAB_STUDY_TREE, newIndex, ATTR_LABEL_TEXT, newTag);
  			
			if (0!=makeStudyPathName(newTag, studyPath)){
				writeToLog("menuManagement::studyTabPopupCallback - ERROR, invalid index in creating new study directory");
				MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
				
			} else if ( 0!= MakeDir(studyPath))  {
				writeToLog("menuManagement::studyTabPopupCallback - ERROR, can't create directory");
				MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
				}
			
			// Enter "rename mode"
			SetActiveTreeItem(studyTabHandle, TAB_STUDY_TREE, newIndex, VAL_REPLACE_SELECTION_WITH_ITEM);
			FakeKeystroke(VAL_F2_VKEY);


		}
		
		// New study case
		else if (lastImageIndex == 1) {
			newStudyParentIndex  = lastIndex;
			editStudyPanelReason = NEW;
			
			time_t currentTS;
			currentTS = time(&currentTS);
			
			// Default values for assay time and admin time are now
			SetCtrlVal(studyEditPanel, STUDY_EDIT_assayTimeValue, currentTS);
			SetCtrlVal(studyEditPanel, STUDY_EDIT_adminTimeValue, currentTS);
			displayTime(&adminTimeControl);
	  		displayTime(&assayTimeControl);
			
			SetCtrlVal(studyEditPanel,       STUDY_EDIT_NAME,         "");
			SetCtrlVal(studyEditPanel,       STUDY_EDIT_SUBJECT_ID,   "");
			SetCtrlAttribute(studyEditPanel, STUDY_EDIT_NAME, ATTR_DIMMED,0);
			setSaveButtonStatus();
			SetActiveCtrl(studyEditPanel,    STUDY_EDIT_NAME);
			fadeInPanel(&studyEditPanelFading);
		} 
	}
	
	else if (menuItemID == studyPopEdit){
			editStudy();
	}
	
	else if (menuItemID == studyPopRename){
 		SetActiveTreeItem(studyTabHandle, TAB_STUDY_TREE, lastIndex, VAL_REPLACE_SELECTION_WITH_ITEM);
		FakeKeystroke(VAL_F2_VKEY);
	}

		
	else if (menuItemID == studyPopShow){
			showStudy();
	}

	else if (menuItemID == studyPopArchive){
		char defaultDir[MAX_PATHNAME_LEN]="";
		DWORD size    = MAX_PATHNAME_LEN-1;
		RegQueryValueEx(petBoxRegKey, "LastExport",   NULL, NULL, (LPBYTE)defaultDir, &size);	
		DisplayPanel(exportStudyPanel);
		SetCtrlVal(exportStudyPanel, EXPORT_PNL_DEST, defaultDir);
		SetCtrlAttribute(exportStudyPanel, EXPORT_PNL_DEST, ATTR_NO_EDIT_TEXT, 1);
		SetCtrlAttribute(exportStudyPanel, EXPORT_PNL_DEST, ATTR_TEXT_SELECTION_START, size);
		SetActiveCtrl(exportStudyPanel, EXPORT_PNL_BROWSE);
	}
	
	else if (menuItemID == studyPopDelete){
		
		// Delete directory case
		if (lastImageIndex==1){
			sprintfWithLock(2048, tempString,"Are you sure you want to delete directory %s?",itemName);
			if (1 == ConfirmPopup ("Confirm directory deletion", tempString) ){
					if (0!=makeStudyPathName(lastTag, studyPath)){
						writeToLog("menuManagement::studyTabPopupCallback - ERROR, invalid index when deleting study directory");
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
					} else
				 	if (0!=forceDeleteDir(studyPath)) {
						sprintfWithLock(1024, errorMessage,
							"menuManagement::studyTabPopupCallback - ERROR, forceDeleteDir(%s) failed.  Last tag: %s" ,studyPath, lastTag );
						writeToLog(errorMessage);
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
					}
					else {
						DeleteListItem(studyTabHandle, TAB_STUDY_TREE, lastIndex, 1);
					}
			}
		}
		
		// Delete study case
		else if (lastImageIndex==2){
			sprintfWithLock(2048,tempString,"Are you sure you want to delete study %s?",itemName);
			if (1 == ConfirmPopup ("Confirm study deletion", tempString) ){
					if (0!=makeStudyPathName(lastTag, studyPath)){
						writeToLog("menuManagement::studyTabPopupCallback - ERROR, invalid index when deleting study");
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
						return;
					}
					int delRetCode=0;
					delRetCode = forceDeleteDir(studyPath);
				 	if (0!=delRetCode) {
						char temp[1024];
						sprintfWithLock(1024, temp,"menuManagement::studyTabPopupCallback - ERROR,  %d - forceDeleteDir() failed for %s", delRetCode, studyPath);
						writeToLog(temp);
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
					}
					else {
						DeleteListItem(studyTabHandle, TAB_STUDY_TREE, lastIndex, 1);
					}
			}
		}
	}
	
	else if (menuItemID == studyPopView){
		
		char searchPath[MAX_FILE_NAME_LENGTH];
		char fileName[MAX_FILE_NAME_LENGTH];
		char command[MAX_FILE_NAME_LENGTH];
		
		makeStudyPathName(lastTag, studyPath);
		sprintfWithLock(MAX_FILE_NAME_LENGTH, searchPath, "%s\\dicom\\*", studyPath);
		if ( 0==  GetFirstFile(searchPath, 1, 0, 0, 0, 0, 0, fileName)){
			sprintfWithLock(MAX_FILE_NAME_LENGTH, searchPath, "%s\\dicom\\%s", studyPath, fileName);
			sprintfWithLock(MAX_FILE_NAME_LENGTH, command, "\"%s\" \"%s\"",DICOMViewer, searchPath);
			LaunchExecutable(command);
		} // end if GetFirstFile

	} // end if menuitem
} // end studyTabPopupCallback

/* 
	This function handles POPUP menu selection in the Protocol TAB 
	
*/
void CVICALLBACK protocolTabPopupCallback (int menuBarHandle, int menuItemID, void *callbackData, int panelHandle){
	char itemName[MAX_FILE_NAME_LENGTH];
	char fullPath[MAX_FILE_NAME_LENGTH];
	char newTag[MAX_FILE_NAME_LENGTH];
	int  newIndex;
	
	// New directory case -------------------------------------------------------------
	if  (menuItemID == protocolPopNewDir){
		
		newIndex = insertChildInProtocolTree("", "protocol dir", lastIndex, theColor, theBGColor, DIR_IMAGE_INDEX);
		GetTreeItemTag(protocolTabHandle, TAB_PROT_TREE, newIndex, newTag);
		SetTreeItemAttribute(protocolTabHandle, TAB_PROT_TREE, newIndex, ATTR_LABEL_TEXT, newTag);
		
		// will enter "rename mode"
		SetActiveTreeItem(protocolTabHandle, TAB_PROT_TREE, newIndex, VAL_REPLACE_SELECTION_WITH_ITEM);
		FakeKeystroke(VAL_F2_VKEY);   		
		
		if (0!=makeProtocolPathName(newTag,fullPath,"")){
				writeToLog("menuManagement::protocolTabPopupCallback - ERROR, invalid tag when creating new directory");
				MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			} else if (0!=MakeDir(fullPath)){
				writeToLog("menuManagement::protocolTabPopupCallback - MakeDir() failed creating new directory");
				MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			}		
		
	} else
	
	// New protocol case ---------------------------------------------------------------
	if  (menuItemID == protocolPopNewProt){
		editProtocolPanelReason = NEW;
		editProtocol();
	} else
	
	// Delete protocol or directory ----------------------------------------------------
	if  (menuItemID == protocolPopDelete){
		GetLabelFromIndex(protocolTabHandle, TAB_PROT_TREE, lastIndex, itemName);
		
		// DELETE DIRECTORY CASE  --------
		if (lastImageIndex==NO_IMAGE_INDEX || lastImageIndex==DIR_IMAGE_INDEX){
			
			if ( 0!=makeProtocolPathName(lastTag,fullPath,"") ){
				writeToLog("menuManagement::protocolTabPopupCallback - ERROR, invalid tag deleting directory");
				MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
				return;
			}
			
			if ( !isDirectoryEmpty(fullPath) ){
				MessagePopup("Invalid Request", "Can't delete non-empty directory.");
				return;
			}
			
			sprintfWithLock(2048, tempString,"Are you sure you want to delete directory %s?",itemName);
			
			if (1 == ConfirmPopup ("Confirm directory deletion", tempString) ){
				
				if (0!=DeleteDir(fullPath)){
					writeToLog("menuManagement::protocolTabPopupCallback - ERROR, DeleteDir() failed deleting directory");
					MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
				} else {
					DeleteListItem(protocolTabHandle, TAB_PROT_TREE, lastIndex, 1);
				}
			}
		}
		
		// DELETE PROTOCOL CASE  ---------
		else if (lastImageIndex==PROT_IMAGE_INDEX){ 
			sprintfWithLock(2048,tempString,"Are you sure you want to delete protocol %s?",itemName);
			if (1 == ConfirmPopup ("Confirm protocol deletion", tempString) ){
			 	deleteProtocol(lastTag);
				DeleteListItem(protocolTabHandle, TAB_PROT_TREE, lastIndex, 1);
			}
		}
		updateProtocolSubMenu(); 			

	} else
	
	// Edit protocol  -----------------------------------------------------------------
	if  (menuItemID == protocolPopEdit){
		editProtocolPanelReason = EDIT;
		myStrcpy(editPanelTag, lastTag);
		editProtocol();
	} else
									
	// Rename protocol or protocol directory   ------------------------------------------
	if  (menuItemID == protocolPopRename){
		SetActiveTreeItem(protocolTabHandle, TAB_PROT_TREE, lastIndex, VAL_REPLACE_SELECTION_WITH_ITEM);
		FakeKeystroke(VAL_F2_VKEY);
	}
	
	
}

// Keep quiet if something does not work, not a big deal
static void addToRecentProtocol(char *tag){
	int i;

	// Is protocol already there ? 
	for(i=0; i< numRecentProt; i++) 
		if ( 0 == strcmp(tag,recentProtocols[i])) return;
		
	// is there an empty slot 
	if (numRecentProt < MAX_RECENT_PROTOCOLS) myStrcpy(recentProtocols[numRecentProt++],tag);
	// If not, bump
	else{
		for(i=0; i<	MAX_RECENT_PROTOCOLS-1; i++)myStrcpy(recentProtocols[i],recentProtocols[i+1]);
		myStrcpy(recentProtocols[i],tag);
	}
	updateProtocolSubMenu();
}

// Converts a string of protocol names (separated by '!') to an array of strings (protocols)
void stringToProtocolArray(char *string){
	char *eos;
	
	for(numRecentProt=0; numRecentProt< MAX_RECENT_PROTOCOLS; numRecentProt++) (*recentProtocols[numRecentProt])=0;
	numRecentProt=0;
	
	eos = strtok(string,"!");
	while (0!= eos && numRecentProt<MAX_RECENT_PROTOCOLS){
		myStrcpy(recentProtocols[numRecentProt++], eos);
		eos = strtok(NULL,"!");
	}	
}

// Converts the protocol array to a single string of protocol names separated by '!'
// The string is saved to the registry by saveRecenetProtocols
void protocolArrayToString(char* string){
	int i;
	
	(*string)=0;
	for(i=0; i< MAX_RECENT_PROTOCOLS; i++) {
		strcat(tempString,recentProtocols[i]);
		strcat(tempString,"!");
	}
}

void readRecentProtocols(void){
	int retCode;
	int i;
	DWORD  size;
	
	// Allocate space 
	for (i=0; i < MAX_RECENT_PROTOCOLS; i++) {
		recentProtocols[i]=malloc(MAX_FILE_NAME_LENGTH); // protocol full path name
		if ( NULL == recentProtocols[i] ){
			writeToLog("Error in readRecentProtocols - No memory for recentProtocols[i]");
			return;
		}
		(*recentProtocols[i])=0;
	} 
	
	size    = (DWORD) sizeof(tempString);
	retCode = RegQueryValueEx(petBoxRegKey, "RecentProtocols",  NULL, NULL, (LPBYTE)tempString, &size);	
	if (retCode == 0) stringToProtocolArray(tempString);

} 
	
static void saveRecentProtocols(void){	
	int retCode;
	protocolArrayToString(tempString);
	retCode = RegSetValueEx(petBoxRegKey, "RecentProtocols", 0, REG_SZ, (BYTE*)tempString, (DWORD) sizeof(tempString) );
}

int CVICALLBACK sideTAB_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)																	 
	{
		case EVENT_RIGHT_CLICK:
			RunPopupMenu(mainMenuBarHandle, mainMenuBarSettingsMenu, panel, eventData1, eventData2, 0,0,0,0);
			break;
	}
	return 0;
}
/*
	Quit callback from file menu
*/
void CVICALLBACK quit_CB (int menuBar, int menuItem, void *callbackData, int panel){
	if ( 0<GetProtocolHandlerThreadCounter() ){
		if( !ConfirmPopup("Exit Confirm", "There are protocols running. Do you want\nto exit and kill those tasks?")) 
			return;
	}
	shutdown();
}
