//*******************************************************************************
//*                                                                             *
//*  PETBox Acquisition and Management Graphical User Interface                 *
//*  XRAY DEAMON Software                                                       *
//*                                                                             *
//*  Copyright - 2011                                                           *
//*                                                                             *
//*  This code is the property of Sofie Bioscience                              *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Shang Shang and Richard                                         *
//*                                                                             *
//*  Created: May 2011                                                          *
//*                                                                             *
//*  Change log:   Changed 6008 layout on 08/08/11								*
//*				   Add chamber present info read in ResponsetoInquireStatus		*
//*					on 10/17/2011												*
//*				    Produce single uncropped raw xray image						*
//*               Update output side photo format on 12/27/2011					*
//*				  0921: updated x-ray source and side  photo format             *
//*               0925: updated x-ray source start up process record            *
//*******************************************************************************

/* Acquisition related prototypes*/

#include "windows.h"
#include <NIDAQmx.h>
#include <tcpsupp.h>
#include <mmsystem.h>  
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "PGRFlyCapture.h"
#include "Scilib21.h"
#include "SnapSDK.h"

#include "xray_main.h"
#include "xrayDaemon.h"
#include "GlobalDecls.h"
#include "XRAY_SERVER.h"


void serverTermination(void);
int serverInitialization(void); 
int CVICALLBACK ServerTCPCB(unsigned handle, int event, int error, void *callbackData);
void CVICALLBACK handleCommand (void *callbackData);
void makeUniqueXrayFileID(char* outFileName);
void makeUniqueSidePhotoFileID(char* outFileName);
int  CVICALLBACK acquisitionThread (void *functionData);
void CALLBACK AcqTimerCallBackProc( UINT timerid, UINT msg, DWORD user, DWORD dw1, DWORD dw2 );
void respondToInquireStatus(struct PETBoxMessage *buffer);
void sendUnsollicitedStatus(struct PETBoxMessage *buffer); 
void fly_ErrorMessage(FlyCaptureError error, char *errMessage); 
void acquisitionReset(void);


// Local variables   
static int  externalPCHandle;
static char externalPCName[200];
static char studyID[MAX_STUDY_LENGTH];
static char studyPathName[MAX_FILE_NAME_LENGTH];
static char tempString[2048];	   // local temporary work string to be used with sprintf, etc   

// acquisition statistics
static int timeLeft=0;   
	
// acquisition buffers and buffer index
char receiveBuf[256];  
static int servepanel; 

// acquisition timer
MMRESULT  timerID;
UINT     wTimerRes;
unsigned long acqPresetTime, acqStTime,acqCurTime,acqEndTime,sourceStTime, sourcePresentTime, sourceDuration; 

//X-ray server initialization -- Register as a server
int serverInitialization()
{ 
	int retCode;
	externalPCHandle = 0;
	
	//getLocalAddresses(); 
	retCode =  RegisterTCPServer (PORT_NUMBER, ServerTCPCB, 0);
	if (retCode < 0)
		{
			sprintf(tempString,"Error opening port # %d, retCode = %d", PORT_NUMBER, retCode);
			writeToLog(tempString);
		}
		
	else 
		{   
			SetCtrlVal (SERVEPANEL, SERVEPANEL_SERVERLED, 1) ;
			sprintf(tempString,"Successfully opened port # %d", PORT_NUMBER);
			writeToLog(tempString);
		}
		acquisitionInProgress = 0;
		return retCode;
} 

//X-ray server termination -- Unregister server 
void serverTermination()
{
	int retCode;
	
 	if (externalPCHandle) 
		DisconnectTCPClient(externalPCHandle);
	
	retCode = UnregisterTCPServer(PORT_NUMBER);
	if (retCode == 0) 
		{
			sprintf(tempString,"Port # %d closed", PORT_NUMBER);
			writeToLog(tempString);
		}
	else 
		{
			sprintf(tempString,"Error closing port # %d", PORT_NUMBER);
			writeToLog(tempString);
		}
}

int CVICALLBACK ServerTCPCB(unsigned handle, int event, int error, void *callbackData)

