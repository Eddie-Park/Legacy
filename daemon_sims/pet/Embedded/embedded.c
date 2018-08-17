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
//*                                                                             *
//*******************************************************************************


//  1232 - Dont't open TCP communications until firmware is loaded; switched to target file version (for build ID)
//	1233 - PET LED control added
//  1231 - Implemented the systemFiles folder
//         Fixed several bugs with the registry
//  1230 - Getting ready to test all daemons, set product ID to 1001004, build ID, "system files" folder
//  1226 - GetTCPPeerAddr instead of name 


#include "windows.h"
#include <tcpsupp.h>
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <NIDAQmx.h>

#include "commonHeader.h"
#include "GlobalDefs.h"
#include "server.h"
#include "embedded.h"
	 
#define PET_BOX_REG_KEY       "SOFTWARE\\Sofie Inc.\\PETBoxServer\\1.0"
#define PET_BOX_ACQ_REG_KEY   "SOFTWARE\\Sofie Inc.\\PETBoxServer\\1.0\\Acquisition"



// Local functions
static int mainInitialization(void);
static int mainTermination(void);

/*
	Main function
*/
int main (int argc, char * argv[]){
	
	mainInitialization();
	SetWaitCursor(1);
	SetPanelAttribute(mainPanel, ATTR_DIMMED, 1);
	DisplayPanel (mainPanel);
	acqHardwareInitialization();
	serverInitialization();
	
	//MinimizeAllWindows();
	SetPanelAttribute(mainPanel, ATTR_DIMMED, 0);
	SetWaitCursor(0);
	
	char temp[128];
	sprintf(temp, "PET Acquisition 2.0 - build %s", _TARGET_FILE_VERSION_);
	SetCtrlVal(mainPanel, PANEL_buildID, temp);
	RunUserInterface();
	DiscardPanel (mainPanel);

	acqHardwareTermination();
	serverTermination();
	mainTermination();
	
	return 0;
}

