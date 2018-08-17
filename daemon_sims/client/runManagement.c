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


//                                     I M P O R T A N T 

// THE ACQUISITION PANEL IN BOTH UI FILES (physics mode and user mode) MUST HAVE AT LEAST
// THESE ELEMENTS IN THE SAME TAB ORDER: hour, min, sec and LED bars


#include "windows.h"

#include <formatio.h>
#include <tcpsupp.h>
#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include "typedefs.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "utilities.h"
#include "logManagement.h"
#include "monitorManagement.h"
#include "commonHeader.h"
#include "runManagement.h"
#include "xrayDaemonManagement.h"

#include "protocolEdit.h"
#include "studyEdit.h"

#define MAX_ACK_RETRY 100

extern void putToHex(char* out,void* in);

DefineThreadSafeScalarVar (int, ProtocolHandlerThreadCounter, 0);

// Local functions
static int  acquisition         (struct taskList* tl);
static int  acquisitionRecovery (struct taskList* tl);
static int  histogramming       (struct taskList* tl); 
static int  reconstruction      (struct taskList* tl); 
static int  askConfirmation     (struct taskList* tl); 
static void setHistAndReconStatusToFailed(void);
static int  dummyHisto          (struct taskList* tl);
static void  recoverAcquisitionIfNecessary(void);

int CVICALLBACK PETreconstruction(void *functionData);
int CVICALLBACK atlasRegistration(void *functionData);

/*
                                _______________________________________________

*/
int runManagementInitialization(void){
	
	if ( 0!= CmtNewLock(NULL, 0, &acquisitionLock)){
		writeToLog("runManagement::runManagementInitialization - Error, can't create acquisition lock");
	}

	InitializeProtocolHandlerThreadCounter();
	SetProtocolHandlerThreadCounter(0);

	// If server is connected, check for an acquisition already running 
	//  Disable temporarily
	if (TCPServerHandle >= 0) 
		recoverAcquisitionIfNecessary(); 

	// Change any "hist" or "recon" running status(green) to failed status(red)
	setHistAndReconStatusToFailed();
	return 0;
}
/*
                                _______________________________________________

*/
static void recoverAcquisitionIfNecessary(void){
	int timeOut;
	struct PETBoxMessage message;
	char temp[1024];
	char tmp[40];
	
	/* 
		Verify the status of the server and if it is running anything 
		(would happen after a crash and restart of the GUI)
	*/
	message.messageType = TYPE_COMMAND;
	message.commandCode = COMMAND_INQUIRE_STATUS;
	message.length      = (unsigned short) (  (char*)&message.inqCmd - (char*)&message );
	int bytesSent = ClientTCPWrite(TCPServerHandle, &message, message.length, timeOut=5000);
	
	putToHex(tmp, &message);
	sprintfWithLock(1024, temp, "runManagement::recoverAcquisitionIfNecessary - Sending inquire (%d bytes) - %s", message.length, tmp);
	writeToLog(temp);
	
	if (bytesSent<=0) {
		sprintfWithLock(1024, temp, "runManagement::recoverAcquisitionIfNecessary - Error %d in TCPWrite - %s", bytesSent,GetTCPErrorString(bytesSent) ); 
		writeToLog(temp);
		return;
	}
	
	
	// Wait for response
	int retryCount=MAX_ACK_RETRY;
	inquireAckReceived = 0;
	while (retryCount-->0){
		if (inquireAckReceived) break;
		else {
			Delay (0.1);
			// ProcessSystemEvents is required to process TCP callback (in this thread) because RunUserInterface has not been issued yet
			ProcessSystemEvents();  
		}
	}
	
	// No ACK received
	if (!inquireAckReceived) {
		writeToLog("runManagement::recoverAcquisitionIfNecessary - Time out, NO INQUIRE STATUS ACK Received\n");
		return;
	}
	
	// Verify Status for possible acquisition recovery (after crash)
	struct taskList *tl=NULL;
	char   taskListFileName [MAX_FILE_NAME_LENGTH];
	if (serverStatus.status & STATUS_ACQ_RUNNING){ 
		writeToLog("runManagement::runManagementInitialization - Acquisition recovery in progress.");
		if (0!=createTaskList(&tl)) return;
		// Get a copy of the task list on disk
		sprintfWithLock(MAX_FILE_NAME_LENGTH, taskListFileName,"%s\\taskList.bin",serverStatus.studyPathName);
		int readRetCode = readTaskList(taskListFileName, tl);
		if (readRetCode){
			writeToLog("runManagement::recoverAcquisitionIfNecessary - Error, reading task list.");
			return;
		}
		
		// Indicate recovery and reschedule
		sprintfWithLock(1024, temp, "runManagement::recoverAcquisitionIfNecessary - Recovering acquisition in %s [%s]", serverStatus.studyPathName, tl->study.tag ); 
		writeToLog(temp);
		CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, protocolHandlerThreadNoConfirm, tl, 0);
	}
	
}
/*
                                _______________________________________________
								
		Check each study

*/
static void setHistAndReconStatusToFailed(void){
	int  count;
	int  i;
	char studyPathName    [MAX_FILENAME_LEN];
	char taskListName     [MAX_FILENAME_LEN];
	struct taskList* tl = NULL;
	char tag[MAX_TREE_ITEM_TAG_LEN];
	
	if (0!=createTaskList(&tl)) return;
	 
	if (0<=GetNumTreeItems(studyTabHandle,TAB_STUDY_TREE, VAL_ALL, 0, 0, VAL_NEXT_PLUS_SELF, 0, &count)){
		for (i=1; i<count; i++){
			GetTreeItemTag(studyTabHandle,TAB_STUDY_TREE, i, tag);
			if(0==makeStudyPathName(tag, studyPathName)){
				sprintfWithLock(MAX_FILENAME_LEN, taskListName, "%s\\taskList.bin", studyPathName);
				if (fileExists(taskListName)){
					if (0==readTaskList(taskListName, tl)){
						
						// If there's an acquisition running, then do nothing to the status
						if (tl->prot.acq){	
							enum taskStatus statusAcq  = determineTaskStatus(&tl->acqInfo);
							if (statusAcq == TASK_STATUS_RUNNING) continue;
						}
						
						// Otherwise, set to failed if running or scheduled
							
						// If HIST was requested in protocol, and is currently scheduled or running
						// mark as failed
						if (tl->prot.hist){	
							enum taskStatus statusHist  = determineTaskStatus(&tl->histInfo);
							if ( statusHist == TASK_STATUS_SCHEDULED || statusHist  == TASK_STATUS_RUNNING  ){
								tl->histInfo.completionCode=8;	
								updateTaskStatus(&tl->histInfo, i, COLUMN_HIST);
								writeTaskList(tl);
							}
						}
							
						// If RECON was requested in protocol, and is currently scheduled or running
						// mark as failed						
						if (tl->prot.recon){	
							enum taskStatus statusRecon = determineTaskStatus(&tl->reconInfo);
							if ( statusRecon == TASK_STATUS_SCHEDULED || statusRecon  == TASK_STATUS_RUNNING  ){
								tl->reconInfo.completionCode=8;	
								updateTaskStatus(&tl->reconInfo, i, COLUMN_RECON);
								writeTaskList(tl);
							}
						}
					
					}//end if readtaskList
				}//end if file exists
			}// end if studypathName
		} //end for i
	}//end if GetNumTreeItems
	
	free(tl);
}
/*
                                _______________________________________________

*/
void runManagementTermination(void){
	CmtDiscardLock(acquisitionLock);
}