{
	struct PETBoxMessage *dataBuffer;
	int retCode;
	char temp[40];
	
	switch (event)
	{
		case TCP_CONNECT:
			if (externalPCHandle !=0)
				{
					writeToLog("Unexpected connection request received");
					DisconnectTCPClient (handle);
					return 0;
				}
			else 
				{
					externalPCHandle = handle;		
					GetTCPPeerAddr(externalPCHandle, externalPCName, sizeof(externalPCName) );
					sprintf(tempString,"Connection accepted from %s, handle value %d", externalPCName, externalPCHandle);
					writeToLog(tempString);
				}
			break;
			
			case TCP_DATAREADY:   
			dataBuffer = malloc(sizeof(*dataBuffer));
			retCode = ServerTCPRead(externalPCHandle, dataBuffer, sizeof(*dataBuffer), 0);
			if (retCode < 0) 
				{
					sprintf(tempString, "Error %d in TCPRead: %s", retCode, GetTCPSystemErrorString());
					writeToLog(tempString);
				}
			else
				{
				putToHex(temp, dataBuffer);
				sprintf(tempString, "Received command (%d bytes) - %s", retCode, temp);
				writeToLog(tempString);
				PostDeferredCall(handleCommand, dataBuffer);
				}
				
			break; 
				
			case TCP_DISCONNECT:
				
				retCode = DisconnectTCPClient(externalPCHandle);
				sprintf(tempString,"Disconnected from %s, errCode %d, retcode from disconnect %s",externalPCName, 
					error, GetTCPErrorString(retCode) );
				writeToLog(tempString);
					externalPCHandle=0;
					
			break;
	}
	return 0;
}	 


