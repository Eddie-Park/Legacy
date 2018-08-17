//*******************************************************************************
//*                                                                             *
//*  PETBox Server Software                                                     *
//*                                                                             *
//*  Copyright - 2009                                                           *
//*                                                                             *
//*  This code is the property of the Regents of UCLA.                          *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Richard Taschereau and Hui Zhang                                *
//*                                                                             *
//*  Created: September 2009                                                    *
//*                                                                             *
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************

#include "vhsadac16_regs.h"
#include "vhsadc16ctrl.h"
#include "vhsadc16ctrl_v4.h"
#include "vhsadac16_host_api.h"
#include <NIDAQmx.h>
#include <formatio.h>

#include <userint.h>
#include <tcpsupp.h>
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <mmsystem.h>

#include "commonHeader.h"
#include "GlobalDecls.h"
#include "embedded.h"
#include "server.h"

// ------------- Prototypes
void writeToLog(char* string);
void CVICALLBACK handleCommand (void *callbackData);
int  CVICALLBACK acquisitionThread (void *functionData);
void respondToInquireStatus(struct PETBoxMessage *buffer);
void sendUnsollicitedStatus(struct PETBoxMessage *buffer);
void CVICALLBACK handleCommand (void *callbackData);
int tcpCallback (unsigned handle, int xType, int errCode, void *callbackData);
void UpdatePanel(void);

// acquisition related prototypes
void VHS_FPGA_Run(int bRun);
int VHS_FPGA_Set_Regs(void);
void CALLBACK AcqTimerCallBackProc( UINT timerid, UINT msg, DWORD user, DWORD dw1, DWORD dw2 );
void saveRawData(void);

int loadLUT(HANDLE hVHSdevice);
int fpgaLoad(HANDLE hVHSdevice, char* filePath);


// ------------- Local variables
static int  externalPCHandle;
static char externalPCName[200];

static char studyID[MAX_STUDY_LENGTH];
static char studyPathName[MAX_FILE_NAME_LENGTH];

static char tempString[2048];	   // local temporary work string to be used with sprintf, etc

// acquisition statistics
static int timeLeft=0;
static unsigned int promptsRate=0;
static unsigned int randomsRate=0;
static unsigned int det0SinglesRate=0;
static unsigned int det1SinglesRate=0;
static unsigned int det2SinglesRate=0;
static unsigned int det3SinglesRate=0;

static float vhsPeriod;
static float vhsPeriodInNanoSec;
static float rateAdjustmentFactor;
const static float assumedPeriodInNanoSec=10.0;
const static float nanoSec = 1E-9;

// acquisition hardware handle
static HANDLE hVHSdevice=NULL;

// acquisition buffers and buffer index
static PUCHAR acqBuffer=NULL;
static PUCHAR acqWritePointer=NULL;
static PUCHAR acqReadPointer=NULL;
static unsigned int curBuffer=0;
static unsigned long numRxBytes=0;
static unsigned long numSavedBytes=0;
static unsigned long numCallBacks=0;
static unsigned long numDiskWrite=0;
static unsigned long numBufferRead=0;
static unsigned int  dumpIndex=0;
static unsigned int  dumpRawData=0;
static unsigned int  panelUpdate=0;


// acquisition timer
MMRESULT  timerID;
UINT     wTimerRes;
unsigned long acqPresetTime, acqStTime,acqCurTime,acqEndTime;


// Converts the first 16 bytes of "in" to hexadecimal characters
// and puts the result in "out"
void putToHex(char* out,void* in){
	int i,j;
	(*out)=0;
	for (j=0; j<16; j+=4)
		for (i=j; i<j+4; i++)
			sprintf(out+2*i, "%02X", ((unsigned char*)in)[i]); 
}

