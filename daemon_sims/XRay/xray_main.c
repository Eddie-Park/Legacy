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
//*  Change log:  Changed 6008 layout on 08/08/11                               *
//*				   Add chamber present info read in ResponsetoInquireStatus		*
//*					on 10/17/2011												*
//*				  Produce single uncropped raw xray image						*
//*               Update output side photo format on 12/27/2011                 * 
//*                                                                             *
//*******************************************************************************


#include "windows.h"
#include <NIDAQmx.h>
#include <tcpsupp.h>
#include <mmsystem.h>  
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "PGRFlyCapture.h"
#include "SnapSDK.h"
#include "Scilib21.h"

#include "xray_main.h"
#include "xrayDaemon.h"
#include "GlobalDefs.h"
#include "XRAY_SERVER.h" 
				
#define XRAY_REG_KEY       "SOFTWARE\\Sofie Inc.\\XRAYServer\\1.0"
#define XRAY_ACQ_REG_KEY   "SOFTWARE\\Sofie Inc.\\XRAYServer\\1.0\\Acquisition" 


static int mainInitialization(void);
static int mainTermination(void);
int hardwareInitialization(void);
int hardwareTermination(void);
int xraySourceInitialization(void);
int detectorInitialization(void);
int sideCamInitialization(void);
int xraySourceTermination(void);
int detectorTermination(void);
int sideCamTermination(void);
void offsetImageCorrection(void);
void gainImageCorrection(void);

static char tempString[10240];	   // local temporary work string to be used with sprintf, etc 

//Main function
int main (int argc, char * argv[])
{		
		mainInitialization();
		serverInitialization();
		hardwareInitialization();
		DisplayPanel (panelHandle);
		
		char temp[128];
		sprintf(temp, "XRAY Acquisition 1.0 - build %s", _TARGET_FILE_VERSION_);
		SetCtrlVal (panelHandle, SERVEPANEL_buildID, temp);
		RunUserInterface();
		DiscardPanel (panelHandle);

		hardwareTermination();
		serverTermination();
		mainTermination();
	
		return 0;
}


//	Main initialization