void CVICALLBACK handleCommand (void *callbackData)
{
	struct PETBoxMessage *buffer = (struct PETBoxMessage *) callbackData;
	char temp[256];
	int errcode;
	
	switch (buffer->commandCode)
	{
		case XDMN_COMMAND_START:
		writeToLog("Start command received!");
	
		strcpy(studyID, buffer->startCmd.studyID);
	    strcpy(studyPathName, buffer->startCmd.studyPathName);
	
      
		if(buffer->startCmd.startCommandRequest & XDMN_START_FLAG_XRAY)
			xrayRequested =1;
		if(buffer->startCmd.startCommandRequest & XDMN_START_FLAG_PHOTO) 
			sidePhotoRequested =1;

		// Re-initialize hardware when start command is received again after hardware failure is fixed
		// check 6008 status, reset it if needed
		/*if(DAQmxSelfTestDevice ("Dev1")!=0)
		{
			xraySourceRdy=0;	   // x-ray source ready indicator
			xraysourceFail=1;
			writeToLog("6008 status error, reseting...");
		}
		*/
		
		xraySourceRdy=1;
		SetCtrlVal(SERVEPANEL, SERVEPANEL_XRAY_SRC_RDY_LED, xraySourceRdy);
		SetCtrlVal(SERVEPANEL, SERVEPANEL_SOURCE_TOGGLE, xraySourceRdy);
		//xraysourceFail=0; 
		detectorRdy=1;
		SetCtrlVal(SERVEPANEL, SERVEPANEL_XRAY_DETECT_RDY_LED, detectorRdy);
		SetCtrlVal(SERVEPANEL, SERVEPANEL_DETECTOR_TOGGLE, detectorRdy);
		//detectorFail=0;
		sideCameraRdy=1;
		SetCtrlVal(SERVEPANEL, SERVEPANEL_XRAY_SIDECAM_RDY_LED, sideCameraRdy);
		SetCtrlVal(SERVEPANEL, SERVEPANEL_SIDECAM_TOGGLE, sideCameraRdy);
		//sideCameraFail=0;
		

		if (acquisitionInProgress==0 && xraySourceRdy==1 && detectorRdy==1 && sideCameraRdy==1 && (xrayRequested ||sidePhotoRequested))
			{
				acquisitionInProgress=1;
				//reset error codes and success indicators when a new acquisition is started
				
				otherFail=0;
		
				XrayAcqSuccess = 0; 
				sidePhotoAcqSuccess=0;
				CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, acquisitionThread, buffer, &acquisitionThreadFunctionID);
				buffer->messageType = XDMN_MSG_TYPE_ACK;
				buffer->length = (unsigned char*)(&buffer->startAck) - (unsigned char*)buffer;
				
				writeToLog("server::acquisitionThread - Sending ACK");
				int bytesWritten = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				if (bytesWritten != buffer->length)
					{
						writeToLog("server::acquisitionThread - Error sending acknowledge: bad byte count");
					}
			
			} 
		else
			{
				buffer->messageType = XDMN_MSG_TYPE_NACK;
				buffer->length = (char*)(&buffer->startAck.reasonCode+1) - (char*)buffer;
				buffer->startAck.reasonCode = 0;
				if (acquisitionInProgress)
				buffer->startAck.reasonCode = XDMN_NACK_START_ACQ_INPROGRESS;
				else if (!xraySourceRdy)
				buffer->startAck.reasonCode += XDMN_NACK_START_XRAY_FAILURE;
				else if (!detectorRdy)
				buffer->startAck.reasonCode += XDMN_NACK_START_DETECTOR_FAILURE ;
				else if (!sideCameraRdy)
				buffer->startAck.reasonCode +=  XDMN_NACK_START_SIDE_CAM_FAILURE;
				else if (!xrayRequested && !sidePhotoRequested)
				buffer->startAck.reasonCode +=  XDMN_NACK_START_OTHER_FAILURE;		
					
				errcode = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				sprintf(temp,"NACK sent to start command; acquisition in progress = %d; Safety interlock ready = %d; Filament ready = %d; XRAY source ready = %d; XRAY detector ready = %d; Camera ready = %d;xrayRequested ||sidePhotoRequested = %d", acquisitionInProgress, safetyStatus, filamentRdy, xraySourceRdy, detectorRdy, sideCameraRdy,xrayRequested ||sidePhotoRequested);
				writeToLog(temp);
				free(buffer);
			}
		break;
		
		case XDMN_COMMAND_STOP:
		writeToLog("Stop command received!");  
		
		if (acquisitionInProgress)
			{
				acquisitionInProgress = 0;
				buffer->messageType = XDMN_MSG_TYPE_ACK;
				buffer->length = (char*)(&buffer->stopAck) - (char*)buffer;
				ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				writeToLog("ACK sent to stop command");
				while(acquisitionThreadRunning)Delay(0.010);
				CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,acquisitionThreadFunctionID);
				
			} 
		else
			{
				buffer->stopAck.reasonCode = 0;
				buffer->messageType = XDMN_MSG_TYPE_NACK;
				buffer->stopAck.reasonCode = XDMN_NACK_STOP_NO_ACQ;
				buffer->length = (char*)(&buffer->stopAck.reasonCode+1) - (char*)buffer;
				ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				writeToLog("NACK sent to stop command");
			}
		free(buffer);
		
		break;
		
		case XDMN_COMMAND_INQUIRE_STATUS:
		//writeToLog("Inquire command received!"); 
		
		
		//RTA
		char chex[128];
		putToHex(chex, buffer);
		sprintf(temp, "[DEBUG*] handleCommand - Inquire command received - %s", chex);
		writeToLog(temp);
		
		
		
		//buffer->messageType = XDMN_MSG_TYPE_ACK;
		//		buffer->length = (char*)&buffer->inqAck.cam_brightness - (char*)buffer+ 8 ;		   
		//		ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
		respondToInquireStatus(buffer);
		free(buffer);
		break;
		
	}
}

void makeUniqueXrayFileID(char* outFileName)
{
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

		sprintf(outFileName,"%s\\%d.%d.%d.%s.%s%s%s.%s%s%s.raw",
			xrayDefaultFolder, 
			xraySrcSerNum,
			xrayDetSerNum,
			++globalSequenceNumber,
			studyID,
			theDate.mm, theDate.dd, theDate.yyyy,
			theTime.hh, theTime.mm, theTime.ss
			 );

		RegSetValueEx(xrayRegKey, "GlobalSequenceNumber",  0, REG_DWORD,  (LPBYTE)&globalSequenceNumber,   4);
	
}


void makeUniqueSidePhotoFileID(char* outFileName)
{
	
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
			 
		sprintf(outFileName,"%s\\%d.%d.%s.%s%s%s.%s%s%s.pic",
			photoDefaultFolder, 
			sideCameraSerNum,
			globalSequenceNumber,
			studyID,
			theDate.mm, theDate.dd, theDate.yyyy,
			theTime.hh, theTime.mm, theTime.ss
			 );	   
	
}