/*
                                _______________________________________________

	Protocol Handler(s) 
	Runs in its own thread; sequentially performing tasks in the tasklist
	(invoked by studyTabRunCallback, when the run menu item is selected )
	
	Also invoked at startup (recoverAcquisitionIfNecessary), if there are unfinished protocols
*/

int CVICALLBACK protocolHandlerThreadNoConfirm(void *functionData){
	struct taskList* tl = (struct taskList*) functionData;
	int* counterPtr;
	enum taskStatus statusAcq, statusHist, statusRecon;

	// Increment the count of running protocols
	counterPtr = GetPointerToProtocolHandlerThreadCounter();
	(*counterPtr)++;
	ReleasePointerToProtocolHandlerThreadCounter();
	
	// Update overall study status in panel
	int index;
	
	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
	if (tl->prot.acq)   updateTaskStatus(&tl->acqInfo,   index, COLUMN_ACQ);
	if (tl->prot.hist)  updateTaskStatus(&tl->histInfo,  index, COLUMN_HIST);
	if (tl->prot.recon) updateTaskStatus(&tl->reconInfo, index, COLUMN_RECON);
	
	/* Conditionally run acquisition*/
	int newAcquisition=0;
	
	statusAcq = determineTaskStatus(&tl->acqInfo); 
	if (tl->prot.acq && (statusAcq==TASK_STATUS_RUNNING || statusAcq==TASK_STATUS_SCHEDULED) )  {
		acquisitionRunning   = 0;
		acquisitionRequested = 1;
		newAcquisition       = 1;
		if (statusAcq==TASK_STATUS_RUNNING) acquisitionRecovery(tl);
		else                                acquisition(tl); 
	}
	
	// Conditionnally do the xray thingy
	ProcessSystemEvents();
	if (tl->prot.xray && newAcquisition && 0==tl->acqInfo.completionCode){
		xrayAcquisition(tl);  
	}

	/* Conditionally run histogramming*/
	statusAcq  = determineTaskStatus(&tl->acqInfo); 
	statusHist = determineTaskStatus(&tl->histInfo); 
	if (	tl->prot.hist                     &&
			( statusHist ==  TASK_STATUS_SCHEDULED ||
			  statusHist ==  TASK_STATUS_RUNNING ) &&
			( !tl->prot.acq || (tl->prot.acq && statusAcq == TASK_STATUS_COMPLETED) )
		)
		histogramming (tl);
	
	/* Conditionally run reconstruction*/
	statusRecon = determineTaskStatus(&tl->reconInfo); 
	statusHist  = determineTaskStatus(&tl->histInfo);
	if	(	tl->prot.recon  &&
			( statusRecon == TASK_STATUS_SCHEDULED ||
			  statusRecon == TASK_STATUS_RUNNING ) &&
			( (!tl->prot.hist && !tl->prot.acq) || (tl->prot.hist && statusHist == TASK_STATUS_COMPLETED) )
		)
		reconstruction(tl);
	
  	free(tl);

	// Decrement the count of running protocols
	counterPtr = GetPointerToProtocolHandlerThreadCounter();
	(*counterPtr)--;
	ReleasePointerToProtocolHandlerThreadCounter();
	
	return 0;
}
/*
                                _______________________________________________

*/
int CVICALLBACK protocolHandlerThread(void *functionData){
	
	struct taskList* tl =  (struct taskList*) functionData;

	// Ask confirmation only for PET acquisitions
	if (tl->prot.acq && tl->acqInfo.completionCode==-1) { // acq requested and never ran before?
		if (askConfirmation(tl)){
			protocolHandlerThreadNoConfirm(functionData);
			return 0;
		}
		else return 4;
	} 
	else protocolHandlerThreadNoConfirm(functionData);
	
	return 0;

}
/*
                                _______________________________________________

 	Ask a confirmation to run a protocol
 	returns: 0: CANCEL, do not proceed
    	     1: OK
*/
static int askConfirmation(struct taskList* tl){

	SetCtrlVal(confirmationPanel, TAB_PET_protocolName, tl->prot.name);
	SetCtrlVal(confirmationPanel, TAB_PET_studyName,    tl->study.name);
		
	acquisitionConfirmation = 0;
	answerGiven=0;
	fadeInPanel(&confirmPanelFading);
	while (!answerGiven){
		Delay(0.100);
		ProcessSystemEvents();
	}
	fadeOutPanel(&confirmPanelFading); 
	return acquisitionConfirmation;
}
/*
                                _______________________________________________

*/
static void copyLMFFile(struct taskList *tl){
	char temp[1024];
	char fullPathToStudy[1024];
	char lmfFileName[MAX_FILE_NAME_LENGTH];
	char fullPathToLMFFileName[1024];
	int retCode;
	ssize_t size;

	
	makeStudyPathName(tl->study.tag, fullPathToStudy);

	SplitPath(serverStatus.lmfFileName, NULL, NULL, lmfFileName);
	sprintfWithLock(1024, fullPathToLMFFileName, "%s\\%s", fullPathToStudy, lmfFileName );
	
	retCode = GetFileInfo(serverStatus.lmfFileName, &size);	   // 1: file exists
	int retryCount = 10;
	while( retCode==0 && retryCount>0) {
		Delay(1.0); 
		retCode = GetFileInfo(serverStatus.lmfFileName, &size);
		retryCount--;}
	if (retCode == 0){
		sprintfWithLock(1024, temp, "Timeout (10 secs). Copy of %s aborted.", serverStatus.lmfFileName);
		writeToLog(temp);
		return;
	}
	
	sprintfWithLock(1024, temp, "Copying %s to %s", serverStatus.lmfFileName, fullPathToLMFFileName);
	writeToLog(temp);

	// --------------- Prepare to copy with copyFileWithProgress ----------------|
	SetPanelAttribute(acquisitionPanel, ATTR_DIMMED, 1);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_WAIT_CTRL, ATTR_VISIBLE, 1); 
	SetWaitCursor(1);
	struct waitPanelControlPair wpcp={acquisitionPanel, ACQ_PANEL_WAIT_CTRL, 0, 0, 0};
	GetNumListItems(acquisitionPanel,   ACQ_PANEL_WAIT_CTRL, &wpcp.imageCount);

	retCode = copyFileWithProgress(serverStatus.lmfFileName, fullPathToLMFFileName,  &wpcp ); 