static int mainInitialization()
{
		char appDataFolder[MAX_FILE_NAME_LENGTH];
		char logDataFileName[MAX_FILE_NAME_LENGTH];
		DWORD  disposition;
		int  logFileNumber=0;
		DWORD  size;
		unsigned long pathtype=REG_SZ;
		int errcode;

	 	if ((panelHandle=LoadPanel (0, "XRAY_SERVER.uir", SERVEPANEL))   <= 0) return -1;
	
		// Try to create the registry key

		RegCreateKeyEx(
				HKEY_LOCAL_MACHINE,
				XRAY_REG_KEY, 
				0, 
				NULL,
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				NULL,
				&xrayRegKey,
				&disposition);								  
	
		// Registry key already exists, then read it
		if (disposition==2) 
			{
				size = 12; 
				errcode = RegQueryValueEx(xrayRegKey, "GlobalSequenceNumber",  NULL, NULL, (LPBYTE)&globalSequenceNumber, &size);
				size = 12; 
				errcode=	RegQueryValueEx(xrayRegKey, "LogFileNumber",         NULL, NULL, (LPBYTE)&logFileNumber,        &size);
				size=MAX_FILE_NAME_LENGTH;
				RegQueryValueEx(xrayRegKey, "SystemFiles",           NULL, NULL,   (BYTE*)systemFilesPath,        &size);
		
				size=MAX_FILE_NAME_LENGTH;
				RegQueryValueEx(xrayRegKey, "OffsetImage",                   NULL, NULL,   (BYTE*)rawOffsetImageFilePath,            &size);
		
				size=MAX_FILE_NAME_LENGTH;
				RegQueryValueEx(xrayRegKey, "GainImage",             NULL, NULL,   (BYTE*)rawGainImageFilePath,      &size);
		
				size=MAX_FILE_NAME_LENGTH;
				RegQueryValueEx(xrayRegKey, "PixelMap",          NULL, NULL,   (BYTE*)detectorPixelMap,  &size);
			}
		else
			{
		
				globalSequenceNumber=0;
				RegSetValueEx(xrayRegKey, "GlobalSequenceNumber",  0, REG_DWORD,  (LPBYTE)&globalSequenceNumber,   4);
				RegSetValueEx(xrayRegKey, "SystemFiles",           0, REG_SZ,     (BYTE*)systemFilesPath,        1+strlen(systemFilesPath)      );
				RegSetValueEx(xrayRegKey, "OffsetImage",                   0, REG_SZ,     (BYTE*)rawOffsetImageFilePath,     1+strlen(rawOffsetImageFilePath)          );
				RegSetValueEx(xrayRegKey, "GainImage",             0, REG_SZ,     (BYTE*)rawGainImageFilePath,      1+strlen(rawGainImageFilePath)    );
				RegSetValueEx(xrayRegKey, "PixelMap",          0, REG_SZ,     (BYTE*)detectorPixelMap,  1+strlen(detectorPixelMap));
			}
		// Create xray server if necessary
		sprintf(appDataFolder,"%s\\XRAYServer",getenv("APPDATA"));		  //C:\Users\sofie\AppData\Roaming\PETBoxServer 
		MakeDir(appDataFolder);

		// Update the registry key
	 	sprintf(logDataFileName,"%s\\XrayServerLogFile_%d.txt",appDataFolder, logFileNumber++ );
		RegSetValueEx(xrayRegKey, "LogFileNumber",  0, REG_DWORD,  (BYTE*)&logFileNumber,   4);
	
		// ... and open log file
		logFile=fopen(logDataFileName,"w");
		if (logFile)
			{
			sprintf(tempString, "Genisys(4) XRAY Acquisition - Started build  %s", _TARGET_FILE_VERSION_);
			writeToLog(tempString);
			} 

		// Try to create the acquistion registry key
		// The acqusition registry key is a sub key of XRAY_REG_KEY
		// and contains the parameters for acquisition.
		RegCreateKeyEx(
				HKEY_LOCAL_MACHINE,
				XRAY_ACQ_REG_KEY, 
				0, 
				NULL,
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				NULL,
				&xrayAcqRegKey,
				&disposition);								  
	
		// Registry key already exists, then read it
		if (disposition==2) 
			{
				//detector parameters
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_current",  NULL, NULL, (LPBYTE)&xray_current,  &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_voltage",  NULL, NULL, (LPBYTE)&xray_voltage,  &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_exposure", NULL, NULL, (LPBYTE)&xray_exposure, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_target_current", NULL, NULL, (LPBYTE)&xray_target_current, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_max_current", NULL, NULL, (LPBYTE)&xray_max_current, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_target_hv", NULL, NULL, (LPBYTE)&xray_target_hv, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "xray_max_hv", NULL, NULL, (LPBYTE)&xray_max_hv, &size);

				size=MAX_FILE_NAME_LENGTH;
				RegQueryValueEx(xrayAcqRegKey, "xrayDefaultFolder",  NULL, &pathtype, (LPBYTE)&xrayDefaultFolder[0],  &size);
				size=MAX_FILE_NAME_LENGTH; 
				RegQueryValueEx(xrayAcqRegKey, "xraySrcSerNum",  NULL, &pathtype, (LPBYTE)&xraySrcSerNum[0],  &size);
				size=MAX_FILE_NAME_LENGTH; 
				RegQueryValueEx(xrayAcqRegKey, "xrayDetSerNum",  NULL, &pathtype, (LPBYTE)&xrayDetSerNum[0],  &size);

				//side camera parameters
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_frameRate", NULL, NULL, (LPBYTE)&side_camera_frameRate, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_shutter", NULL, NULL, (LPBYTE)&side_camera_shutter, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_autoShutter", NULL, NULL, (LPBYTE)&side_camera_autoShutter, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_brightness", NULL, NULL, (LPBYTE)&side_camera_brightness, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_autoBrightness", NULL, NULL, (LPBYTE)&side_camera_autoBrightness, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_gain", NULL, NULL, (LPBYTE)&side_camera_gain, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_autoGain", NULL, NULL, (LPBYTE)&side_camera_autoGain, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_exposure", NULL, NULL, (LPBYTE)&side_camera_exposure, &size);
				size = 12;
				RegQueryValueEx(xrayAcqRegKey, "side_camera_autoExposure", NULL, NULL, (LPBYTE)&side_camera_autoExposure, &size);
		
				size=MAX_FILE_NAME_LENGTH;
				RegQueryValueEx(xrayAcqRegKey, "sidePhotoDefaultFolder",  NULL, &pathtype, (LPBYTE)&photoDefaultFolder[0],  &size);
				size=MAX_FILE_NAME_LENGTH; 
				RegQueryValueEx(xrayAcqRegKey, "sideCameraSerNum",  NULL, &pathtype, (LPBYTE)&sideCameraSerNum[0],  &size); 
			}
		else // set parameters to default values
			{
				xray_exposure=PRESET_EXPOSURE_TIME_VALUE;
				xray_current= PRESET_CURRENT_VALUE;
				xray_voltage= PRESET_HV_VALUE;
				xray_target_current=TARGET_CURRENT_VALUE;
				xray_max_current= MAX_CURRENT_VALUE;
				xray_target_hv= TARGET_HV_VALUE;
				xray_max_hv= MAX_HV_VALUE;
		
			 	sprintf(xrayDefaultFolder,"%s",DEFAULT_XRAY_DATA_PATH);
				sprintf(xraySrcSerNum, "%s", DEFAULT_XRAY_SOURCE_SN);
				sprintf(xrayDetSerNum, "%s", DEFAULT_XRAY_DETECTOR_SN);

				side_camera_frameRate = FRAME_RATE;
				side_camera_shutter = SHUTTER;
				side_camera_autoShutter = AUTO_SHUTTER;;
				side_camera_brightness = BRIGHTNESS;
				side_camera_autoBrightness= AUTO_BRIGHTNESS;
				side_camera_gain= GAIN;
			    side_camera_autoGain =AUTO_GAIN;
				side_camera_exposure = EXPOSURE;
				side_camera_autoExposure = AUTO_EXPOSURE;
		
				sprintf(photoDefaultFolder,"%s",DEFAULT_PHOTO_DATA_PATH);
				sprintf(sideCameraSerNum, "%s", DEFAULT_SIDE_CAMERA_SN);
		
			}
	
		// Update the registry key	   
		RegSetValueEx(xrayAcqRegKey, "xray_current",  0, REG_DWORD,  (BYTE*)&xray_current,   4);
		RegSetValueEx(xrayAcqRegKey, "xray_voltage",  0, REG_DWORD,  (BYTE*)&xray_voltage,   4);
		RegSetValueEx(xrayAcqRegKey, "xray_exposure",  0, REG_DWORD,  (BYTE*)&xray_exposure,   4);
		RegSetValueEx(xrayAcqRegKey, "xray_target_current",  0, REG_DWORD,  (BYTE*)&xray_target_current,   4);
		RegSetValueEx(xrayAcqRegKey, "xray_max_current",  0, REG_DWORD,  (BYTE*)&xray_max_current,   4);
		RegSetValueEx(xrayAcqRegKey, "xray_target_hv",  0, REG_DWORD,  (BYTE*)&xray_target_hv,   4);
		RegSetValueEx(xrayAcqRegKey, "xray_max_hv",  0, REG_DWORD,  (BYTE*)&xray_max_hv,   4);
	
		RegSetValueEx(xrayAcqRegKey, "xrayDefaultFolder",  0, REG_SZ,  (BYTE*)&xrayDefaultFolder[0],   strlen(xrayDefaultFolder)+1);
		RegSetValueEx(xrayAcqRegKey, "xraySrcSerNum",  0, REG_SZ,  (BYTE*)&xraySrcSerNum[0],   strlen(xraySrcSerNum)+1); 	
	 	RegSetValueEx(xrayAcqRegKey, "xrayDetSerNum",  0, REG_SZ,  (BYTE*)&xrayDetSerNum[0],   strlen(xrayDetSerNum)+1); 

		RegSetValueEx(xrayAcqRegKey, "side_camera_framerate",  0, REG_DWORD,  (BYTE*)&side_camera_frameRate,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_shutter",  0, REG_DWORD,  (BYTE*)&side_camera_shutter,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_autoShutter",  0, REG_DWORD,  (BYTE*)&side_camera_autoShutter,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_brightness",  0, REG_DWORD,  (BYTE*)&side_camera_brightness,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_autoBrightness",  0, REG_DWORD,  (BYTE*)&side_camera_autoBrightness,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_gain",  0, REG_DWORD,  (BYTE*)&side_camera_gain,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_autoGain",  0, REG_DWORD,  (BYTE*)&side_camera_autoGain,   4);
		RegSetValueEx(xrayAcqRegKey, "side_camera_exposure",  0, REG_DWORD,  (BYTE*)&side_camera_exposure,   4); 
		RegSetValueEx(xrayAcqRegKey, "side_camera_autoExposure",  0, REG_DWORD,  (BYTE*)&side_camera_autoExposure,   4); 
	
		RegSetValueEx(xrayAcqRegKey, "sidePhotoDefaultFolder",  0, REG_SZ,  (BYTE*)&photoDefaultFolder[0],   strlen(photoDefaultFolder)+1);
		RegSetValueEx(xrayAcqRegKey, "sideCameraSerNum",  0, REG_SZ,  (BYTE*)&sideCameraSerNum[0],   strlen(sideCameraSerNum)+1); 
	
		MakeDir(xrayDefaultFolder);
		MakeDir(photoDefaultFolder);
	
		pinhole_Xray_Para_Initial();
	    pinhole_Cam_Para_Initial();
		
		//offset image initializtion
		sprintf(tempString, "%s%s", systemFilesPath, correctedOffsetImageFilePath);
		HANDLE oFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		
		if(oFile == INVALID_HANDLE_VALUE)
		{
			writeToLog("Creating corrected offset image...");
			offsetImageCorrection();
		}
		else
			CloseHandle(oFile);
		
		//gain image initialization
		sprintf(tempString, "%s%s", systemFilesPath, correctedGainImageFilePath);
		HANDLE gFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(gFile==INVALID_HANDLE_VALUE)
		{
			writeToLog("Creating corrected gain image...");
			gainImageCorrection();
		}
		else
			CloseHandle(gFile);
		
		SetCtrlVal(panelHandle, SERVEPANEL_loopOpen ,   0);
		SetCtrlVal(panelHandle, SERVEPANEL_loopClosed , 0);
	
		return 0;
}
	

  /*
	Main Termination
*/
static int mainTermination()
{
		writeToLog("XRAY Server Terminated Normally");
		if (logFile) 
			fclose(logFile);
		free(cord_xray);
		free(cord_cam);
		return 0;
}