// Gets a list of all possible IP adresses this computer has
void getLocalAddresses(){
	char ** addresses = NULL;
	int numAddresses;
	int index;
	int retCode;

	writeToLog("List of possible IP addresses:");
	retCode = GetAllTCPHostAddresses (&addresses, &numAddresses);
	if (retCode!=0) return;
	
	for (index = 0; index < numAddresses; index++){
		sprintf(tempString,"     %s",addresses[index]);
		writeToLog(tempString);
		TCPFreeMemory (addresses[index]); 
	}
	TCPFreeMemory (addresses);
}									  

/*
	PETBox server initialization
		- register as a TCP server
*/
int serverInitialization(){
	int retCode;
	externalPCHandle = 0;
	
	vhsPeriod            = 1.0 / VHS_CLOCK_FREQUENCY;
	vhsPeriodInNanoSec   = vhsPeriod/nanoSec;
	rateAdjustmentFactor = assumedPeriodInNanoSec / vhsPeriodInNanoSec;

	getLocalAddresses();
	retCode = RegisterTCPServer(PORT_NUMBER, tcpCallback, 0);
	if (retCode == 0) {
		sprintf(tempString,"Successfully opened port # %d", PORT_NUMBER);
		writeToLog(tempString);
	}
	else {
		sprintf(tempString,"Error opening port # %d, retCode = %d", PORT_NUMBER, retCode);
		writeToLog(tempString);
	}
	acquisitionInProgress=0;
	SetCtrlAttribute(mainPanel,PANEL_det3SinglesRate, ATTR_DIMMED, 1);

	return retCode;
}

/*
	PETBox server termination
		- (force) disconnect from any client
*/
void serverTermination(){
	int retCode;
	
 	if (externalPCHandle) DisconnectTCPClient(externalPCHandle);
	retCode = UnregisterTCPServer(PORT_NUMBER);
	
	if (retCode == 0) {
		sprintf(tempString,"Port # %d closed", PORT_NUMBER);
		writeToLog(tempString);
	}
	else {
		sprintf(tempString,"Error closing port # %d", PORT_NUMBER);
		writeToLog(tempString);
	}
}


/*
	VHS board initialization
*/
int acqHardwareInitialization(){
	
	int iStatus=0;
	
	 
	// Open VHS-ADAC API driver
	iStatus = 0;

	
		sprintf(tempString,"Acquisition hardware found.");
		writeToLog(tempString);
	
	
	//  LOAD LUT LOADER
	sprintf(tempString,"Loading LUT Loader %s", lutLoaderFilePath);
	SetCtrlVal(mainPanel, PANEL_buildID, tempString);
	ProcessSystemEvents();
	
	sprintf(tempString, "%s%s", systemFilesPath, lutLoaderFilePath);
	
	
	//load LUT
	sprintf(tempString,"Loading LUT %s", lutFilePath);
	SetCtrlVal(mainPanel, PANEL_buildID, tempString);
	ProcessSystemEvents();
	iStatus = 0;
	
	//load FPGA bitstream (acquisition firmware) from file
	sprintf(tempString,"Loading DAQ %s", FPGABitstreamFilePath);
	SetCtrlVal(mainPanel, PANEL_buildID, tempString);
	ProcessSystemEvents();
	
	sprintf(tempString, "%s%s", systemFilesPath, FPGABitstreamFilePath);
	iStatus = 0;
	
	
	
	ProcessSystemEvents();
		
	
	// LED Stuff
	int retCode=0;
	PET_On=0;
	
	//PET LED initialization
	PET_On=0;
	
	acqHardwareRdy=1;
	SetCtrlVal(mainPanel,PANEL_ACQ_HW_READY, 1);
	sprintf(tempString,"Acquisition hardware initialized successfully.");
	writeToLog(tempString);

	return retCode;
}

/*
	VHS board and 6008 termination
*/
void acqHardwareTermination(){
	
	
		sprintf(tempString,"Acquisition hardware released.");
		writeToLog(tempString);
	
}


int loadLUT(HANDLE hVHSdevice){
	
	return 0;
}


