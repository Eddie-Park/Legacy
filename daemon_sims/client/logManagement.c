#include "windows.h"
#include <formatio.h>
#include <utility.h>
#include <userint.h>
#include <ansi_c.h>
#include "globalDecls.h"
#include "logManagement.h"
#include "utilities.h"

static HANDLE mutexLogHandle;

int logManagementInitialization(){

	char logDataFileName [MAX_FILE_NAME_LENGTH];
	char returnedFileName[MAX_FILE_NAME_LENGTH];
	int  logFileNumber=0;
	DWORD  size;
	int retCode;
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH,logDataFileName, "%s\\logFiles", appDataDirectory);
	retCode = GetFirstFile(logDataFileName, 1,0,0,0,0,1,returnedFileName);
	if (retCode == -1) retCode = MakeDir(logDataFileName);
	if (retCode == 0 || retCode == -9){
		// OK
	} else {
		return -1;
	}

	size=4;
	retCode = RegQueryValueEx(petBoxRegKey, "LogFileNumber",  NULL, NULL, (LPBYTE)&logFileNumber,  &size);
	sprintfWithLock(MAX_FILE_NAME_LENGTH, logDataFileName,"%s\\logFiles\\PETBoxLogFile_%d.txt",appDataDirectory, logFileNumber++ );
	RegSetValueEx(petBoxRegKey, "LogFileNumber",  0, REG_DWORD,  (BYTE*)&logFileNumber,   4);
	
	logFile=fopen(logDataFileName,"w");
	mutexLogHandle = CreateMutex(NULL, FALSE, "mutex_log");
	
	char temp[128];
	sprintfWithLock(128, temp, "Genisys GUI Version 2.0 Build %s - Started", _TARGET_FILE_VERSION_);
	writeToLog(temp);
 	
	return 0;

}

int logManagementTermination(){
	fclose(logFile);
	CloseHandle(mutexLogHandle);
	return 0;
}

void traceToLog(char* message){
	if (trace) writeToLog(message);
}

void writeToLog(char* message){
	time_t currentTime;
	char localTimeString[200];
	size_t  length;
	
	
	if (logFile && mutexLogHandle){
		
		DWORD retCode = WaitForSingleObject(mutexLogHandle, 500);
	
		switch (retCode){
		
			// Got the lock
			case WAIT_OBJECT_0:
				time(&currentTime);
				myStrcpy(localTimeString, ctime(&currentTime) );
				length = strlen(localTimeString);
				*(localTimeString+length-1) = 0;

				fprintf(logFile, "%s - %s\n", localTimeString, message ); 
				fflush(logFile);
				break;
			
			case WAIT_TIMEOUT:
				break;
			
			case WAIT_ABANDONED:
				break;
		}
	
		ReleaseMutex(mutexLogHandle);

	}
}