/*
	Write to log function
	
	writes "message" to the log file.  It prepends a time stamp and
	the buffer is written immediattely
*/
void writeToLog(char* message)
{
		time_t currentTime;
		char localTimeString[200];
		int  length;

		if (logFile)
			{
				time(&currentTime);
				strcpy(localTimeString, ctime(&currentTime) );
				length = strlen(localTimeString);
				*(localTimeString+length-1) = 0;
	
				fprintf(logFile, "%s - %s\n", localTimeString, message ); 
				fflush(logFile);
			}
}


int CVICALLBACK ServerPanelCallback (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
		switch (event)
		{
			case EVENT_GOT_FOCUS:

				break;
			case EVENT_LOST_FOCUS:

				break;
			case EVENT_CLOSE:
				QuitUserInterface (0);
				break;
		}
		return 0;
}

int  CVICALLBACK CB_toggleDetectorReady(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK_UP)
	{
		GetCtrlVal(panel, control, &detectorRdy);
		SetCtrlVal(panel, SERVEPANEL_XRAY_DETECT_RDY_LED, detectorRdy);
		SetCtrlVal(panel, SERVEPANEL_DETECTOR_TOGGLE, detectorRdy);
	}
	return 0;
}
int  CVICALLBACK CB_toggleSideCamReady(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK_UP)
	{
		GetCtrlVal(panel, control, &sideCameraRdy);
		SetCtrlVal(panel, SERVEPANEL_XRAY_SIDECAM_RDY_LED, sideCameraRdy);
		SetCtrlVal(panel, SERVEPANEL_SIDECAM_TOGGLE, sideCameraRdy);
	}
	return 0;
}
int  CVICALLBACK CB_toggleSourceReady(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK_UP)
	{
		GetCtrlVal(panel, control, &xraySourceRdy);
		SetCtrlVal(panel, SERVEPANEL_XRAY_SRC_RDY_LED, xraySourceRdy);
		SetCtrlVal(panel, SERVEPANEL_SOURCE_TOGGLE, xraySourceRdy);
	}
	return 0;
}

