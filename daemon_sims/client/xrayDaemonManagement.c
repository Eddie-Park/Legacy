

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
#include <tcpsupp.h>
#include <analysis.h>
#include <utility.h>
#include <time.h>
#include <userint.h>
#include <ansi_c.h>

#include "constants.h"
#include "typedefs.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "runManagement.h"
#include "utilities.h"
#include "logManagement.h"

#include "xrayDaemon.h"
#include "xrayDaemonManagement.h"


static int timeOut;

static void CVICALLBACK handleResponse (void *callbackData);
static char temp[1024];

static struct structInquireAcknowledge xrayDaemonStatus;
static struct structUnsollicited       xrayDaemonUncollicited;

static volatile int xrayDaemonStartAckReceived;
static volatile int xrayDaemonInqAckReceived;
static volatile int xrayDaemonUnsocReceived;
static volatile int xrayAbortRequested;
static volatile int xrayGoRequested;


#define MAX_ACK_RETRY 100

static int sendInquireStatus(void){
	struct PETBoxMessage message;
	char temp[1024];
	//char hex[128];
	
	if(xrayDaemonHandle<0) return 4; // No communications

	message.messageType = XDMN_MSG_TYPE_COMMAND;
	message.commandCode = XDMN_COMMAND_INQUIRE_STATUS;
	message.length      = (unsigned short) (  (char*)&message.inqAck - (char*)&message );

	/*
	putToHex(hex, &message);
	sprintfWithLock(1024, temp, "xrayDaemonManagement::sendInquireStatus - Sending inquire (%d bytes) - %s", message.length, hex);
	writeToLog(temp);
	*/

	xrayDaemonInqAckReceived=0;
	int bytesSent = ClientTCPWrite(xrayDaemonHandle, &message, message.length, timeOut=2000);
	
	if (bytesSent<=0) {
		sprintfWithLock(1024, temp, "xrayDaemonManagement::sendInquireStatus - Error %d in TCPWrite - %s", bytesSent,GetTCPErrorString(bytesSent) ); 
		writeToLog(temp);
		return -1;
	}
	
	// Wait for response
	int retryCount=MAX_ACK_RETRY;
	while (retryCount-->0){
		if (xrayDaemonInqAckReceived) break;
		else {
			Delay (0.1);
			// ProcessSystemEvents is required to process TCP callback (in this thread) because RunUserInterface may not have been issued yet
			ProcessSystemEvents();  
		}
	}
	// No ACK received
	if (!xrayDaemonInqAckReceived) {
		writeToLog("xrayDaemonManagement::sendInquireStatus - Time out, NO INQUIRE STATUS ACK Received\n");
		return -1;
	}
	
	return 0;
}

// ----------------------------------------------------------------------------------

int xrayDaemonInitialization(void){
	sendInquireStatus();
	return 0;

}


// ----------------------------------------------------------------------------------


int xrayDaemonTCPCallbackFunction (unsigned handle, int xType, int errCode, void *callbackData){
	int bytesRead;

	switch(xType){
		struct PETBoxMessage *message;
		
		case TCP_DISCONNECT:
			xrayDaemonHandle = -1;
			SetCtrlAttribute( idleTabHandle , IDLE_PANEL_reconTimer, ATTR_ENABLED, 1); // Schedule automatic reconnection
			writeToLog("Disconnect received from xray daemon");
			break;
			
		case TCP_DATAREADY:
			message = malloc( sizeof(*message) ); // Message will be freed by handle response
			
			if (message == NULL){
				writeToLog("xrayDaemonManagement::tcpCallbackFunction - Error, no memory for message.  Closing communications.");
				return -1;
			}
			
			// Get ONE buffer from TCP (data should be readily available)
			bytesRead = ClientTCPRead(xrayDaemonHandle, message, 4, timeOut=50);	 // Read only 4 first bytes (length, type and code)
			if (bytesRead>=0){
				sprintfWithLock(sizeof(temp),temp,"[DEBUG] xrayDaemonManagement::tcpCallbackFunction - Reading length (%d bytes read); length= %d", bytesRead, message->length); 
				traceToLog(temp);

				bytesRead = ClientTCPRead(xrayDaemonHandle, &(message->commandCode), message->length-4, timeOut=500); // Read the rest of this message    
				if (bytesRead != message->length-4){
					sprintfWithLock(sizeof(temp),temp,"xrayDaemonManagement::tcpCallbackFunction - ERROR reading rest of message; bytes read was %d, was supposed to read %d", bytesRead, message->length-4); 
					writeToLog(temp); 
				}
				
				// Not a direct call here because we have to get out of here quickly
				int retCode = PostDeferredCall(handleResponse, message);
				if (retCode<0){
					writeToLog("xrayDaemonManagement::tcpCallbackFunction - Error in PostDeferredCall.");
				}	
			}
			
			// Negative values for bytes read is an error code
			else if (bytesRead != -kTCP_TimeOutErr){
				sprintfWithLock(sizeof(temp),temp,"xrayDaemonManagement::tcpCallbackFunction - ERROR in tcp read: %d", bytesRead); 
				writeToLog(temp); 
			}
			
			break;
			
		default:
			writeToLog("xrayDaemonManagement::tcpCallbackFunction - Error, Unknown TCP xType.");
			break;
	}
	
	return 0;
}