/*
	Main initialization
*/
static int mainInitialization(){
	char appDataFolder[MAX_FILE_NAME_LENGTH];
	char logDataFileName[MAX_FILE_NAME_LENGTH];
	DWORD  disposition;
	int  logFileNumber=0;
	DWORD  size;
	unsigned long pathtype=REG_SZ;
	int FPGA_offset_value;
	char temp[1024];

	// Initialize GUI (load panels)
 	if ((mainPanel     = LoadPanel(0,"embedded.uir",PANEL))     <= 0) return -1;
 
	// Try to create the registry key
	// The registry key is at the local machine level
	// and contains the number to use in the log file name.
	RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			PET_BOX_REG_KEY, 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&petBoxRegKey,
			&disposition);								  
	

	// Registry key already exists, then read it
	if (disposition==2) {
		size=sizeof(logFileNumber);
		RegQueryValueEx(petBoxRegKey, "LogFileNumber",         NULL, NULL,   (LPBYTE)&logFileNumber,        &size);

		size=sizeof(serialNumber);
		RegQueryValueEx(petBoxRegKey, "SerialNumber",          NULL, NULL,   (LPBYTE)&serialNumber,         &size);
		
		size=sizeof(globalSequenceNumber);
		RegQueryValueEx(petBoxRegKey, "GlobalSequenceNumber",  NULL, NULL,   (LPBYTE)&globalSequenceNumber, &size);
		
		size=MAX_FILE_NAME_LENGTH;
		RegQueryValueEx(petBoxRegKey, "SystemFiles",           NULL, NULL,   (BYTE*)systemFilesPath,        &size);
		
		size=MAX_FILE_NAME_LENGTH;
		RegQueryValueEx(petBoxRegKey, "LUT",                   NULL, NULL,   (BYTE*)lutFilePath,            &size);
		
		size=MAX_FILE_NAME_LENGTH;
		RegQueryValueEx(petBoxRegKey, "LUTLoader",             NULL, NULL,   (BYTE*)lutLoaderFilePath,      &size);
		
		size=MAX_FILE_NAME_LENGTH;
		RegQueryValueEx(petBoxRegKey, "ACQBitStream",          NULL, NULL,   (BYTE*)FPGABitstreamFilePath,  &size);
		
		
	} else{
		
		/* Ask (nicely) for a serial number */
		serialPanel = LoadPanel(0,"embedded.uir",PANEL_SER);
		InstallPopup(serialPanel);
		SetActiveCtrl(serialPanel, PANEL_SER_serNum); 
		RunUserInterface();
		GetCtrlVal(serialPanel, PANEL_SER_serNum, &serialNumber);
		RemovePopup(0);
		
		RegSetValueEx(petBoxRegKey, "SerialNumber",          0, REG_DWORD,  (LPBYTE)&serialNumber,           4);
		globalSequenceNumber=0;
		RegSetValueEx(petBoxRegKey, "GlobalSequenceNumber",  0, REG_DWORD,  (LPBYTE)&globalSequenceNumber,   4);
		
		RegSetValueEx(petBoxRegKey, "SystemFiles",           0, REG_SZ,     (BYTE*)systemFilesPath,        1+strlen(systemFilesPath)      );
		RegSetValueEx(petBoxRegKey, "LUT",                   0, REG_SZ,     (BYTE*)lutFilePath,            1+strlen(lutFilePath)          );
		RegSetValueEx(petBoxRegKey, "LUTLoader",             0, REG_SZ,     (BYTE*)lutLoaderFilePath,      1+strlen(lutLoaderFilePath)    );
		RegSetValueEx(petBoxRegKey, "ACQBitStream",          0, REG_SZ,     (BYTE*)FPGABitstreamFilePath,  1+strlen(FPGABitstreamFilePath));
		
		
	}
	sprintf(temp,"This is PETBox #%i",serialNumber);
	SetCtrlVal(mainPanel, PANEL_petBoxID, temp);
	

	// Create PETBox server in "...\Application Data" if necessary
	sprintf(appDataFolder,"%s\\PETBoxServer",getenv("APPDATA"));		  //C:\Users\sofie\AppData\Roaming\PETBoxServer
	MakeDir(appDataFolder);
	
	// Update the registry key
 	sprintf(logDataFileName,"%s\\ServerLogFile_%d.txt",appDataFolder, logFileNumber++ );
	RegSetValueEx(petBoxRegKey, "LogFileNumber",  0, REG_DWORD,  (BYTE*)&logFileNumber,   4);
	
	// ... and open log file
	logFile=fopen(logDataFileName,"w");
	if (logFile){
		SetCtrlVal(mainPanel,PANEL_LOG_LED, 1);
		sprintf(temp, "Genisys(4) PET Acquisition - Started build  %s", _TARGET_FILE_VERSION_);
		writeToLog(temp);
	} else 	{
		SetCtrlVal(mainPanel,PANEL_LOG_LED, 0);
	}

	// added by Hui
	// Try to create the acquistion registry key
	// The acqusition registry key is a sub key of PET_BOX_REG_KEY
	// and contains the parameters for acquisition.
	RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			PET_BOX_ACQ_REG_KEY, 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&petBoxAcqRegKey,
			&disposition);								  
	
	// Registry key already exists, then read it
	if (disposition==2) {
		
		size=sizeof(FPGA_offset_value);
		RegQueryValueEx(petBoxAcqRegKey, "LLD",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  &size);
		FPGA_LLD=FPGA_offset_value;
		
		size=sizeof(FPGA_offset_value);
		RegQueryValueEx(petBoxAcqRegKey, "ULD",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  &size);	
		FPGA_ULD=FPGA_offset_value;
		
		size=sizeof(FPGA_Noise_Threshold);
		RegQueryValueEx(petBoxAcqRegKey, "Noise_Threshold",       NULL, NULL, (LPBYTE)&FPGA_Noise_Threshold,      &size);	
		
		size=sizeof(FPGA_Event_Threshold);
		RegQueryValueEx(petBoxAcqRegKey, "Evt_Threshold",         NULL, NULL, (LPBYTE)&FPGA_Event_Threshold,      &size);	

		size=sizeof(FPGA_Blocking_Time);
		RegQueryValueEx(petBoxAcqRegKey, "Evt_Blocking_Time",     NULL, NULL, (LPBYTE)&FPGA_Blocking_Time,        &size);	

		size=sizeof(FPGA_Integration_Time);
		RegQueryValueEx(petBoxAcqRegKey, "Evt_Integration_Time",  NULL, NULL, (LPBYTE)&FPGA_Integration_Time,     &size);	
												   
		size=sizeof(FPGA_Coinc_Window_d);
		RegQueryValueEx(petBoxAcqRegKey, "Coinc_Timing_Window",   NULL, NULL, (LPBYTE)&FPGA_Coinc_Window_d,       &size);

		size=sizeof(msTimerPeriod);
		RegQueryValueEx(petBoxAcqRegKey, "Timer_Period_ms",       NULL, NULL, (LPBYTE)&msTimerPeriod,             &size);
		
		size=MAX_FILE_NAME_LENGTH;
		RegQueryValueEx(petBoxAcqRegKey, "RawDataFilePath",       NULL, &pathtype, (LPBYTE)&rawDataFilePath[0],    &size);	

	}
	else // set parameters to default values
	{
		FPGA_LLD              = 150;
		FPGA_ULD              = 650;
		FPGA_Noise_Threshold  = 50;
		FPGA_Event_Threshold  = 300;
		FPGA_Blocking_Time    = 100;
		FPGA_Integration_Time = 80; //**
		FPGA_Coinc_Window_d   = 20;
		msTimerPeriod         = 25;		
	 	sprintf(rawDataFilePath,"%s",DEFAULT_RAW_DATA_PATH);
	}
	
	// Update the registry key
	RegSetValueEx(petBoxAcqRegKey, "LLD",                   0, REG_DWORD,  (BYTE*)&FPGA_LLD,                4);
	RegSetValueEx(petBoxAcqRegKey, "ULD",                   0, REG_DWORD,  (BYTE*)&FPGA_ULD,                4);
	RegSetValueEx(petBoxAcqRegKey, "Noise_Threshold",       0, REG_DWORD,  (BYTE*)&FPGA_Noise_Threshold,    4);
	RegSetValueEx(petBoxAcqRegKey, "Evt_Threshold",         0, REG_DWORD,  (BYTE*)&FPGA_Event_Threshold,    4);
	RegSetValueEx(petBoxAcqRegKey, "Evt_Blocking_Time",     0, REG_DWORD,  (BYTE*)&FPGA_Blocking_Time,      4);
	RegSetValueEx(petBoxAcqRegKey, "Evt_Integration_Time",  0, REG_DWORD,  (BYTE*)&FPGA_Integration_Time,   4);
	RegSetValueEx(petBoxAcqRegKey, "Coinc_Timing_Window",   0, REG_DWORD,  (BYTE*)&FPGA_Coinc_Window_d,     4);
	RegSetValueEx(petBoxAcqRegKey, "Timer_Period_ms",       0, REG_DWORD,  (BYTE*)&msTimerPeriod,           4);
	RegSetValueEx(petBoxAcqRegKey, "RawDataFilePath",       0, REG_SZ,     (BYTE*)&rawDataFilePath[0],      strlen(rawDataFilePath)+1);

	panelUpdateFreq = 1000/msTimerPeriod;
	
	MakeDir(rawDataFilePath);

	// end added by Hui
	long checkSize = 0;
	if (GetFileSize("n:\\PETbox_StudyData\\sample.lmf", &checkSize) != 0)
	{
		CopyFile("c:\\test\\sample.lmf", "n:\\PETbox_StudyData\\sample.lmf");
		writeToLog("Sample LMF file written to n:");
	}
	else
	{
		writeToLog("Sample LMF file n:\\PETbox_StudyData\\sample.lmf found");
	}
		
	return 0;
}