// Transfer to HEX
void putToHex(char* out,void* in)
{
		int i,j;
		(*out)=0;
		for (j=0; j<16; j+=4)
			for (i=j; i<j+4; i++)
				sprintf(out+2*i, "%02X", ((unsigned char*)in)[i]); 
}

// Acquisition hardware initialization
int hardwareInitialization()
{
	
	if(xraySourceInitialization())
	 
	{
		xraySourceRdy=1;	   // x-ray source ready indicator
		SetCtrlVal(SERVEPANEL, SERVEPANEL_XRAY_SRC_RDY_LED, xraySourceRdy);
		SetCtrlVal(SERVEPANEL, SERVEPANEL_SOURCE_TOGGLE, xraySourceRdy);
		//sxraysourceFail=0;
	}

	if(detectorInitialization())  
	{
		detectorRdy = 1;		  // detector ready indicator
		SetCtrlVal(SERVEPANEL, SERVEPANEL_XRAY_DETECT_RDY_LED, detectorRdy);
		SetCtrlVal(SERVEPANEL, SERVEPANEL_DETECTOR_TOGGLE, detectorRdy);
	    //detectorFail=0;
	}
	
	if(sideCamInitialization())
	
	{
		sideCameraRdy=1; //side cameral ready indicator
		SetCtrlVal(SERVEPANEL, SERVEPANEL_XRAY_SIDECAM_RDY_LED, sideCameraRdy);
		SetCtrlVal(SERVEPANEL, SERVEPANEL_SIDECAM_TOGGLE, sideCameraRdy);
		//sideCameraFail=0;  
	}
	
	if(xraySourceRdy && detectorRdy && sideCameraRdy)
		writeToLog("Acquisition hardware initialized successfully.");
		return 0;
}