/*
	ACQUISITION THREAD
	
	The thread is started upon reception of a start command.  
*/
int  CVICALLBACK acquisitionThread (void *functionData)
{
	
		struct PETBoxMessage *buffer = (struct PETBoxMessage*) functionData;
		DWORD	  dwUser;
		unsigned short* pImgBuf; // SDK image buffer
		int *Dim;   //output image dimension buffer
		unsigned long nBytesWritten;
		XrayAcqSuccess = 0;
		sidePhotoAcqSuccess=0;
		
		unsigned short *pIntTime = malloc(sizeof( unsigned short));
	
		// set the sleep policy
		SetSleepPolicy (VAL_SLEEP_SOME);
		writeToLog("acquisitionThread()");
		// Extract required info from buffer 
		timeLeft = xray_exposure;
		//sprintf(tempString,"No.1:timeLeft=%d.", timeLeft);
		//writeToLog(tempString);
		
		if (xrayRequested)   // if Xray is requested, start x-ray acquisition
		{
		// Prepare unique id for file name and acknowledge 
		makeUniqueXrayFileID(pathToLocalXray);
	
		// Set up raw image buffer
		nBufSize = MAX_IMAGE_LENGTH;
		pImgBuf = (unsigned short*)malloc(nBufSize);
		
		if ( pImgBuf == NULL)
			{
				writeToLog("Insufficient memory available for raw image.");
				otherFail=1;
				sendUnsollicitedStatus(buffer); 
				acquisitionReset();
				return -1;
			}

		// start windows MM timer
		TIMECAPS tc;

		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
		{
			sprintf(tempString,"Error: Can't start Windows MM timer\nAcquisition aborted.");
			writeToLog(tempString);
			otherFail=1; 
			sendUnsollicitedStatus(buffer);
			acquisitionReset();
			return -1;
		}

		// set timer period
		wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
		timeBeginPeriod(wTimerRes); 
		acquisitionThreadRunning=1;
	
		// Send interlock signal out
		safety_Enable=1;
		
		Delay(0.05);
		
		//check safety interlocks
		
		/******** for test in UCLA only*****/
		safetyStatus = 1;
		/******** for test in UCLA only*****/
		
		if(!safetyStatus)
		{
				sprintf(tempString,"Error: safety interlock not ready.");
				writeToLog(tempString);
			    otherFail=1; 
				sendUnsollicitedStatus(buffer); 
				acquisitionReset(); 
				return -1;
			}
		
	//	Delay(0.5);  //********delay after safety interlock closed*****//
		
		//send HV_Enable
		HV_Enable = 1;
	
		SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_ON, 1);
		sourceStTime = timeGetTime();
		filamentRdy=0;
		sourceDuration=0;

		writeToLog("Starting Acquisition Loop");
		
		// main loop during acquisition
		/*while (acquisitionInProgress )
			{
				//keep reading current value and HV value
				Current_Data = 1.0;
				HV_Data = 5.0 ;
				
				//real time HV and current monitor
				SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_CURRENT, Current_Data);
				SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_HV, HV_Data);
						
				Delay(0.002);
				
			 	if(!timeLeft)
			 		{
						
						//stop X-ray
						HV_Enable = 0;
						SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_ON, 0);
						// load image to buffer when available
						
						break; 
					}
			 
			 
			 
			
			
			}*/
	   
	 	// Skip Acq Loop
		Delay(1.0);
		
		 // stop timer
		acqEndTime=timeGetTime(); 
		if (timerID)
			timeKillEvent(timerID);

		//end timer period
		timeEndPeriod(wTimerRes);
	
		int result = 0;
		//Load image from buffer, do image corrections, save raw images to files 
		if (acquisitionInProgress)
		{   
			// Check if I/O error occurred 
			MakeDir(xrayDefaultFolder);
			
			result = CopyFile("C:\\test\\sample.raw", pathToLocalXray);
			XrayAcqSuccess = 1; 
			sprintf(tempString,"Acquisition raw data file created as: %s. result = %d", pathToLocalXray, result);
			writeToLog(tempString);
			XrayAcqSuccess = 1;  
			
		
			
		}   
	
	
			if (pImgBuf)
			free(pImgBuf);  
		}
		int result1 = 0;
		//acquire side photo
		if (acquisitionInProgress && sidePhotoRequested) 
		{
			makeUniqueSidePhotoFileID(pathToLocalPhoto);
			
			//save cropped photo	
			result1 = CopyFile("C:\\test\\sample.pic", pathToLocalPhoto);
			sprintf(tempString,"Side photo data file created as: %s. result = %d", pathToLocalPhoto, result1);
			writeToLog(tempString);
			sidePhotoAcqSuccess=1;
			
		}
		sendUnsollicitedStatus(buffer); 
		writeToLog("Acquisition success.");
		
		acquisitionReset();

		free(buffer);
	  

		return 0;

}  