int fpgaLoad(HANDLE hVHSdevice, char* filePath)
{
	
	writeToLog("Load bitstream file successfully.");
	
	return 0;
}


/*
   Handles (asynchronous) TCP callbacks from LabWindows/CVI
*/
int tcpCallback (unsigned handle, int xType, int errCode, void *callbackData){
	struct PETBoxMessage *dataBuffer;
	int retCode;
	char temp[40];
	memset (temp, 0, 40);
	
	switch(xType){
		
		// Connection request; accept connection only if we're not already connected
		case TCP_CONNECT:
			if (externalPCHandle !=0){
				writeToLog("Unexpected connection request received");
				DisconnectTCPClient(handle);
				return 0;
			}
			externalPCHandle = handle;
			GetTCPPeerAddr(externalPCHandle, externalPCName, sizeof(externalPCName) );
			sprintf(tempString,"Connection accepted from %s, handle value %d", externalPCName, externalPCHandle);
			writeToLog(tempString);
			SetCtrlVal(mainPanel,PANEL_CONNECTED_LED, 1);
			break;
			
		// Disconnection request
		case TCP_DISCONNECT:
			retCode = DisconnectTCPClient(externalPCHandle);
			sprintf(tempString,"Disconnected from %s, errCode %d, retcode from disconnect %s",externalPCName, 
				errCode, GetTCPErrorString(retCode) );
			writeToLog(tempString);
			externalPCHandle=0;
			SetCtrlVal(mainPanel,PANEL_CONNECTED_LED, 0);
			break;
			
		// Data has been received (a command from the GUI)
		case TCP_DATAREADY:
			dataBuffer = malloc(sizeof(*dataBuffer));
			retCode = ServerTCPRead(externalPCHandle, dataBuffer, sizeof(*dataBuffer), 0);
			if (retCode < 0) {
				sprintf(tempString, "Error %d in TCPRead: %s", retCode, GetTCPSystemErrorString());
				writeToLog(tempString);
			}
			// Deferred call to "handleCommand" function
			// (as opposed to direct call) to return to
			// the system ASAP
			putToHex(temp, dataBuffer);
			sprintf(tempString, "Received command (%d bytes) - %s", retCode, temp);
			writeToLog(tempString);
			PostDeferredCall(handleCommand, dataBuffer);
			break;
	}
	return 0;	
}

/*
	"Handle command" function
		A command (start, stop, or inquire status) has been received (in buffer).
*/
void CVICALLBACK handleCommand (void *callbackData){
	struct PETBoxMessage *buffer = (struct PETBoxMessage *) callbackData;	
	char temp[128];
	
	switch(buffer->commandCode){
		
		// If no acquisition running, accept the start and start the acquisition thread
		case COMMAND_START:
			if (acquisitionInProgress==0 && acqHardwareRdy==1){
				acquisitionInProgress=1;
				CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, acquisitionThread, buffer, &acquisitionThreadFunctionID);
				// free(buffer) will be done by the acquisition thread
				// acknowledge will also be sent by the thread
			} else{
				buffer->messageType = TYPE_NACK;
				buffer->length = (char*)&buffer->startAck - (char*)buffer;
				ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				sprintf(temp,"NACK sent to start command; acquisition in progress = %d; hardware ready = %d", acquisitionInProgress, acqHardwareRdy);
				writeToLog(temp);
				free(buffer);
			}
			break;
		
		// If acquisition in progress, accept stop, request the thread to stop and send acknowledge
		case COMMAND_STOP:
			if (acquisitionInProgress){
				acquisitionInProgress = 0;
				while(acquisitionThreadRunning)Delay(0.010);
				CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,acquisitionThreadFunctionID);
				buffer->messageType = TYPE_ACK;
				buffer->length = (char*)&buffer->stopAck - (char*)buffer;
				ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
			} else{
				buffer->messageType = TYPE_NACK;
				buffer->length = (char*)&buffer->stopAck - (char*)buffer;
				ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				writeToLog("NACK sent to stop command");
			}
			free(buffer);
			break;
			
		// Respond to inquire status by sending current status with positive acknowledge
		case COMMAND_INQUIRE_STATUS:
			respondToInquireStatus(buffer);
			free(buffer);
			break;
	}
}