int xraySourceInitialization()
{
		// Initialize HV_enable
		HV_Enable = 1;  
		writeToLog("X-ray source initialized successfully."); 
		return 1;
}

// Acquisition hardware termination
int hardwareTermination()
{
	//source termination
	   xraySourceTermination();
	
	//detector termination  
	   detectorTermination();

	//side camera termination
	   sideCamTermination();

	writeToLog("Acquisition hardware terminated.");
	return 0;
	  
}

int detectorInitialization() { writeToLog("Detector initialized successfully."); return 1; }
int sideCamInitialization() { writeToLog("Side camera initialized successfully."); return 1; }
int xraySourceTermination() { return 1; }
int detectorTermination() { return 1; }
int sideCamTermination() { return 1; }


/*Offset image correction*/
void offsetImageCorrection(void)
{
	//local variables
	unsigned long nBufSize = MAX_IMAGE_LENGTH;		//buffer size for xray image
	unsigned int nWidth = IMAGE_WIDTH;		 //image wide
	unsigned int nHeight = IMAGE_HEIGHT;	 //image height   
	unsigned int EntrySize = 2000;
		
	short errcode;
	unsigned long nBytes; 			

	short* imgBuf_temp = (short*)malloc(nBufSize);
	char errMessage[256];

	//open raw offset image
	sprintf(tempString, "%s%s", systemFilesPath, rawOffsetImageFilePath);
	HANDLE hFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		writeToLog("Open raw offset image error.");
	}
	
	ReadFile(hFile, imgBuf_temp, nBufSize-1, &nBytes, NULL) ;
		
	for (int i=0; i<nBufSize/2; i++)
        imgBuf_temp[i] = (((unsigned short)imgBuf_temp[i])>>8) + (((unsigned char)imgBuf_temp[i])<<8); 

	/*Pixel correction*/
	sprintf(tempString, "%s%s", systemFilesPath, detectorPixelMap);
	char *filePath = tempString;		 // file path to detecotor's pixel map
	PIXMAPENTRY *pixMap_temp;
	pixMap_temp = (PIXMAPENTRY*)malloc(SCMAXPIXMAPSIZE * sizeof(PIXMAPENTRY));
	
	int *numEntries_temp = (int *)malloc(EntrySize);

	//read pixel map
	errcode = ScReadPixMap(filePath, pixMap_temp, numEntries_temp);
	if (errcode !=0)
	{  ScErrorMessage (errcode, errMessage);
		MessagePopup("Open Pixel Map Error", errMessage);
	}
	else
	{ 
		unsigned short nPixMapCount= *numEntries_temp;
		//unsigned short pcMethod = SCMETHOD_MEAN;
		//unsigned short pcMethod = SCMETHOD_INTERPOLATE;
		unsigned short pcMethod = SCMETHOD_GRADIENT;
		//pixel correction 
		errcode = ScPixelCorrection(imgBuf_temp, nWidth, nHeight, pixMap_temp, nPixMapCount, pcMethod);

		if (errcode !=0)
		{  	ScErrorMessage (errcode, errMessage);
			MessagePopup("Pixel Map correction Error", errMessage);
		}
		else
		{ 
			//write processed offset image to file
			sprintf(tempString, "%s%s", systemFilesPath, correctedOffsetImageFilePath); 
			HANDLE pFile = CreateFile(tempString, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
			if ( pFile != INVALID_HANDLE_VALUE ) 
				{
				for (int i=0; i<nBufSize/2; i++)   
	            imgBuf_temp[i] = (((unsigned short)imgBuf_temp[i])>>8) + (((unsigned char)imgBuf_temp[i])<<8);
			 
				WriteFile(pFile, imgBuf_temp, nBufSize, &nBytes, NULL);
				writeToLog ("Offset image processed successfully."); 
				CloseHandle(pFile);
				} 
		}  
	}
	CloseHandle(hFile);
	free(imgBuf_temp);
	free(pixMap_temp);
	free(numEntries_temp);

}