//	retCode = CopyFileA(serverStatus.lmfFileName, fullPathToLMFFileName, TRUE);  // OLD copy

	SetWaitCursor(0);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_WAIT_CTRL, ATTR_VISIBLE, 0); 
	// SetPanelAttribute(acquisitionPanel, ATTR_DIMMED, 0);
	// ---------------------------------------------------------------------------|	
		
	if ( 0 != retCode ) {
		writeToLog("Copy successful.");
		retCode = DeleteFile(serverStatus.lmfFileName);
		if ( 0 != retCode ){
			sprintfWithLock(1024, temp, "Delete of %s failed, code %d", serverStatus.lmfFileName , retCode);
			writeToLog(temp);
		}
	}
	else{
		sprintfWithLock(1024, temp, "Copying %s failed, code %d", serverStatus.lmfFileName , retCode);
		writeToLog(temp);
		FILE* fin = fopen(serverStatus.lmfFileName,"rb");
		if (fin == NULL) writeToLog("      input file can't be opened");
		else fclose(fin);
	}
}
/*
                                _______________________________________________

*/
static int sendStartToServer(struct taskList* tl){
 	struct PETBoxMessage message;
 	int bytesSent;
	int timeOut;
	char temp[1024];
	char tmp[40];

	// Send START to server
	startAckReceived=0;
	message.messageType = TYPE_COMMAND;
	message.commandCode = COMMAND_START;
	message.startCmd.duration       = tl->prot.duration;
	myStrcpy(message.startCmd.studyID, tl->study.name);
	makeStudyPathName(tl->study.tag, message.startCmd.studyPathName);
	message.length = (unsigned short) (    ((char*)&message.startCmd.studyPathName - (char*)&message) + strlen(message.startCmd.studyPathName) + 1   );

	bytesSent = ClientTCPWrite(TCPServerHandle, &message, message.length, timeOut=5000);
	putToHex(tmp, &message);
	
	sprintfWithLock(1024, temp, "Sending start in study %s, tag %s, path %s - %s",tl->study.name, tl->study.tag, message.startCmd.studyPathName, tmp);
	
	writeToLog(temp);

	if (bytesSent<=0) {
		sprintfWithLock(1024, temp, "runManagement::acquisition - Error %d in TCPWrite - %s", bytesSent,GetTCPErrorString(bytesSent) ); 
		writeToLog(temp);
		tl->acqInfo.completionCode = 8;
		int index;
		GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
		updateTaskStatus(&tl->acqInfo, index, COLUMN_ACQ);
		CmtReleaseLock(acquisitionLock);
		return 8;
	}
	
    // Wait for START acknowledge
	int retryCount=MAX_ACK_RETRY;
	while (retryCount-->0){
		if (startAckReceived) break;
		else Delay (0.1);
		ProcessTCPEvents();
	}
	
	// No ACK received?
	int retCode =0;
	if (!startAckReceived) {
		writeToLog("runManagement::acquisition - Time out, NO START ACK Received\n");
		retCode = 8;
	}
	
	return retCode;
}
/*
                                _______________________________________________

*/
static void setProgressBar(double fComplete){
	const int initialLength = 542;
	int length = (int) (fComplete * initialLength);
	SetCtrlAttribute(acquisitionPanel, ACQ_PANEL_PROGRESS_BAR, ATTR_WIDTH, length);
}