void makeUniqueFileID(char* outFileName){
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
	
	memcpy(theTime.timeString, TimeStr(), 8);
	theTime.filler1=0;
	theTime.filler2=0;
	theTime.filler3=0;
	
	memcpy(theDate.dateString, DateStr(), 10);
	theDate.filler1=0;
	theDate.filler2=0;
	theDate.filler3=0;

	sprintf(outFileName,"%s\\1.2.840.%d.%d.%d.%d.%s.%s%s%s.%s%s%s.lmf",
		rawDataFilePath, 
		dicomRegID, 
		productID,
		serialNumber,
		++globalSequenceNumber,
		studyID,
		theDate.mm, theDate.dd, theDate.yyyy,
		theTime.hh, theTime.mm, theTime.ss
		 );
	
	RegSetValueEx(petBoxRegKey, "GlobalSequenceNumber",  0, REG_DWORD,  (LPBYTE)&globalSequenceNumber,   4);
	
}

/*
	ACQUISITION THREAD
	
	The thread is started upon reception of a start command.  Parameters
	and contained in the buffer.  Controls in the local GUI panel are
	also updated.
*/
int  CVICALLBACK acquisitionThread (void *functionData){
	struct PETBoxMessage *buffer = (struct PETBoxMessage*) functionData;
	DWORD	  dwUser;
	int iStatus =0;
	
	// set the sleep policy
	SetSleepPolicy (VAL_SLEEP_SOME);
	

	/* Extract required info from buffer */
	timeLeft = buffer->startCmd.duration;
	// break if acquisition time is not set correctly (<=0)
	if(timeLeft<=0)
	{
		sprintf(tempString, "Error: Acquisition time not specify correctly. Acquisition not started.");
		writeToLog(tempString);
		return -1;
	}
	strcpy(studyID, buffer->startCmd.studyID);
	strcpy(studyPathName, buffer->startCmd.studyPathName);
	
	/* Prepare unique id for file name and acknowledge */
	makeUniqueFileID(rawDataFileName);
	
	
	buffer->messageType = TYPE_ACK;
	strcpy(buffer->startAck.lmfFileName, rawDataFileName);
	buffer->length = strlen(rawDataFileName) + 1 + (char*)&buffer->startAck.lmfFileName - (char*)buffer;
	writeToLog("server::acquisitionThread - Sending ACK");
	int bytesWritten = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
	if (bytesWritten != buffer->length){
		writeToLog("server::acquisitionThread - Error sending acknowledge: bad byte count");
	}

	
	curBuffer=0;
	numRxBytes=0;
	numSavedBytes=0;
	numCallBacks=0;
	numDiskWrite=0;
	numBufferRead=0;
	dumpIndex=0;
	acqPresetTime=0;
	acqStTime=0;
	acqCurTime=0;
	acqEndTime=0;
	//VHS_FPGA_Set_Regs();	

 	// open file to save raw data
	MakeDir(rawDataFilePath);
								
	int result = RenameFile("n:\\PETbox_StudyData\\sample.lmf", rawDataFileName);
	if (result != 0)
		sprintf(tempString,"Acquisition raw data error:%d filename = %s\n", result, rawDataFileName);  
	else
		sprintf(tempString,"Acquisition raw data file created as: %s.", rawDataFileName);
	
	writeToLog(tempString);	
	
	
	//start PED LED
	PET_On=1;
	

	// Update GUI Panel
	SetCtrlVal(mainPanel,PANEL_ACQ_LED, 1);
	SetCtrlVal(mainPanel, PANEL_TIME_LEFT, timeLeft);
	SetCtrlVal(mainPanel, PANEL_SavedBytes,     numSavedBytes); 
	SetCtrlVal(mainPanel, PANEL_STUDY_ID,  studyID);
	SetCtrlAttribute(mainPanel,PANEL_STUDY_ID,        ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_TIME_LEFT,       ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_SavedBytes,       ATTR_DIMMED, 0); 
	SetCtrlAttribute(mainPanel,PANEL_promptsRate,     ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_randomsRate,     ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_det0SinglesRate, ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_det1SinglesRate, ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_det2SinglesRate, ATTR_DIMMED, 0);
	SetCtrlAttribute(mainPanel,PANEL_det3SinglesRate, ATTR_DIMMED, 0);

	// start windows MM timer
	TIMECAPS tc;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
	{
		sprintf(tempString,"Error: Can't start Windows MM timer\nAcquisition aborted.");
		writeToLog(tempString);
		return -1;
	}

	// set timer period
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	timeBeginPeriod(wTimerRes); 

	acquisitionThreadRunning=1;
	// write to log acquisition start info
	sprintf(tempString, "Acquisition started, duration: %d secs., study %s, path %s", timeLeft, studyID, studyPathName);
	writeToLog(tempString);
	
	sendUnsollicitedStatus(buffer);

	// start timer event
    timerID = timeSetEvent(msTimerPeriod,wTimerRes, (LPTIMECALLBACK)&AcqTimerCallBackProc,(DWORD_PTR) &dwUser, TIME_PERIODIC|TIME_KILL_SYNCHRONOUS ); 

    if(timerID==0)
	{	
		sprintf(tempString,"Error: Can't start Windows MM timer\nAcquisition aborted.");
		writeToLog(tempString);
	
		acquisitionThreadRunning=0;
		return -1;
	}
	
	// set the run signal

	acqStTime=timeGetTime();
	acqPresetTime=timeLeft;
	acquisitionInProgress=1;

	writeToLog("About to Start Acquisition While Loop\n");
	// main loop during acquisition
	while (acquisitionInProgress && timeLeft>0 )
	{
			 
		/* update local panel */
		if (panelUpdate==1)
		{
			panelUpdate=0;
			UpdatePanel();
			sendUnsollicitedStatus(buffer);
			writeToLog("acquisition thread: panelUpdate\n");
		}
		Delay(0.005);
	}
	
	//stop PED LED
	PET_On=0;
	

	// prepare to stop the acquisition
	sendUnsollicitedStatus(buffer);

	// stop timer
	acqEndTime=timeGetTime();

	if (timerID)
		timeKillEvent(timerID);

	//end timer period
	timeEndPeriod(wTimerRes);

	unsigned int fpgaPromptCounts,numCountsRead;
	
	
	writeToLog("Acquisition stopped");
	timeLeft=0;
	
	// Update Panel
	SetCtrlVal(mainPanel, PANEL_TIME_LEFT, timeLeft);
	SetCtrlVal(mainPanel,PANEL_ACQ_LED, 0);
	SetCtrlAttribute(mainPanel,PANEL_STUDY_ID,       ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_SavedBytes,     ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_TIME_LEFT,      ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_promptsRate,    ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_randomsRate,    ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_det0SinglesRate, ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_det1SinglesRate, ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_det2SinglesRate, ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel,PANEL_det3SinglesRate, ATTR_DIMMED, 1);
	
	// Reset acquisition variables	
	(*studyID)               = 0;
	(*studyPathName)         = 0;
	(*rawDataFileName)       = 0;
	acquisitionThreadRunning = 0;
	acquisitionInProgress    = 0;
	timeLeft                 = 0;
	promptsRate              = 0;
	randomsRate              = 0;
	det0SinglesRate          = 0;
	det1SinglesRate          = 0;
	det2SinglesRate          = 0;
	det3SinglesRate          = 0;
	
	free(buffer);
	return 0;

}