/*Gain image correction*/
void gainImageCorrection(void)
{
		//local variables
		unsigned long nBufSize = MAX_IMAGE_LENGTH;		//buffer size for xray image
		unsigned long gainBufSize = MAX_IMAGE_LENGTH;   //buffer size for gain image
		unsigned long offsetBufSize = MAX_IMAGE_LENGTH;  //buffer size for offset image
		unsigned int nWidth = IMAGE_WIDTH;		 //image wide
		unsigned int nHeight = IMAGE_HEIGHT;	 //image height
		
		unsigned int EntrySize = 2000;
		short errcode;
		unsigned long nBytes; 			

		//allocate buffer
		short* imgBuf_temp = (short*)malloc(nBufSize);
		short* offsetImgBuf_temp = (short*)malloc(offsetBufSize);
		char errMessage[256];

		//open raw gain image
		sprintf(tempString, "%s%s", systemFilesPath, rawGainImageFilePath);  
		HANDLE hFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hFile==INVALID_HANDLE_VALUE)
			{
				writeToLog("Open raw gain image error.");
			}
 		ReadFile(hFile, imgBuf_temp, nBufSize-1, &nBytes, NULL) ;
		
		for (int i=0; i<nBufSize/2; i++)
             imgBuf_temp[i] = (((unsigned short)imgBuf_temp[i])>>8) + (((unsigned char)imgBuf_temp[i])<<8);
		
		/*offset correction for gain image */
		//open offset image
		sprintf(tempString, "%s%s", systemFilesPath, correctedOffsetImageFilePath);
		HANDLE tFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 		ReadFile(tFile, offsetImgBuf_temp, offsetBufSize-1, &nBytes, NULL) ;  
	
		for (int i=0; i<offsetBufSize/2; i++)
        	offsetImgBuf_temp[i] = (((unsigned short)offsetImgBuf_temp[i])>>8) + (((unsigned char)offsetImgBuf_temp[i])<<8);
		
		//offset correction
		errcode =ScOffsetCorrection(imgBuf_temp, offsetImgBuf_temp, nWidth, nHeight);

		/*Pixel correction for gain iamge*/
		sprintf(tempString, "%s%s", systemFilesPath, detectorPixelMap);
		char *filePath = tempString;		 // file path to detecotor's pixel map
		PIXMAPENTRY *pixMap_temp;
		pixMap_temp = (PIXMAPENTRY*)malloc(SCMAXPIXMAPSIZE * sizeof(PIXMAPENTRY));
	
		int *numEntries_temp = (int *)malloc(EntrySize);
		//read pixel map
		errcode = ScReadPixMap(filePath, pixMap_temp, numEntries_temp);
		if (errcode !=0)
		{  
			ScErrorMessage (errcode, errMessage);
			MessagePopup("Open Pixel Map Error", errMessage);
		}
		else
		{ 
			unsigned short nPixMapCount= *numEntries_temp;
			unsigned short pcMethod = SCMETHOD_MEAN;
			//unsigned short pcMethod = SCMETHOD_INTERPOLATE;
			//unsigned short pcMethod = SCMETHOD_GRADIENT;
		
			// pixel correction
			errcode = ScPixelCorrection(imgBuf_temp, nWidth, nHeight, pixMap_temp, nPixMapCount, pcMethod);

			if (errcode !=0)
			{
				ScErrorMessage (errcode, errMessage);
				MessagePopup("Open Pixel Map Error", errMessage);
			}
		
			//save processed gain image
			sprintf(tempString, "%s%s", systemFilesPath, correctedGainImageFilePath);  
			HANDLE pFile = CreateFile(tempString, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
			if ( pFile != INVALID_HANDLE_VALUE ) 
			{
				for (int i=0; i<nBufSize/2; i++)   
            	imgBuf_temp[i] = (((unsigned short)imgBuf_temp[i])>>8) + (((unsigned char)imgBuf_temp[i])<<8);
			    WriteFile(pFile, imgBuf_temp, nBufSize, &nBytes, NULL);
				writeToLog("Gain image processed successfully."); 
				CloseHandle(pFile); 
			} 
		}  
		CloseHandle(tFile);
		CloseHandle(hFile);
		free(imgBuf_temp);
		free(offsetImgBuf_temp);
		free(pixMap_temp); 
		free(numEntries_temp); 
	
}