static void performAcquisitionLoop(struct taskList *tl){
	unsigned short int hour, min, sec;
	struct PETBoxMessage message;
	char temp[1024];
	int bytesSent;
	int timeOut;
	
	while(unsocStats.timeLeft > 0 && acquisitionRequested){
		durationToHms( &hour, &min, &sec, unsocStats.timeLeft);
		sprintfWithLock(sizeof(temp), temp, "%t",hour);
		SetCtrlVal(acquisitionPanel,ACQ_PANEL_hour, temp);
		sprintfWithLock(sizeof(temp), temp, "%t",min);
		SetCtrlVal(acquisitionPanel,ACQ_PANEL_min, temp);
		sprintfWithLock(sizeof(temp), temp, "%t",sec);
		SetCtrlVal(acquisitionPanel,ACQ_PANEL_sec, temp);
		setProgressBar(1.0 -   (double) unsocStats.timeLeft / (double) tl->prot.duration);
		setLEDControlValue(&promptsRateMeter, (1+unsocStats.promptsRate)/1000. );
		Delay(0.500);   // was 0.020 then 0.3	Apparently, there is an implicit ProcessSystemEvents() in Delay()
	}
	
	time(&tl->acqInfo.endTime);
	
	// Tell monitoring to stop
	breathingMonitoringRequested = 0;  
	
	// Normal completion (if acquisitionRequested is still true)
	if (acquisitionRequested){
		sprintfWithLock(1024,temp, "Acquisition ended in study %s", tl->study.name);
		writeToLog(temp);
		tl->acqInfo.completionCode = 0;
		copyLMFFile(tl);
	} else
		// Cancel request
	{
		message.messageType = TYPE_COMMAND;
		message.commandCode = COMMAND_STOP;
		message.length = (unsigned short)  (   (char*)&message.stopCmd - (char*)&message  );
		bytesSent = ClientTCPWrite(TCPServerHandle, &message, message.length, timeOut=5000);
		if (bytesSent<=0) {
			sprintfWithLock(1024,temp, "Error %d in TCPWrite - %s", bytesSent,GetTCPErrorString(bytesSent) ); 
			writeToLog(temp);
		}
		sprintfWithLock(1024,temp, "Acquisition cancelled in study %s", tl->study.name);
		writeToLog(temp);
		tl->acqInfo.completionCode = 8;
	}

	setProgressBar(0.0);

}
/*
                                _______________________________________________

*/
static void prepareForAcquisition(struct taskList *tl){
	
	// Prepare Panel Fields
	SetCtrlVal(acquisitionPanel,ACQ_PANEL_studyName,    tl->study.name);
	SetCtrlVal(acquisitionPanel,ACQ_PANEL_protocolName, tl->prot.name);
	SetCtrlVal(acquisitionPanel,ACQ_PANEL_breathFreq, 0);
	SetCtrlVal(acquisitionPanel,ACQ_PANEL_hour, 0);
	SetCtrlVal(acquisitionPanel,ACQ_PANEL_min, 0);
	SetCtrlVal(acquisitionPanel,ACQ_PANEL_sec, 0);
	DeleteGraphPlot(acquisitionPanel, ACQ_PANEL_breathingHistory, -1, VAL_IMMEDIATE_DRAW);
	SetPanelAttribute(acquisitionPanel, ATTR_DIMMED, 0);
	fadeInPanel(&acqPanelFading);
	
	// Prepare breathing monitoring
	breathingMonitoringThreadRunning = 0;
	if (tl->prot.breathingMon) {
		breathingMonitoringRequested = 1;
		CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, monitoringThread, tl, 0);
	} else{
		breathingMonitoringRequested = 0;
	}
}
/*
                                _______________________________________________

*/
static int acquisition(struct taskList* tl){
	char temp[1024];
	int gotLock;
	int index;
	
	// The acquisition lock is to prevent multiple acquisition
	CmtTryToGetLock(acquisitionLock, &gotLock);
	if ( 1!= gotLock) {
		writeToLog("runManagement::acquitision - Fatal error, can't obtain lock");
		return 8;
	}
	acquisitionRunning    = 1;
	
	sprintfWithLock(1024, temp, "Acquisition started in study %s, protocol %s", tl->study.name, tl->prot.name);
	writeToLog(temp);
	
	prepareForAcquisition(tl);
	
	time(&tl->acqInfo.startTime);
	writeTaskList(tl);
	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);

	updateTaskStatus(&tl->acqInfo, index, COLUMN_ACQ);
	
	// If start is successful ...
	if ( 0 ==  sendStartToServer(tl) ){
	
		// Set initial time
		unsocStats.timeLeft = tl->prot.duration;
		performAcquisitionLoop(tl);
	    fadeOutPanel(&acqPanelFading);
	    SetPanelAttribute(acquisitionPanel, ATTR_DIMMED, 0);
	
		// Recover fresh index (may have changed if studies were added/deleted)
		GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
		updateTaskStatus(&tl->acqInfo, index, COLUMN_ACQ);
		writeTaskList(tl);
		while(breathingMonitoringThreadRunning) Delay(0.020); // wait until monitoring is done
		acquisitionRunning =0;
	} 
	
	// Cleanup
	CmtReleaseLock(acquisitionLock);
	return tl->acqInfo.completionCode ;
}
/*
                                _______________________________________________

*/
static int acquisitionRecovery(struct taskList* tl){
	char temp[1024];
	int gotLock;
	int index;
	
	// Have to put a delay here (!) to be sure the UserInterface is running
	// otherwise the panel never gets updated.  Perhaps there is a way to 
	// know if/when the user interface is running ... Not a big deal.
	Delay(2.0);
	
	CmtTryToGetLock(acquisitionLock, &gotLock);
	if ( 1!= gotLock) {
		writeToLog("runManagement::acquisitionRecovery - Fatal error, can't obtain lock");
		return 8;
	}
 	acquisitionRunning    = 1;

	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
	updateTaskStatus(&tl->acqInfo, index, COLUMN_ACQ);
	
	sprintfWithLock(1024,temp, "Acquisition recovered in study %s, protocol %s, tag %s", tl->study.name, tl->prot.name, tl->study.tag);
	writeToLog(temp);
	
	prepareForAcquisition(tl);
	performAcquisitionLoop(tl);
	
	// Just in case index may have changed during acquisition
  	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
	updateTaskStatus(&tl->acqInfo, index, COLUMN_ACQ);
	writeTaskList(tl);
	
	while(breathingMonitoringThreadRunning) Delay(0.020); // wait until monitoring is done
	acquisitionRunning =0;
	
	// Cleanup
	CmtReleaseLock(acquisitionLock);
	fadeOutPanel(&acqPanelFading);
	return tl->acqInfo.completionCode;
}
/*
                                _______________________________________________

*/
int CVICALLBACK stopAcqNow_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			acquisitionRequested         = 0;
			breathingMonitoringRequested = 0;    // Early notification
			break;
	}
	return 0;
}
/*
                                _______________________________________________

*/