void CALLBACK AcqTimerCallBackProc( UINT timerid, UINT msg, DWORD user, DWORD dw1, DWORD dw2 )
{

	unsigned int fpgaStatus;
	unsigned int fpgaHalfFull=0;
	unsigned int numCountsRead=0;
	unsigned long elapsedTime=0;

	numCallBacks++;

	
	acqCurTime=timeGetTime();
	elapsedTime=(acqCurTime-acqStTime)/1000;
	timeLeft=acqPresetTime-elapsedTime;

	if (timeLeft<=0)
	{
		timeLeft=0;
		writeToLog("Acquisition time over.");
		return;
	}


	if (numCallBacks%panelUpdateFreq==0)
		panelUpdate=1;


	return;
}	
	
	
void UpdatePanel()	
{

	return;
}

void saveRawData()
{

	return;	
}

/*
	Respond to inquire status
	
	This function sends a control message, either positive acknowledge or unsollicited
	statistics message to the GUI.  The parameter typ indicates which one it is.
	
	A positive acknowledge is sent in response to an inquire status command.  It contains
	current statistics, current status and study identification, if any.
	
	An unsollocited statistics message in sent during acquisition at regular intervals.
	It is shorter than the positive acknowledge since it contains only statistics and status
	(no study identification).
*/