// ----------------------------------------------------------------------------------


static void CVICALLBACK handleResponse (void *callbackData){
	struct PETBoxMessage *message = (struct PETBoxMessage*) callbackData;

	char temp[1024];

	switch (message->messageType){
		
		case XDMN_MSG_TYPE_NACK:
			writeToLog("xrayDaemonManagement::handleResponse - Negative acknowledge received");
			break;
			
		case XDMN_MSG_TYPE_ACK:
			
			switch(message->commandCode ){
				
				case XDMN_COMMAND_START:
					xrayDaemonStartAckReceived=1;
					writeToLog("xrayDaemonManagement::handleResponse - Start acknowledge received.");
					break;
					
				case XDMN_COMMAND_INQUIRE_STATUS:
					xrayDaemonInqAckReceived=1; 
					myMemcpy((char*)&xrayDaemonStatus, (char*)&message->inqAck, (int) sizeof(message->inqAck) );
					/*
					sprintfWithLock(sizeof(temp),temp,
						"xrayDaemonManagement::handleResponse - Inquire status acknowledge received, set to %d",xrayDaemonInqAckReceived ); 
					writeToLog(temp);
					*/
					break;
			}
			break;

		case XDMN_MSG_TYPE_UNSOC:
			xrayDaemonUnsocReceived=1;
			myMemcpy((char*)&xrayDaemonUncollicited, (char*)&message->unsoc, (int) sizeof(message->unsoc) );
			sprintfWithLock(sizeof(temp), temp, "xrayDaemonManagement::handleResponse - Unsollicited status received, l:%d, sof:%d", message->length, sizeof(message->unsoc) );
			writeToLog(temp);
			break;

		case XDMN_MSG_TYPE_ATTN:
			writeToLog("xrayDaemonManagement::handleResponse - ATTENTION received.");
			break;
			
			
		default:
			sprintfWithLock(sizeof(temp),temp,"ERROR - Unknown message type: %d",message->messageType);
			writeToLog(temp);
			break;
	}
	
	free(message);
}


// ----------------------------------------------------------------------------------

static int sendStart(struct taskList* tl){

	struct PETBoxMessage message;
	char temp[1024];
	char tmp[40];
	
	xrayDaemonStartAckReceived=0;
	message.messageType = XDMN_MSG_TYPE_COMMAND;
	message.commandCode = XDMN_COMMAND_START;
	
	message.startCmd.startCommandRequest  = XDMN_START_FLAG_XRAY + XDMN_START_FLAG_PHOTO;
	myStrcpy(message.startCmd.studyID, tl->study.name);
	makeStudyPathName(tl->study.tag, message.startCmd.studyPathName);
	message.length = (unsigned short) (    ((char*)&message.startCmd.studyPathName - (char*)&message) + strlen(message.startCmd.studyPathName) + 1   );

	int bytesSent = ClientTCPWrite(xrayDaemonHandle, &message, message.length, timeOut=5000);
	putToHex(tmp, &message);
	sprintfWithLock(1024, temp, "Sending XRAY start (%d bytes) in study %s, tag %s, path %s - %s", bytesSent, tl->study.name, tl->study.tag, message.startCmd.studyPathName, tmp);
	writeToLog(temp);

	if (bytesSent<=0) {
		sprintfWithLock(1024, temp, "xrayDaemonManagement::xrayAcquisition - Error %d in TCPWrite - %s", bytesSent,GetTCPErrorString(bytesSent) ); 
		writeToLog(temp);
		return 8;
	}


    // Wait for START acknowledge
	int retryCount=MAX_ACK_RETRY;
	while (retryCount-->0){
		if (xrayDaemonStartAckReceived) break;
		else Delay (0.1);
		ProcessTCPEvents();
	}
	
	
	// No ACK received?
	if (!xrayDaemonStartAckReceived) {
		writeToLog("xrayDaemonManagement::xrayAcquisition - Time out, NO START ACK Received");
	    return 8;
	}
	
	return 0;
}

// 
static void transferFile(char* inputPath){
	char onlyFileName [MAX_FILENAME_LEN];	
	char fullFilePath [MAX_PATHNAME_LEN];
	
	SplitPath( inputPath, NULL, NULL, onlyFileName);
	sprintfWithLock(MAX_PATHNAME_LEN, fullFilePath, "%s\\%s", xrayDaemonUncollicited.studyPathName, onlyFileName );
	
	
	if ( 0==CopyFile(inputPath, fullFilePath) ){
		if (0 != DeleteFile(inputPath) ){
			sprintfWithLock(1024, temp, "xrayDaemonManagement::xrayAcquisition - WARNING, file not deleted: %s",  inputPath);
			writeToLog(temp);
		}
	} else {
		sprintfWithLock(1024, temp, "xrayDaemonManagement::xrayAcquisition - ERROR, file could not be copied: %s", inputPath);
		writeToLog(temp);
	}

}

