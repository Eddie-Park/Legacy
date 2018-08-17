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
#include "typedefs.h"
#include "utilities.h"
#include "logManagement.h"
#include "protocolTreeManagement.h"
#include "protocolEdit.h"
#include "menuManagement.h"
#include "globalDecls.h"
#include "genisys4GUI.h"

static void recursiveInsert(char *path, int parentIndex);
static int  createProtocolTree(void);
static void setCtrlMenuDim(int newDir, int newProt, int edit, int rename, int delete);

int protocolTreeManagementInitialization(void){
 	int histIndex;

	SetTreeColumnAttribute(protocolTabHandle, TAB_PROT_TREE, 0, ATTR_COLUMN_WIDTH,160);
	SetTreeColumnAttribute(protocolTabHandle, TAB_PROT_TREE, 0, ATTR_VERTICAL_GRID_VISIBLE,0); 
	SetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE, ATTR_SCROLL_BAR_COLOR, VAL_LT_GRAY); 

	histIndex = createProtocolTree();
	SetActiveTreeItem(studyTabHandle,TAB_PROT_TREE,histIndex,VAL_REPLACE_SELECTION_WITH_ITEM);
	
	return 0;
}

void protocolTreeManagementTermination(void){
}

/* 
	Create protocol tree from disk
*/
static int createProtocolTree(void){
	int allIndex;

	/* Get topmost index and color attributes to use with other cells */
	GetTreeItemFromTag  (protocolTabHandle, TAB_PROT_TREE,"all",&allIndex);
	GetTreeItemAttribute(protocolTabHandle, TAB_PROT_TREE, allIndex,ATTR_LABEL_BGCOLOR,&theBGColor);
	GetTreeItemAttribute(protocolTabHandle, TAB_PROT_TREE, allIndex,ATTR_LABEL_COLOR,  &theColor);
	
	// setup search mask and populate tree
	recursiveInsert(protFolder, allIndex);
	
	return allIndex;
}

/*
	Insert protocols or directories (recursive)
*/
static void recursiveInsert(char *path, int parentIndex){
	register int i;
	int size, newIndex;
	char nextLevel   [MAX_PATHNAME_LEN];
	char protocolFullPath[MAX_PATHNAME_LEN];
	char searchPath[MAX_PATHNAME_LEN];
	char **nameArray;
	char *theDot;

	// Insert Directory names
	sprintfWithLock(MAX_PATHNAME_LEN, searchPath, "%s\\*", path); 
	nameArray = createDirList(searchPath,1,&size);
	if (nameArray==NULL){
		return;		
	}
	for(i=0; i<size; i++){
		theDot = strchr(nameArray[i],'.');
		
		// Directory case (no dot in name)
		if( theDot == 0 ) {
			newIndex = insertChildInProtocolTree(nameArray[i], "protocol dir", parentIndex, theColor, theBGColor, DIR_IMAGE_INDEX);
			if (newIndex>=0){
				sprintfWithLock(MAX_PATHNAME_LEN, nextLevel,"%s\\%s",path,nameArray[i]);
				recursiveInsert(nextLevel, newIndex);
			}
		} else
			
		// Protocol case
		{
			struct protocol prot;
			sprintfWithLock(MAX_PATHNAME_LEN, protocolFullPath,"%s\\%s",path,nameArray[i]);
			readProtocol(protocolFullPath, &prot);
			newIndex = insertChildInProtocolTree(removeExtension(nameArray[i]), prot.tag, parentIndex, theColor, theBGColor, PROT_IMAGE_INDEX);		
		}
	}
	freeDirList(nameArray,size);
}


/* Insert item in the protocol TAB tree */
int insertChildInProtocolTree(char *name, char* tag, int parent, int theColor, int theBGColor, int imageIndex){
	int newItemIndex, level;

	newItemIndex = InsertTreeItem(protocolTabHandle,TAB_PROT_TREE, VAL_CHILD, parent, VAL_LAST, name, tag,0,0);
	if (newItemIndex>=0){
		GetTreeItemLevel(protocolTabHandle,TAB_PROT_TREE,newItemIndex,&level);
		SetTreeItemAttribute(protocolTabHandle,TAB_PROT_TREE,newItemIndex,ATTR_LABEL_BGCOLOR,theBGColor);
		SetTreeItemAttribute(protocolTabHandle,TAB_PROT_TREE,newItemIndex,ATTR_LABEL_COLOR,theColor);
		SetTreeItemAttribute(protocolTabHandle,TAB_PROT_TREE,newItemIndex,ATTR_IMAGE_INDEX,level);
		SetTreeItemAttribute(protocolTabHandle,TAB_PROT_TREE,newItemIndex,ATTR_IMAGE_INDEX, imageIndex);
	}
	return newItemIndex;
}


