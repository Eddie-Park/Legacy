
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
#include "constants.h"
#include "typedefs.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "communicationsManagement.h"
#include "runManagement.h"
#include "utilities.h"
#include "logManagement.h"
#include "commonHeader.h"
#include "xrayDaemonManagement.h"
#include <ansi_c.h>


static const int timeOutDefault=2000;  //    in milliseconds, time out for TCP connect
static int timeOut;
static CmtThreadLockHandle tcpLock=0;

int tcpCallbackFunction (unsigned handle, int xType, int errCode, void *callbackData);
void CVICALLBACK handleResponse (void *callbackData);

// Connects (OR reconnects to both Daemons)
int communicationsInitialization(void){
	int retCode;
	char temp[1024];
//	SetSleepPolicy(VAL_SLEEP_NONE);	   // DON'T do that, it's is steeling from breathing monitoring
	
	// Create lock for TCP callback function
	retCode = CmtNewLock(NULL, 0, &tcpLock);
	
	if (retCode<0){
		sprintfWithLock(1024, temp,"communicationsManagement::communicationsInitialization - Failed to create lock, retcode %d", retCode); 
		writeToLog(temp);
		return -1;
	}
	
	// Initialize communications with PET Daemon (if not already connected)
	if (TCPServerHandle <0){
		retCode = ConnectToTCPServer((unsigned int*) &TCPServerHandle, portNumber, ipAddress, tcpCallbackFunction, 0, timeOut=timeOutDefault);
		if (retCode!=0)	{
			SetCtrlAttribute(idleTabHandle, IDLE_PANEL_NOT_CONNECTED, ATTR_VISIBLE, 1);
			SetCtrlAttribute(idleTabHandle, IDLE_PANEL_READY, ATTR_VISIBLE, 0);
			sprintfWithLock(1024,temp,"Connection to server failed, IP address %s, port %d", ipAddress, portNumber); 
			writeToLog(temp);
			TCPServerHandle = -1;
			return -1;
		} else {
			SetCtrlAttribute(idleTabHandle, IDLE_PANEL_NOT_CONNECTED, ATTR_VISIBLE, 0);
			SetCtrlAttribute(idleTabHandle, IDLE_PANEL_READY, ATTR_VISIBLE, 1);
			sprintfWithLock(1024,temp,"Connected to server IP address %s, port %d", ipAddress, portNumber); 
			writeToLog(temp);
		}
	}
	
	// Initialize communications with XRAY Daemon
	if (xrayDaemonHandle <0){
		retCode = ConnectToTCPServer((unsigned int*) &xrayDaemonHandle, XRAY_DAEMON_PORT_NUMBER, ipAddress, xrayDaemonTCPCallbackFunction, 0, timeOut=timeOutDefault);
		if (retCode!=0)	{
			sprintfWithLock(sizeof(temp),temp,"Connection to xray daemon failed, IP address %s, port %d", ipAddress, XRAY_DAEMON_PORT_NUMBER); 
			xrayDaemonHandle = -1;
			writeToLog(temp);
			return -1;
		} else {
			sprintfWithLock(sizeof(temp),temp,"Connected to xray daemon IP address %s, port %d", ipAddress, XRAY_DAEMON_PORT_NUMBER); 
			writeToLog(temp);
		}
	}
	
	return 0;
}

void communicationsTermination(){
	if (TCPServerHandle>=0) DisconnectFromTCPServer(TCPServerHandle);
	TCPServerHandle = -1;
	CmtDiscardLock(tcpLock);
	SetSleepPolicy(VAL_SLEEP_MORE);
}