/*
	Main Termination
*/
static int mainTermination(){
	writeToLog("PETBox Server Terminated Normally");
	if (logFile) fclose(logFile);
	return 0;
}

/*
	Write to log function
	
	writes "message" to the log file.  It prepends a time stamp and
	the buffer is written immediattely
*/
void writeToLog(char* message){
	time_t currentTime;
	char localTimeString[256];
	int  length;

	if (logFile){
		time(&currentTime);
		strcpy(localTimeString, ctime(&currentTime) );
		length = strlen(localTimeString);
		*(localTimeString+length-1) = 0;
	
		fprintf(logFile, "%s - %s\n", localTimeString, message ); 
		fflush(logFile);
	}
}

/*
	GUI callback function 
	
	Invoked when the user tries to close the window (close box)
	Ask for confirmation if an acquisition is in progress
*/
int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int confirm;
	switch (event)
	{
		case EVENT_COMMIT:
			if (acquisitionThreadRunning){
				confirm = ConfirmPopup("Confirm Exit", "Acquisition is in progress; exiting will terminate it.\nDo you want to exit anyway?");
				if (!confirm) return 0;
			}
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OK_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			 QuitUserInterface(0);
			break;
	}
	return 0;
}

int  CVICALLBACK CB_SET_ACQ_HW_NOT_READY(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
	acqHardwareRdy = 0;
	SetCtrlVal(mainPanel,PANEL_ACQ_HW_READY, acqHardwareRdy); 
	}
	return 0;
}

int  CVICALLBACK CB_SET_ACQ_HW_READY(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
	acqHardwareRdy = 1;
	SetCtrlVal(mainPanel,PANEL_ACQ_HW_READY, acqHardwareRdy); 
	
	}
	return 0;
}