/*Reset acquisition parameters*/
void acquisitionReset()
{
	//make sure x-ray is stopped 
		HV_Enable = 0;
		SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_ON, 0);
	//stop interlock signal out
		safety_Enable=0;
	//stop timer
		if (timerID)
		timeKillEvent(timerID);
		acquisitionThreadRunning 	= 0;
		acquisitionInProgress    	= 0;
		timeLeft                 	= 0;
	//	elapsedTime					= 0;
		Current_Data				= 0;
		HV_Data						= 0;
		acqPresetTime				= 0;
		acqStTime					= 0;
		acqCurTime					= 0;
		acqEndTime					= 0;
		sourceStTime				= 0;
		sourcePresentTime			= 0;
		sourceDuration				= 0;
		(*studyID)               	= 0;
		(*studyPathName)         	= 0;
		(*pathToLocalXray)       	= 0;
		(*pathToLocalPhoto)      	= 0;
		SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_CURRENT, 0.0);
		SetCtrlVal (SERVEPANEL, SERVEPANEL_XRAY_HV, 0.0);

} 

void CALLBACK AcqTimerCallBackProc( UINT timerid, UINT msg, DWORD user, DWORD dw1, DWORD dw2 )
{
		unsigned long elapsedTime=0;

		// keep reading safety interlock status
		safetyStatus = 1;
		filamentRdy = 1;
	
		//update remaining time
		acqCurTime=timeGetTime();
		elapsedTime=acqCurTime-acqStTime;
		timeLeft=acqPresetTime-elapsedTime;
		sprintf(tempString,"AcqTimerCallback: timeleft=%d    elapsedtime=%d.",timeLeft, elapsedTime);
		writeToLog(tempString);
		if (timeLeft<=0)
			{
				timeLeft=0;
				return;
			}

		return ;
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
void respondToInquireStatus(struct PETBoxMessage *buffer)
{
		int retCode;
		char temp[40];
		writeToLog("Sending ACK to inquire command");  
	
		if (externalPCHandle)
			{
				
				// check 6008 status, reset it if needed
				
				
				// Send interlock signal out
				safety_Enable=1;
				Delay(0.05);
				//read safety interlocks (chamber) status
				safetyStatus =1;
				// ---------------
				
				//
				buffer->messageType = XDMN_MSG_TYPE_ACK;
				buffer->inqAck.status      = 0;
				if (acquisitionThreadRunning) 
					buffer->inqAck.status +=  XDMN_INQ_STAT_ACQ_IN_PROGRESS;
				if (HV_Enable)
					buffer->inqAck.status +=  XDMN_INQ_STAT_XRAY_ON;
				if ( safetyStatus)
					buffer->inqAck.status += XDMN_INQ_STAT_CHAMBER_PRESENT;
		
				buffer->inqAck.error = 0;
				if (!xraySourceRdy)
					buffer->inqAck.error += XDMN_INQ_STAT_ERR_XSRC_FAIL;
				if (!detectorRdy)
					buffer->inqAck.error += XDMN_INQ_STAT_ERR_XDET_FAIL;
				if (!sideCameraRdy)
					buffer->inqAck.error += XDMN_INQ_STAT_ERR_CAM_FAIL;
				if (!safetyStatus)
					buffer->inqAck.error += XDMN_INQ_STAT_ERR_INTERLOCK_OPEN;
				if (otherFail)
					buffer->inqAck.error += XDMN_INQ_STAT_ERR_OTHER_FAIL;
		
				strcpy(buffer->inqAck.studyID,       studyID      );
				strcpy(buffer->inqAck.studyPathName, studyPathName);
				strcpy(buffer->inqAck.pathToLocalXray, pathToLocalXray);
				strcpy(buffer->inqAck.pathToLocalPhoto, pathToLocalPhoto);  
				
				strcpy(buffer->inqAck.xraySrcSerNum, xraySrcSerNum);
				strcpy(buffer->inqAck.xrayDetSerNum, xrayDetSerNum);
				buffer->inqAck.xray_voltage  = xray_voltage;
				buffer->inqAck.xray_current = xray_current;
				buffer->inqAck.xray_exposure = xray_exposure;
				buffer->inqAck.xray_current_read = 25 ;
				buffer->inqAck.xray_voltage_read = 44 ;  
				buffer->inqAck.safety_status_read = safetyStatus; 
		
				strcpy(buffer->inqAck.camSerNum, sideCameraSerNum);
				buffer->inqAck.cam_exposure = side_camera_exposure;
				buffer->inqAck.cam_gain = side_camera_gain;
				buffer->inqAck.cam_brightness = side_camera_brightness;
				
				//buffer->length = (char*)(&buffer->inqAck.cam_brightness+sizeof(buffer->inqAck.cam_brightness)) - (char*)buffer;
				buffer->length = sizeof(*buffer);
				retCode = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				if (retCode < 0) {
					sprintf(tempString, "Error %d in TCPWrite: %s", retCode, GetTCPSystemErrorString());
					writeToLog(tempString);
				}
				putToHex(temp, buffer);
				sprintf(tempString, "Sending response (%d bytes) to inquire status - %s", buffer->length, temp);
				writeToLog(tempString);	
				
				// RTA
				// Send interlock signal out
				safety_Enable=0;
				// ---------------
				
				
				
			}
}



int CVICALLBACK loopTest (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		
		case EVENT_LEFT_CLICK:
				safety_Enable=1;
				Delay(0.05);
				//read safety interlocks (chamber) status
				 SetCtrlVal(panelHandle, SERVEPANEL_loopClosed , 1);
			
			break;
			
			
		case EVENT_LEFT_CLICK_UP:
				safety_Enable=0;
				SetCtrlVal(panelHandle, SERVEPANEL_loopOpen ,   0);
				SetCtrlVal(panelHandle, SERVEPANEL_loopClosed , 0);
			break;
	}
	return 0;
}