int tcpCallbackFunction (unsigned handle, int xType, int errCode, void *callbackData){
	int bytesRead;
	char temp[128];
	
	// Not quite sure if I need to serialize this function; but can't hurt too much to do it ...
	traceToLog("[DEBUG] - communicationsManagement::tcpCallbackFunction  - Entered.");
	int lockCode = CmtGetLock(tcpLock);
	if (lockCode == 0) traceToLog("[DEBUG] - communicationsManagement::tcpCallbackFunction  - Got TCP lock");
	else  writeToLog("communicationsManagement::tcpCallbackFunction - ERROR, TCP lock not obtained");
	
	switch(xType){
		
		case TCP_DISCONNECT:
			SetCtrlAttribute(idleTabHandle, IDLE_PANEL_NOT_CONNECTED, ATTR_VISIBLE, 1);
			SetCtrlAttribute(idleTabHandle, IDLE_PANEL_READY, ATTR_VISIBLE, 0);
			writeToLog("Disconnect received from server");
			TCPServerHandle = -1;
			acquisitionRequested = 0;  // Cancel acquisition if any
			SetCtrlAttribute( idleTabHandle , IDLE_PANEL_reconTimer, ATTR_ENABLED, 1); // Schedule automatic reconnection
			break;
			
		case TCP_DATAREADY:
 			traceToLog("[DEBUG] - communicationsManagement::tcpCallbackFunction - TCP Data Ready.");
			
			struct PETBoxMessage *message;
			message = malloc( sizeof(*message) ); // Message will be freed by handle response
			if (message == NULL){
				writeToLog("communicationsManagement::tcpCallbackFunction - Error, no memory for message.  Closing communications.");
				communicationsTermination();	 // this takes care of the lock
				return -1;
			}
			
			// Get ONE buffer from TCP (data should be readily available)
			bytesRead = ClientTCPRead(TCPServerHandle, message, 4, timeOut=50);	 // Read only 4 first bytes (length, type and code)
			if (bytesRead>=0){
				sprintfWithLock(128,temp,"[DEBUG] communicationsManagement::tcpCallbackFunction - Reading length (%d bytes read); length= %d", bytesRead, message->length); 
				traceToLog(temp);

				bytesRead = ClientTCPRead(TCPServerHandle, &(message->startCmd), message->length-4, timeOut=500); // Read the rest of this message    
				if (bytesRead != message->length-4){
					sprintfWithLock(128,temp,"communicationsManagement::tcpCallbackFunction - ERROR reading rest of message; bytes read was %d, was supposed to read %d", bytesRead, message->length-4); 
					writeToLog(temp); 
				}
				
				// Not a direct call here because we 
				// have to get out of here quickly
				int retCode = PostDeferredCall(handleResponse, message);
				if (retCode<0){
					writeToLog("communicationsManagement::tcpCallbackFunction - Error in PostDeferredCall.");
				}	
			}
			else if (bytesRead != -kTCP_TimeOutErr){
				sprintfWithLock(128,temp,"communicationsManagement::tcpCallbackFunction - ERROR in tcp read: %d", bytesRead); 
				writeToLog(temp); 
			}
			
			break;
			
		default:
			writeToLog("communicationsManagement::tcpCallbackFunction - Error, Unknown TCP xType.");
			break;
	}
	
	lockCode = CmtReleaseLock(tcpLock);
	if (lockCode !=0) writeToLog("communicationsManagement::tcpCallbackFunction - ERROR TCP lock not released");
	traceToLog("[DEBUG] - communicationsManagement::tcpCallbackFunction  - Exited.");
	return 0;
}

void CVICALLBACK handleResponse (void *callbackData){
	struct PETBoxMessage *message = (struct PETBoxMessage*) callbackData;
	char   temp[1024];
	
	traceToLog("[DEBUG] - communicationsManagement::handleResponse  - Entered.");

	switch (message->messageType){
		
		case TYPE_NACK:
			writeToLog("communicationsManagement::handleResponse - Negative acknowledge received");
			break;
			
		case TYPE_ACK:
			switch(message->commandCode ){
				
				case COMMAND_START:
					// ACK for a previously issued start command.
					// The run manager is waiting for that ...
					startAckReceived = 1;
					writeToLog("communicationsManagement::handleResponse - Start acknowledge received, file:");
					writeToLog(message->startAck.lmfFileName);
					myStrcpy(serverStatus.lmfFileName, message->startAck.lmfFileName); 
					break;
					
				case COMMAND_INQUIRE_STATUS:
					// ACK to the INQUIRE_STATUS sent at startup.
					// update our copy of the server status
					inquireAckReceived=1;
					myMemcpy((char*)&serverStatus, (char*)&message->inqAck, (int) sizeof(message->inqAck) );
				    sprintfWithLock(1024,temp,"PETBox serial number is %d",serverStatus.serialNumber);
				    writeToLog(temp);
					break;
			}
			break;

		case TYPE_UNSOC:
			// Unsollicited status sent during acquisition.
			// simply update our copy of server status
			myMemcpy( (char*) &unsocStats, (char*) &message->unsoc, (int) sizeof(message->unsoc) );  
			break;
			
			
		default:
			sprintfWithLock(1024,temp,"ERROR - Unknown message type: %d",message->messageType);
			writeToLog(temp);
			break;
	}
	traceToLog("[DEBUG] - communicationsManagement::handleResponse  - Exited.");
	free(message);
}


int CVICALLBACK reconnectClick_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_RIGHT_CLICK:
			RunPopupMenu(otherMenuBarHandle, reconnectMenu, idleTabHandle, eventData1, eventData2, 0,0,0,0);
			break;
	}
	return 0;
}

void CVICALLBACK reconnect_CB (int menuBar, int menuItem, void *callbackData,
		int panel)
{
		communicationsInitialization();
}

int CVICALLBACK reconTimer_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if ( TCPServerHandle <0 || xrayDaemonHandle <0) {
				writeToLog("communicationsManagement::reconnect_CB : Trying to reconnect");
				communicationsInitialization();
			}
			else {
				writeToLog("communicationsManagement::reconnect_CB : Reconnection timer disabled.");
				SetCtrlAttribute( idleTabHandle , IDLE_PANEL_reconTimer, ATTR_ENABLED, 0);
			}
			break;
	}
	return 0;
}