// Make a unique file ID, DICOM style
//  input: studyID(name)
// output: unique file name
void makeUniqueFileID(char* outFileName, char *studyID){
	union{
		char timeString[9];
		struct{
			char hh[2];
			char filler1;
			char mm[2];
			char filler2;
			char ss[2];
			char filler3;
		};
	} theTime;
	
	union{
		char dateString[11];
		struct{
			char mm[2];
			char filler1;
			char dd[2];
			char filler2;
			char yyyy[4];
			char filler3;
		};
	} theDate;
	
	myMemcpy(theTime.timeString, TimeStr(), 8);
	theTime.filler1=0;
	theTime.filler2=0;
	theTime.filler3=0;
	
	myMemcpy(theDate.dateString, DateStr(), 10);
	theDate.filler1=0;
	theDate.filler2=0;
	theDate.filler3=0;

	sprintfWithLock(MAX_FILE_NAME_LENGTH, outFileName,"1.2.840.%d.%d.%d.%d.%s.%s%s%s.%s%s%s",
		dicomRegID, 
		productID,
		serverStatus.serialNumber,
		++globalSequenceNumber,
		studyID,
		theDate.mm, theDate.dd, theDate.yyyy,
		theTime.hh, theTime.mm, theTime.ss
		 );
	RegSetValueEx(petBoxRegKey, "GlobalSequenceNumber",  0, REG_DWORD,  (LPBYTE)&globalSequenceNumber,   4);
	
}