void respondToInquireStatus(struct PETBoxMessage *buffer){
	int retCode;
	char temp[40];
	
	if (externalPCHandle){
		buffer->messageType                = TYPE_ACK;
		buffer->inqAck.serialNumber = serialNumber;
		strcpy(buffer->inqAck.studyID,       studyID      );
		strcpy(buffer->inqAck.studyPathName, studyPathName);
		strcpy(buffer->inqAck.lmfFileName,   rawDataFileName);
		
		// Prepare status field
		buffer->inqAck.status      = 0;
		if (acquisitionThreadRunning) 
			buffer->inqAck.status +=  STATUS_ACQ_RUNNING;
		
		buffer->length = (char*)&buffer->inqAck.lmfFileName - (char*)buffer + strlen(rawDataFileName) +1;   // +1 to include '0' at the end of the string
		
		retCode = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
		if (retCode < 0) {
			sprintf(tempString, "Error %d in TCPWrite: %s", retCode, GetTCPSystemErrorString());
			writeToLog(tempString);
		}
		putToHex(temp, buffer);
		sprintf(tempString, "Sending response (%d bytes) to inquire status - %s", buffer->length, temp);
		writeToLog(tempString);
	}
}

void sendUnsollicitedStatus(struct PETBoxMessage *buffer){
	int retCode;
	char temp[40];
	memset (temp, 0, sizeof(temp));
	if (externalPCHandle){
		buffer->messageType            = TYPE_UNSOC;
		buffer->commandCode            = 0;
		buffer->unsoc.timeLeft         = timeLeft;
		buffer->unsoc.promptsRate      = promptsRate;
		
		buffer->unsoc.status           = 0;
		if (acquisitionThreadRunning) 
			buffer->unsoc.status +=  STATUS_ACQ_RUNNING;

		buffer->length = (char*)&buffer->unsoc - (char*)buffer + sizeof(buffer->unsoc);
		
		retCode = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
		if (retCode < 0) {
			sprintf(tempString, "Error %d in TCPWrite: %s", retCode, GetTCPSystemErrorString());
			writeToLog(tempString);
		}
		putToHex(temp, buffer);
		sprintf(tempString, "Sending unsollicited Status (%d bytes) - %s", buffer->length, temp);
		writeToLog(tempString);
	}
}


/*
	Start or stop FPGA on the acquisition board
*/
void VHS_FPGA_Run(int bRun)
{


	return;
}


/*
	Set FPGA registers
	Reg4: changed from Channel34 Offset(old) to status Test for RTDex(new)
	Reg5: changed from Channel12 Offset(old) to Input energy window(new)
	Reg6: changed from Channel78 Offset(old) to head2 singles show(new)
	Reg7: changed from Channel56 Offset(old) to head3 singles show(new)
*/

int VHS_FPGA_Set_Regs(void)
{

	return 1;


}