void sendUnsollicitedStatus(struct PETBoxMessage *buffer)
{
		int retCode;
		char temp[40];
	
		if (externalPCHandle)
			{
				buffer->messageType = XDMN_MSG_TYPE_UNSOC;
				if ( XrayAcqSuccess && sidePhotoAcqSuccess)
					buffer->unsoc.completionCode      =  XDMN_UNSOC_COMP_SUCCESS;
				else
					buffer->unsoc.completionCode  = XDMN_UNSOC_COMP_FAILURE;
		
				buffer->unsoc.error = 0;
		
				if (!xraySourceRdy)
					buffer->unsoc.error += XDMN_INQ_STAT_ERR_XSRC_FAIL;
				if (!detectorRdy)
					buffer->unsoc.error += XDMN_INQ_STAT_ERR_XDET_FAIL;
				if (!sideCameraRdy)
					buffer->unsoc.error += XDMN_INQ_STAT_ERR_CAM_FAIL;
				if (!safetyStatus)
					buffer->unsoc.error += XDMN_INQ_STAT_ERR_INTERLOCK_OPEN;
				if (otherFail)
					buffer->unsoc.error += XDMN_INQ_STAT_ERR_OTHER_FAIL;
		
		
				strcpy(buffer->unsoc.studyID,       studyID      );
				strcpy(buffer->unsoc.studyPathName, studyPathName);
				if(XrayAcqSuccess)
					{
						strcpy(buffer->unsoc.pathToLocalXray, pathToLocalXray);
					}
				else
					{
						strcpy(buffer->unsoc.pathToLocalXray, "N/A");
					}
		
				if(sidePhotoAcqSuccess)
					strcpy(buffer->unsoc.pathToLocalPhoto, pathToLocalPhoto);
				else
					strcpy(buffer->unsoc.pathToLocalPhoto, "Not available");   	
			 
				buffer->length = (char*)&buffer->unsoc - (char*)buffer + sizeof(buffer->unsoc);
		
				retCode = ServerTCPWrite(externalPCHandle, buffer, buffer->length, 2000);
				if (retCode < 0) 
					{
						sprintf(tempString, "Error %d in TCPWrite: %s", retCode, GetTCPSystemErrorString());
						writeToLog(tempString);
					}
				putToHex(temp, buffer);
				sprintf(tempString, "Sending response (%d bytes) to unsollicited status - %s", buffer->length, temp);
				writeToLog(tempString);
			}
}



void fly_ErrorMessage(FlyCaptureError error, char *errMessage)
{
		return;
}