/*
                                _______________________________________________

*/
static int histogramming(struct taskList* tl){
	char studyPathName[MAX_FILE_NAME_LENGTH];
	char searchName   [MAX_FILE_NAME_LENGTH];
	int index;
	int histHandle;
	int retCode;

	// Set the search path for .lmf files
	makeStudyPathName(tl->study.tag, studyPathName);
	sprintfWithLock(MAX_FILE_NAME_LENGTH, searchName, "%s\\*.lmf", studyPathName);
	retCode = GetFirstFile(searchName, 1,0, 0, 0, 0, 0, tl->prot.lmfFile);
	
	if (retCode !=0) {
		MessagePopup("Histogramming Error", "No list-mode file found.");
		tl->histInfo.completionCode = FAILURE;
	}  else {
	
		// make the hist root name based on the lmf file name
		myStrcpy(tl->prot.histFileRootName, tl->prot.lmfFile);
		char * theDot = strrchr(tl->prot.histFileRootName, '.');
		if (theDot != NULL){
			(*theDot)=0;
		} else {
			makeUniqueFileID(tl->prot.histFileRootName, tl->study.name); // Generate the histogram "root" name from scratch
		}
	
		// Delete existing histograms
		sprintfWithLock(MAX_FILE_NAME_LENGTH, searchName, "%s\\*.scn*", studyPathName);
		DeleteFile(searchName);

		// Initial update to task list
	  	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
		time(&tl->histInfo.startTime);
		updateTaskStatus(&tl->histInfo, index, COLUMN_HIST);
		writeTaskList(tl);
	
		// Invoke external histogram program
		makeStudyPathName(tl->study.tag, studyPathName);
		char command[1024];
		sprintfWithLock(1024, command,"histogram.exe \"%s\"", studyPathName);
		retCode = LaunchExecutableEx(command, LE_HIDE, &histHandle);
		if (retCode == 0){
			while ( 0 == ExecutableHasTerminated(histHandle)) Delay(1.5);
			tl->histInfo.completionCode = SUCCESS;
		}  else {
			MessagePopup("Installation Error", "Histogramming program not found.\nPlease notify Sofie Biosciences of this error.");
			tl->histInfo.completionCode = FAILURE;
		}
		RetireExecutableHandle (histHandle);
	
	}

	// Epilog  (final update to tasklist)
	time(&tl->histInfo.endTime);
  	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
	updateTaskStatus(&tl->histInfo, index, COLUMN_HIST);
	writeTaskList(tl);
	return 0;
}
/*
                                _______________________________________________
								
								Reconstruction in a wide sense: PET recon AND Atlas registration

*/