/*
	Protocol TAB Callback function
		called on a right-click in the protocol TAB 
		or a double left-click for protocol edit
*/
int CVICALLBACK protTree_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		
		case EVENT_KEYPRESS:
			if (eventData1 == VAL_F2_VKEY){
				GetActiveTreeItem(protocolTabHandle, TAB_PROT_TREE, &lastIndex);
				GetLabelFromIndex(protocolTabHandle, TAB_PROT_TREE, lastIndex, oldName);       // save old name  for later
			}
			break;
			
			
		
		case EVENT_COMMIT:
			if (eventData1 == LABEL_CHANGE){
				
				// Make sure new name does not contain trailing blanks
				char newName[MAX_PATHNAME_LEN];
				GetLabelFromIndex(panel, control, eventData2, newName);
				removeTrailingBlanks(newName);					
				SetTreeItemAttribute(panel, control, eventData2, ATTR_LABEL_TEXT,  newName); 
				
				// Determine if this is a protocol or a directory
 				GetTreeItemAttribute(panel, control, eventData2, ATTR_IMAGE_INDEX, &lastImageIndex);

				// rename protocol
				if (lastImageIndex==PROT_IMAGE_INDEX){
					int retCode;
					retCode =  renameProtocol(lastTag, oldName);
					if (retCode !=0){
						writeToLog("menuManagement::protocolTabPopupCallback - ERROR, rename protocol failed.");
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
					}
				} else
				
				// rename protocol directory
				{
					char fullPath[MAX_PATHNAME_LEN];
					char oldPathName[MAX_PATHNAME_LEN];
					char newPathName[MAX_PATHNAME_LEN];
					
				 	myGetTreeItemPathFromIndex(panel, control, eventData2, fullPath, 0);
					sprintfWithLock(MAX_PATHNAME_LEN, oldPathName,"%s\\%s\\%s", protFolder, fullPath, oldName);
					sprintfWithLock(MAX_PATHNAME_LEN, newPathName,"%s\\%s\\%s", protFolder, fullPath, newName);
				
					if (0 != rename(oldPathName, newPathName) ){
						writeToLog("menuManagement::protocolTabPopupCallback - ERROR, rename protocol directory failed.");
						MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
					}
				}
				
			updateProtocolSubMenu(); 			
	
			}
			break;
		
		
		
		case EVENT_RIGHT_CLICK:
			lastRightClickPoint = MakePoint(eventData2, eventData1);
			GetIndexFromPoint(panel,control, lastRightClickPoint, &lastIndex, &lastArea, &lastColumn);
			if (lastIndex <0) return 1;		  
			GetTreeItemTag(panel, control, lastIndex, lastTag);

			GetTreeItemAttribute(panel, control, lastIndex, ATTR_IMAGE_INDEX, &lastImageIndex);
			switch (lastArea){
				/* Close enough to an item */
				case VAL_ITEM_PLUS_MINUS_AREA:
				case VAL_ITEM_MARK_AREA:
				case VAL_ITEM_IMAGE_AREA:
				case VAL_ITEM_LABEL_AREA:
					switch (lastImageIndex){
						
						// "ALL directory": new (directory)
						case NO_IMAGE_INDEX:
							//protocolCategoryIndex = 0;
							setCtrlMenuDim(0,1,1,1,1);  // New
							break;
							
						// Category item: new dir, new protocol, edit, rename(category), delete(category)
						case DIR_IMAGE_INDEX:
							//protocolCategoryIndex = lastIndex;
							setCtrlMenuDim(1,0,1,0,0);  // New
							break;
							
						// Protocol item: edit, rename, delete 	
						case PROT_IMAGE_INDEX:
							//GetTreeItemParent(panel, control, lastIndex, &protocolCategoryIndex);
							setCtrlMenuDim(1,1,0,0,0);   
							break;
					}
					break;
					
				default:
					return 1;
					//break;
					
			} // end switch lastArea

			RunPopupMenu(protocolBarHandle, protocolTabPopupMenu, protocolTabHandle, eventData1, eventData2, 0,0,0,0);
			return 1; // swallow
			//break;
			
			
		case EVENT_LEFT_DOUBLE_CLICK:
			GetIndexFromPoint(panel,control, MakePoint(eventData2, eventData1),  &lastIndex, &lastArea, &lastColumn);
			if (lastIndex <0) return 1;
			GetTreeItemTag(panel, control, lastIndex, lastTag);
	
			GetTreeItemAttribute(panel, control, lastIndex, ATTR_IMAGE_INDEX, &lastImageIndex);
			switch (lastArea){
				
				/* Close enough to an item */
				case VAL_ITEM_PLUS_MINUS_AREA:
				case VAL_ITEM_MARK_AREA:
				case VAL_ITEM_IMAGE_AREA:
				case VAL_ITEM_LABEL_AREA:
					if (lastImageIndex==PROT_IMAGE_INDEX){
							editProtocolPanelReason = EDIT;
							myStrcpy(editPanelTag, lastTag);
							editProtocol();
					}
					break;
				
				default:
					return 1;
					//break;
					
			} // end switch lastArea
			break;
	}// end switch event
	
	return 0;
}

/* Selectively dims items in the protocol POPUP menu */
static void setCtrlMenuDim(int newDir, int newProt, int edit, int rename, int delete){
	SetMenuBarAttribute(protocolBarHandle, protocolPopNewDir,  ATTR_DIMMED, newDir);
	SetMenuBarAttribute(protocolBarHandle, protocolPopNewProt, ATTR_DIMMED, newProt);
	SetMenuBarAttribute(protocolBarHandle, protocolPopEdit,    ATTR_DIMMED, edit);
	SetMenuBarAttribute(protocolBarHandle, protocolPopRename,  ATTR_DIMMED, rename);
	SetMenuBarAttribute(protocolBarHandle, protocolPopDelete,  ATTR_DIMMED, delete);
}