// ----------------------------------------------------------------------------------

int  xrayAcquisition (struct taskList* tl){
	
	int retryCount;
	int retCode=0;
	
	if(xrayDaemonHandle<0) {
		writeToLog("xrayDaemonManagement::xrayAcquisition - WARNING, xray requested but daemon not connected");
		return 4; // No communications
	}

	// Display XRAY Panel
	SetCtrlVal(      xrayPanel,   TAB_XRAY_protocolName,  tl->prot.name  );
	SetCtrlVal(      xrayPanel,   TAB_XRAY_studyName,     tl->study.name );
	SetCtrlAttribute(xrayPanel,   TAB_XRAY_CANCEL_BUTTON, ATTR_DIMMED,  0);
	SetCtrlIndex(    xrayPanel,   TAB_XRAY_PICTURERING,   1              );
	SetCtrlAttribute(xrayPanel,   TAB_XRAY_OK_BUTTON,     ATTR_VISIBLE, 0);
	fadeInPanel(&xrayPanelFading);
	
	// Verify/wait if chamber present
	xrayAbortRequested=0;
    xrayDaemonStatus.safety_status_read=0;
	xrayGoRequested=0;
	
//	while(  0==(xrayDaemonStatus.status & XDMN_INQ_STAT_CHAMBER_PRESENT) && !xrayGoRequested) {
//	while( !xrayGoRequested) {
	while(  0==xrayDaemonStatus.safety_status_read) {
		if ( 0!= sendInquireStatus() ) {
			fadeOutPanel(&xrayPanelFading);
			return -1;
		}
		
		Delay(0.2);
		ProcessSystemEvents();
		if(xrayAbortRequested) {
			fadeOutPanel(&xrayPanelFading);
			return -1;
		}
	}

	// Good to go! Send START to server
	SetCtrlAttribute(xrayPanel, TAB_XRAY_OK_BUTTON, ATTR_VISIBLE, 0);
	SetCtrlAttribute(xrayPanel, TAB_XRAY_TIMER, ATTR_ENABLED, 1);				// Make smooth transition to "Acquiring xray" 
	SetCtrlAttribute(xrayPanel, TAB_XRAY_CANCEL_BUTTON, ATTR_DIMMED, 1);
	if ( 0 != sendStart(tl) ) {
		fadeOutPanel(&xrayPanelFading);
		return -1;
	}
	
	// Wait for UNSOLLICITATED status
	xrayDaemonUnsocReceived=0;
	retryCount=MAX_ACK_RETRY;
	while (retryCount-->0){
		if (xrayDaemonUnsocReceived) break;
		else Delay (0.2);
		ProcessTCPEvents();
	}
	
	// No status received?
	if (!xrayDaemonUnsocReceived) {
		writeToLog("xrayDaemonManagement::xrayAcquisition - Time out: no unsollicited status received after start xray");
		retCode = 8;
	}
	
	// Was everything OK?
	if ( xrayDaemonUncollicited.completionCode != XDMN_UNSOC_COMP_SUCCESS ){
		sprintfWithLock(1024, temp, "xrayDaemonManagement::xrayAcquisition - XRAY failure, reason code %d", xrayDaemonUncollicited.error ); 
		writeToLog(temp);
		retCode = xrayDaemonUncollicited.error;
	}  else {
		// Copy files
		transferFile(xrayDaemonUncollicited.pathToLocalXray);
		transferFile(xrayDaemonUncollicited.pathToLocalPhoto);
	}
	
 	fadeOutPanel(&xrayPanelFading);
	return retCode;	

}

// ----------------------------------------------------------------------------------

void xrayDaemonTermination(void){
	if (xrayDaemonHandle>=0) DisconnectFromTCPServer(xrayDaemonHandle);
	xrayDaemonHandle = -1;
}

// ----------------------------------------------------------------------------------


int CVICALLBACK xrayServer_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			xrayDaemonInitialization();
			break;
	}
	return 0;
}

// ----------------------------------------------------------------------------------


int CVICALLBACK cancel_XRAY_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			xrayAbortRequested=1;
			break;
	}
	return 0;
}

int CVICALLBACK xrayRingTimer_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int value;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetCtrlVal(    xrayPanel,   TAB_XRAY_PICTURERING,   &value  );      
			if   (value<10) SetCtrlVal( xrayPanel,   TAB_XRAY_PICTURERING,  value+1  );
			else SetCtrlAttribute(xrayPanel, TAB_XRAY_TIMER, ATTR_ENABLED, 0);
			break;
	}
	return 0;
}

int CVICALLBACK xrayGO_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			xrayGoRequested=1;
			break;
	}
	return 0;
}