struct reconParm{
	int    sequenceNumber;
	struct taskList *tl;	
};

static int reconstruction(struct taskList* tl){ 
	
	char studyPathName[MAX_FILE_NAME_LENGTH];
	char dicomDirectoryName[MAX_FILE_NAME_LENGTH];
	struct reconParm theReconParm={globalSequenceNumber, tl};
	int index;
	
	CmtThreadFunctionID  petReconThreadID;
	CmtThreadFunctionID  atlasRegThreadID;
	
	// Empty the DICOM directory (actually: delete then create)
	makeStudyPathName(tl->study.tag, studyPathName);
	sprintfWithLock(MAX_FILE_NAME_LENGTH, dicomDirectoryName, "%s\\dicom", studyPathName);
	forceDeleteDir(dicomDirectoryName);

	// Create DICOM Directory 
	if ( 0!=MakeDir(dicomDirectoryName) ){
		writeToLog("runManagement::reconstruction - Error, can't create DICOM directory");
		return -1;
	}
	if ( 0!=SetFileAttrs(dicomDirectoryName, -1, 1, -1, -1 )) { // system atribute: so that GUI won't display it in study list
		writeToLog("runManagement::reconstruction - Error setting DICOM directory attributes");
		return -1;
	}

	// UPDATE status in GUI
	time(&tl->reconInfo.startTime);
	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
	updateTaskStatus(&tl->reconInfo, index, COLUMN_RECON);
	writeTaskList(tl);

	// Schedule Functions
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, PETreconstruction, &theReconParm, &petReconThreadID);
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, atlasRegistration, &theReconParm, &atlasRegThreadID);
	
	// Wait for completion
	CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, petReconThreadID, OPT_TP_PROCESS_EVENTS_WHILE_WAITING );
	CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, atlasRegThreadID, OPT_TP_PROCESS_EVENTS_WHILE_WAITING );
	
	// Recover index (may have changed if studies were added/deleted)
	time(&tl->reconInfo.endTime);
	tl->reconInfo.completionCode = 0;	  // Don't really know ...
	GetTreeItemFromTag(studyTabHandle,TAB_STUDY_TREE, tl->study.tag, &index);
	updateTaskStatus(&tl->reconInfo, index, COLUMN_RECON);
	writeTaskList(tl);
 	
	//Cleanup and return
	CmtReleaseThreadPoolFunctionID         (DEFAULT_THREAD_POOL_HANDLE, petReconThreadID );
	CmtReleaseThreadPoolFunctionID         (DEFAULT_THREAD_POOL_HANDLE, atlasRegThreadID );

	return 0;	
}


int CVICALLBACK atlasRegistration(void *functionData){
	struct reconParm* parm = (struct reconParm*) functionData;
	struct taskList* tl    = parm->tl;
	
	char studyPathName[MAX_FILE_NAME_LENGTH];
	char command[1024];
	int pgmHandle;

	// Get the study path name from tag
	makeStudyPathName(tl->study.tag, studyPathName);

	// Call the Atlas Registration Program
	sprintfWithLock(1024, command,"X_Ray_Atlas_Registration.exe \"%s\"", studyPathName);
	LaunchExecutableEx(command, LE_HIDE, &pgmHandle);
	while ( 0 == ExecutableHasTerminated(pgmHandle)) Delay(1.0);
	RetireExecutableHandle(pgmHandle);
	
	// Call the "CT" DICOM converter
	sprintfWithLock(1024, command,"dicomCT.exe \"%s\" \"%s\" %d %d", studyPathName, institutionName, serverStatus.serialNumber, parm->sequenceNumber);
	LaunchExecutableEx(command, LE_HIDE, &pgmHandle);
	while ( 0 == ExecutableHasTerminated(pgmHandle)) Delay(1.0);
	RetireExecutableHandle(pgmHandle);
	
	return 0;
	
}

