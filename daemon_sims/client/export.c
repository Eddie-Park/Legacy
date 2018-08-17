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
#include "winbase.h"
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
#include "export.h"

static struct waitPanelControlPair wpcp;
static HANDLE exportMutex;


int exportInitialization(void){
	exportMutex = CreateMutex(NULL, FALSE, "mutex.export");
	if (exportMutex==NULL) return 8;
	return 0;
}

int exportTermination(void){
	CloseHandle(exportMutex);
	return 0;
}

// Copy a set of files (determined by mask) from inputDirectory to outputDirectory
static int copyFiles(char* inputDirectory, char* outputDirectory, char* mask){
	char  fileName   [MAX_FILE_NAME_LENGTH];
	char  inFilePath [MAX_FILE_NAME_LENGTH];
	char  outFilePath[MAX_FILE_NAME_LENGTH];
	char  searchPath [MAX_FILE_NAME_LENGTH];   
	char  temp       [MAX_FILE_NAME_LENGTH];   
												  
	sprintfWithLock(MAX_FILE_NAME_LENGTH,searchPath, "%s\\%s", inputDirectory, mask);
	
	// Initialize fields for copyFileWithProgress
	wpcp.panel   = exportStudyPanel;
	wpcp.control = EXPORT_PNL_WAIT_CTRL;
	GetNumListItems(exportStudyPanel,   EXPORT_PNL_WAIT_CTRL, &wpcp.imageCount);
	wpcp.imageIndex  = 0;
	wpcp.tickCounter = 0;
	
	// Copy loop
	int getFileRetCode = GetFirstFile(searchPath, 1, 0, 0, 0, 0, 0, fileName);
	while (getFileRetCode == 0){
		sprintfWithLock(MAX_FILE_NAME_LENGTH, inFilePath,  "%s\\%s", inputDirectory,  fileName);
		sprintfWithLock(MAX_FILE_NAME_LENGTH, outFilePath, "%s\\%s", outputDirectory, fileName);
		if ( 0 == copyFileWithProgress(inFilePath, outFilePath,  &wpcp ) ){
			sprintfWithLock(MAX_FILE_NAME_LENGTH,temp, "export::copyDirectory - Copy failed for file %s", inFilePath);
			writeToLog(temp);
			return 8;
		}
		getFileRetCode = GetNextFile(fileName);
	}
	
	return 0;
	
}

// Copy an entire directory (files and subdirectories - 1 level only )
// from inputDirectory to outputDirectory 
// Can not be called recursively because too many non-reentrant functions 

static int copyDirectory(char* inputDirectory, char* outputDirectory){
	char  inDirPath [MAX_FILE_NAME_LENGTH];
	char  inDirName [MAX_FILE_NAME_LENGTH];
	char  outDirPath[MAX_FILE_NAME_LENGTH];
	char  searchPath [MAX_FILE_NAME_LENGTH];   
	
	// Copy files first
	copyFiles(inputDirectory, outputDirectory, "*");
	
	// Copy sub-directories
	sprintfWithLock(MAX_FILE_NAME_LENGTH, searchPath, "%s\\%s", inputDirectory, "*");
	
	int getFileRetCode = GetFirstFile(searchPath, 0, 0, 0, 0, 0, 1, inDirPath);
	while (getFileRetCode == 0){
		SplitPath(inDirPath, NULL, NULL, inDirName);
		sprintfWithLock(MAX_FILE_NAME_LENGTH, outDirPath, "%s\\%s", outputDirectory, inDirName);
		int makeDirRetCode = MakeDir(outDirPath);
		if ( makeDirRetCode == 0 || makeDirRetCode == -9 ) copyFiles(inDirPath, outDirPath, "*");
		else MessagePopup("Error", "Can't create destination sub-directory");
		getFileRetCode = GetNextFile(inDirPath);
	}
	
	return 0;
}