int CVICALLBACK PETreconstruction(void *functionData){
	struct reconParm* parm = (struct reconParm*) functionData;
	struct taskList* tl    = parm->tl;
	
	int reconHandle;
	int dicomHandle;
	char searchName[MAX_FILE_NAME_LENGTH];
	char studyPathName[MAX_FILE_NAME_LENGTH];
	char temp[MAX_FILE_NAME_LENGTH];
	char uniqueID[MAX_FILE_NAME_LENGTH];

	char command[1024];

	// Set the search path for a sinogram file
	makeStudyPathName(tl->study.tag, studyPathName);
	sprintfWithLock(MAX_FILE_NAME_LENGTH, searchName, "%s\\*.scn", studyPathName); 
	
	int retCode = GetFirstFile(searchName, 1,0, 0, 0, 0, 0, temp);
	makeUniqueFileID(uniqueID, tl->study.name);

	// Delete existing images
	sprintfWithLock(MAX_FILE_NAME_LENGTH, searchName, "%s\\*.im3", studyPathName);
	DeleteFile(searchName);

	int cnt=0;
	char fullImagePathName[MAX_FILE_NAME_LENGTH];

	while(retCode == 0){
		sprintfWithLock(MAX_FILE_NAME_LENGTH, fullImagePathName, "%s\\%s_%m.im3", studyPathName, uniqueID, cnt);
		sprintfWithLock( sizeof(command), command,"recon.exe -i \"%s\\%s\" -o \"%s\" -P \"%s\\%s\" -I %d",
			studyPathName, temp, 	   // That's the sinogram input file
			fullImagePathName,         // That's the image output file
			normFolder,                // That's the enclosing folder of the P matrix
			pMatrix,                   // ... and that's the PMatrix
			tl->prot.iterations);	   //
		writeToLog("runManagement::reconstruction - *DEBUG*, issuing command");
		writeToLog(command);
		LaunchExecutableEx(command, LE_HIDE, &reconHandle);
		while ( 0 == ExecutableHasTerminated(reconHandle)) Delay(1.5);
		RetireExecutableHandle(reconHandle);
		retCode = GetNextFile(temp);  
		cnt++;
	}
	
	// Call the PET DICOM converter
	sprintfWithLock(1024, command,"DICOM.exe \"%s\" \"%s\" %d %d", studyPathName, institutionName, serverStatus.serialNumber, parm->sequenceNumber);
	LaunchExecutableEx(command, LE_HIDE, &dicomHandle);
	while ( 0 == ExecutableHasTerminated(dicomHandle)) Delay(1.0);
	RetireExecutableHandle(dicomHandle);
	
		
	return 0;
}
/*
                                _______________________________________________

*/
int CVICALLBACK confirm_ACQ_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			acquisitionConfirmation = 1;
			answerGiven=1;
			break;
	}
	return 0;
}
/*
                                _______________________________________________

*/
int CVICALLBACK cancel_ACQ_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			acquisitionConfirmation = 0;
			answerGiven=1;
			break;
	}
	return 0;
}


/*

*/

/* (this is similar to "studyTabRunCallback"; adapted to batch processing )  */
void  runBatchProtocol (int studyIndex, int protocolIndex){
	
	char   studyPath         [MAX_FILE_NAME_LENGTH];
	char   studyDataFileName [MAX_FILE_NAME_LENGTH];
	char   taskListFileName  [MAX_FILE_NAME_LENGTH];
	struct taskList  *tl=NULL;
	
	if (0!=createTaskList(&tl)) return;
	GetTreeItemTag(studyTabHandle,TAB_STUDY_TREE,studyIndex, tl->study.tag);

	// Get study Data
	if (0!=makeStudyPathName(tl->study.tag, studyPath)){
		writeToLog("runManagement::runBatchProtocol - ERROR, invalid study index");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}

	sprintfWithLock(MAX_FILE_NAME_LENGTH, studyDataFileName,"%s\\studyData.bin",studyPath);
	
	if (0!=readStudyData(studyDataFileName, &tl->study)){
		writeToLog("runManagement::runBatchProtocol - ERROR, I/O error reading study data");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}
	
	// Try to read an existing task list
	sprintfWithLock(MAX_FILE_NAME_LENGTH,taskListFileName,"%s\\taskList.bin",studyPath);
	int tlRetCode = readTaskList(taskListFileName, tl); 

	if ( tlRetCode == 0 ) {
		// Existing tasklist: merge new protocol with existing one
		if (0!=oldmergeProtocol(protocolIndex, tl)){
			writeToLog("runManagement::runBatchProtocol - ERROR, I/O error reading protocol data");
			MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
			return;
		}
	} else {
		// No preexisting task list, can read protocol as is
		if (0!=oldreadProtocol(protocolIndex, &tl->prot)){
			writeToLog("runManagement::runBatchProtocol - ERROR, I/O error reading protocol data");
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
			writeToLog("Alert - Acquisition is already running!");
			return;
		}
		
		if (TCPServerHandle<0) {
			free(tl);
			writeToLog("Alert - Can't acquire data without a connection to Genisys.");
			return;
		}
	}
	
	
	
	// All clear, Schedule task handler
	int threadID;
	if (0!=CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, protocolHandlerThreadNoConfirm, tl, &threadID)){
		writeToLog("runManagement::runBatchProtocol - ERROR, can't schedule thread");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return;
	}
	CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, threadID, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
	CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, threadID);
	
}