static void doExport(struct exportData* xd){
	char studyPath[MAX_FILE_NAME_LENGTH];
	int dummy;
	int getFileRetCode;

	DWORD mutexCode = WaitForSingleObject(exportMutex, INFINITE);
	
	switch (mutexCode){
		
		case WAIT_OBJECT_0:
			
				getFileRetCode = GetFileAttrs(xd->exportDirectory, &dummy, &dummy, &dummy, &dummy);
	
				if ( getFileRetCode == 1 ){
					if (0!=makeStudyPathName(xd->studyTag, studyPath)){
						writeToLog("export::doExport - invalid index making study path name");
						MessagePopup("Error", "Error exporting study");
						return;
					}
					if (xd->checkLMF)     copyFiles(studyPath, xd->exportDirectory, "*.lmf" );
					if (xd->checkHIST)    copyFiles(studyPath, xd->exportDirectory, "*.scn*");
					if (xd->checkPHOTOS)  copyFiles(studyPath, xd->exportDirectory, "*.raw" );
					if (xd->checkRAWIMG)  copyFiles(studyPath, xd->exportDirectory, "*.im3" );
					
					if (xd->checkDICOM) {
						char inDir [MAX_FILE_NAME_LENGTH];
						char outDir[MAX_FILE_NAME_LENGTH];
						sprintfWithLock(MAX_FILE_NAME_LENGTH, inDir,  "%s\\dicom", studyPath);
						sprintfWithLock(MAX_FILE_NAME_LENGTH, outDir, "%s\\dicom", xd->exportDirectory);
						int makeDirRetCode =  MakeDir(outDir);
						if ( makeDirRetCode == 0 || makeDirRetCode == -9 ) copyDirectory(inDir, outDir);
						else MessagePopup("Error", "Can't create destination 'dicom' directory");
					}
				} else {
					MessagePopup("Error", "Export Directory not found");
				}
				ReleaseMutex(exportMutex);
			break;
			
		default:
			break;
	}
	

}


int CVICALLBACK exportBrowse_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char exportDirectory[MAX_PATHNAME_LEN];
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(exportStudyPanel, EXPORT_PNL_DEST,   exportDirectory);
			if (DirSelectPopup(exportDirectory, "Select Destination Directory for Export", 1, 1, exportDirectory) ){
				SetCtrlVal(exportStudyPanel, EXPORT_PNL_DEST, exportDirectory);
				RegSetValueEx(petBoxRegKey, "LastExport",      0, REG_SZ,      (BYTE*)exportDirectory, (DWORD) (1+strlen(exportDirectory)) );
			}
			break;
	}
	return 0;
}

int CVICALLBACK exportOK_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char exportContainingDirectory[MAX_FILE_NAME_LENGTH];
	char studyName[MAX_FILE_NAME_LENGTH]; 
	
	switch (event)
	{
		case EVENT_COMMIT:
			SetPanelAttribute(exportStudyPanel, ATTR_DIMMED, 1);
			SetCtrlAttribute(exportStudyPanel, EXPORT_PNL_WAIT_CTRL, ATTR_VISIBLE, 1); 
			SetWaitCursor(1);
			
			struct exportData* xd = malloc(sizeof(*xd));
			
			if (xd == NULL){
				writeToLog("export::exportOK_CB - ERROR, can't allocate memory for export data structure");
				return 8;
			}

			// Make the export directory name and create it
			GetCtrlVal(exportStudyPanel, EXPORT_PNL_DEST,   exportContainingDirectory);
			GetTreeCellAttribute(studyTabHandle,TAB_STUDY_TREE, lastIndex, COLUMN_STUDYNAME, ATTR_LABEL_TEXT, studyName);
			sprintfWithLock(MAX_FILE_NAME_LENGTH, xd->exportDirectory, "%s\\%s",exportContainingDirectory, studyName); 
			if (0!=MakeDir(xd->exportDirectory) ){
				MessagePopup("Error", "Cannot create export directory");
				return 0;
			}
			GetCtrlVal(exportStudyPanel, EXPORT_PNL_LMF,    &xd->checkLMF);
			GetCtrlVal(exportStudyPanel, EXPORT_PNL_HIST,   &xd->checkHIST);
			GetCtrlVal(exportStudyPanel, EXPORT_PNL_PHOTOS, &xd->checkPHOTOS);
			GetCtrlVal(exportStudyPanel, EXPORT_PNL_DICOM,  &xd->checkDICOM);
			xd->batchMode  = 0;
			myStrcpy( xd->studyTag, lastTag);
			
			doExport(xd);
			
			SetCtrlAttribute(exportStudyPanel, EXPORT_PNL_WAIT_CTRL, ATTR_VISIBLE, 0); 
			HidePanel(panel);
			SetPanelAttribute(exportStudyPanel, ATTR_DIMMED, 0);
			SetWaitCursor(0);
			free(xd);
			break;
	}
	return 0;
}

int CVICALLBACK exportCancel_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(panel);
			break;
	}
	return 0;
}

