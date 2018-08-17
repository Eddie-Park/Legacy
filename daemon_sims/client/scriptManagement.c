
#include "windows.h"
#include "winbase.h"
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "constants.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "utilities.h"
#include "logManagement.h"
#include "scriptManagement.h"
#include "protocolEdit.h"
#include "runManagement.h"

#define MAX_TOKEN 32

static FILE* scriptLog=NULL;
static char* tokenStack[MAX_TOKEN];
static int numToken=0;
static char* myStrTok(char* string, char* delims);

static char* editKeywords[]={"PROTOCOL_NAME", "ITERATIONS", "SIGMA", "RECON", "HIST", "LLD", "ULD"};
static char* editProtocolValues[sizeof(editKeywords)/sizeof(char*)];
static const int numEditProtocolValues=(int) (sizeof(editKeywords)/sizeof(char*));

static char* runKeywords[]={"PROTOCOL_NAME", "STUDY_NAME"};
static char* runProtocolValues[sizeof(runKeywords)/sizeof(char*)];
static const int numRunProtocolValues=(int) (sizeof(runKeywords)/sizeof(char*));

static char* exportKeywords[]={"STUDY_NAME", "RAW_IMAGE", "SCN"};
static char* exportProtocolValues[sizeof(exportKeywords)/sizeof(char*)];
static const int numExportProtocolValues=(int) (sizeof(exportKeywords)/sizeof(char*));

int CVICALLBACK scriptHandler(void *functionData);
static int parseLine(char*  line);

static inline int allocateValues(char* v[], int n){
	for (int k=0; k<n; k++) {
		v[k]=malloc(MAX_FILE_NAME_LENGTH);
		if (NULL!= v[k]) (*v[k])=0;
		else return -1;
	}
	return 0;
}

static inline void freeValues(char* v[], int n){
	for (int k=0; k<n; k++) free(v[k]);
}

int scriptManagementInitialization(){
	CmtNewLock(NULL, 0, &scriptingLock);
	if (0!=allocateValues(editProtocolValues,   numEditProtocolValues   ) ) return -1;
	if (0!=allocateValues(runProtocolValues,    numRunProtocolValues    ) ) return -1;
	if (0!=allocateValues(exportProtocolValues, numExportProtocolValues ) ) return -1;
	SetCtrlAttribute(mainPanel, MAIN_PANEL_scriptBanner, ATTR_VISIBLE, 0);    
	return 0;	
}

int scriptManagementTermination(){
	CmtDiscardLock(scriptingLock);
	freeValues(editProtocolValues,   numEditProtocolValues   );
	freeValues(runProtocolValues,    numRunProtocolValues    );
	freeValues(exportProtocolValues, numExportProtocolValues );
	return 0;
}



void CVICALLBACK scriptExecution_CB (int menuBar, int menuItem, void *callbackData,
		int panel){
	
	char*  scriptName        = malloc(MAX_FILE_NAME_LENGTH);
	int    gotLock=0;
	
	if (NULL==scriptName) return;
	
	CmtTryToGetLock(scriptingLock, &gotLock);
	if (!gotLock){
		MessagePopup("Error", "A script is already running");
		return;
	}
	CmtReleaseLock(scriptingLock);
	
	if ( 0!= FileSelectPopup("","*.txt", "", "Select Script File to Run", VAL_SELECT_BUTTON, 0, 0, 1, 0, scriptName) ){
 		CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, scriptHandler, scriptName, 0);
 	}

}

int CVICALLBACK scriptHandler(void *functionData){
	char* scriptName = (char*) functionData;
	char message[512];
	time_t currentTime;
	char localTimeString[200];
	size_t length;
	
	CmtGetLock(scriptingLock);
	
	// Write message in banner
	sprintfWithLock(512, message,"          Script %s running ...", scriptName);
	SetCtrlVal      (mainPanel, MAIN_PANEL_scriptBanner, message);
	SetCtrlAttribute(mainPanel, MAIN_PANEL_scriptBanner, ATTR_VISIBLE, 1);
	
	// Write message in system log
	sprintfWithLock(512, message,"Running script %s", scriptName);
	writeToLog(message);
	
	// Prepare a script logfile
	char logFileName[MAX_FILE_NAME_LENGTH];
	sprintfWithLock(MAX_FILE_NAME_LENGTH,logFileName,"%s.log", scriptName);
	
	scriptLog=fopen(logFileName,"w");
	time(&currentTime);
	myStrcpy(localTimeString, ctime(&currentTime) );
	length = strlen(localTimeString);
	*(localTimeString+length-1) = 0;
  	if (scriptLog) fprintf(scriptLog, "%s - Script started\n", localTimeString );    
	
	// Read and run the script
	FILE  *fdata;
	char buffer[512];
	
	fdata = fopen(scriptName,"r");
	if (fdata){
		while(NULL!=fgets(buffer, (int) sizeof(buffer), fdata) ){
			chomp(buffer);
			if ( parseLine(buffer) ){
				writeToLog("Error in script execution");
				break;
			}
		}
		fclose(fdata);
	}
										  
	// Script ends
	time(&currentTime);
	myStrcpy(localTimeString, ctime(&currentTime) );
	length = strlen(localTimeString);
	*(localTimeString+length-1) = 0;
  	if (scriptLog) fprintf(scriptLog, "%s - Script Ended\n", localTimeString ); 
	fclose(scriptLog);
	scriptLog=NULL;
	
	// we're done
	sprintfWithLock(512, message,"Script %s ended. ", scriptName);
	writeToLog(message);
	
	SetCtrlAttribute(mainPanel, MAIN_PANEL_scriptBanner, ATTR_VISIBLE, 0);
	CmtReleaseLock(scriptingLock);
	free(scriptName);
	return 0;
}

static inline int getParmIndex(char* token, char* parmArray[], int numParms){
	for (int k=0; k<numParms; k++){
		if ( 0 == strcmp(token, parmArray[k]) ) return k;
	}
	return -1;
}


static inline void myStrcpyNoQuotes(char* dest, char* source){
	if (*source == '"'){ 
		source++; // skip initial quote
		while( (*dest++)=(*source++) );   // Expect warning here too (operator used in conditional expression)
		*(dest-2)=0;  // at this point dest is passed the ending 0;
	} else {
		myStrcpy(dest, source);
	}
}

// Inspects a list of token pairs (keyword, value)
// and puts the value in the appropriate array slot
static inline void fillValues(char* parms[], char* values[], int n){
	for (int k=0; k<numToken; k++){
		// Get parameter index
		int pidx = getParmIndex(tokenStack[k], parms, n);
		if (pidx>=0) myStrcpyNoQuotes(values[pidx], tokenStack[++k]);
	}
}




int editProtocolHandler(void){
	/*
	int index;
	struct protocol prot;
	
	fillValues(editKeywords, editProtocolValues, numEditProtocolValues);

	fprintf(scriptLog, "[EDIT PROTOCOL] - %s, iterations %s, sigma %s\n", editProtocolValues[0], editProtocolValues[1], editProtocolValues[2] );
	sprintfWithLock(MAX_FILE_NAME_LENGTH, fullProtocolPath, "%s\\%s.ptc", protFolder,editProtocolValues[0] );
	index = findIndexFromFullProtocolPath(fullProtocolPath); 
	
	if (index==0){
		fprintf(scriptLog, "    Error finding protocol \"%s\"\n", fullProtocolPath );
		return 8;
	}
	if ( 0!= readProtocol(index, &prot) ){
		fprintf(scriptLog, "    Error opening protocol \"%s\"\n", fullProtocolPath );
		return 8;
	}
	
	if (0!= *editProtocolValues[1]) prot.iterations = atoi( editProtocolValues[1] );
	if (0!= *editProtocolValues[3]) {
		if ( 0 == strcmp(editProtocolValues[3], "YES" ) ) prot.recon=1;
		else                                              prot.recon=0;
	}
	if (0!= *editProtocolValues[4]) {
		if ( 0 == strcmp(editProtocolValues[3], "YES" ) ) prot.hist=1;
		else                                              prot.hist=0;
	}
	
	if (0!=writeProtocolToDisk(fullProtocolPath, &prot)){
		fprintf(scriptLog, "    Error writing protocol \"%s\"\n", fullProtocolPath );
		return 8;
	}
	fprintf(scriptLog, "    Protocol updated.\n" );

	return 0;
	
	*/
return 0;
}



int runProtocolHandler(void){
	
	/*
	char fullStudyPath[MAX_FILE_NAME_LENGTH];
	char fullProtocolPath[MAX_FILE_NAME_LENGTH];
	int studyIndex=0;
	int protocolIndex=0;
	
	fillValues(runKeywords, runProtocolValues, numRunProtocolValues);
	fprintf(scriptLog, "[RUN PROTOCOL] - protocol %s in study %s\n", runProtocolValues[0], runProtocolValues[1] );
	
	// Get study index
	sprintfWithLock(MAX_FILE_NAME_LENGTH, fullStudyPath, "%s\\%s", studyFolder, runProtocolValues[1] );
	studyIndex = findIndexFromFullStudyPath(fullStudyPath); 
	
	if (studyIndex==0){
		fprintf(scriptLog, "    Error finding study \"%s\"\n", fullStudyPath );
		return 8;
	}
	
	// Get protocol index
	sprintfWithLock(MAX_FILE_NAME_LENGTH,fullProtocolPath, "%s\\%s.ptc", protFolder, runProtocolValues[0] );
	protocolIndex = findIndexFromFullProtocolPath(fullProtocolPath); 
	if (protocolIndex==0){
		fprintf(scriptLog, "    Error finding protocol \"%s\"\n", fullProtocolPath );
		return 8;
	}

	fprintf(scriptLog, "    Launching ... study(%d), protocol(%d)\n", studyIndex, protocolIndex);
	runBatchProtocol(studyIndex, protocolIndex);
 
	
	*/
	return 0;
	

}

int exportSingleFile(char* fullStudyPath, char* mask, char* dest){
  	char searchPath[MAX_FILE_NAME_LENGTH];
 	char fileName[MAX_FILE_NAME_LENGTH];
	int retCode=0;
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH, searchPath,    "%s\\%s", fullStudyPath, mask);
	
	if ( 0== GetFirstFile(searchPath, 1, 0, 0, 0, 0, 0, fileName) ){
		sprintfWithLock(MAX_FILE_NAME_LENGTH, searchPath,    "%s\\%s", fullStudyPath, fileName);
		fprintf(scriptLog, "    Copying \"%s\" to \"%s\"\n",searchPath, dest);
		if ( 0==CopyFileA(searchPath, dest, FALSE)){
			retCode+=8;
			fprintf(scriptLog, "    Copy failed - reason %d\n", GetLastError());
		}
	} else {
		retCode += 8;
		fprintf(scriptLog, "    Can't find input file %s\n",searchPath);
	}
//		getFileRetCode = GetNextFile(fileName);
	
	return retCode;
}

int exportHandler(void){
 	char fullStudyPath[MAX_FILE_NAME_LENGTH];
 	int retCode=0;
	
	fillValues(exportKeywords, exportProtocolValues, numExportProtocolValues);
	fprintf(scriptLog, "[EXPORT] - Study %s, RAW_IMAGE=%s, SCN=%s\n", exportProtocolValues[0], exportProtocolValues[1], exportProtocolValues[2] );

	// Make study path
	sprintfWithLock(MAX_FILE_NAME_LENGTH,fullStudyPath, "%s\\%s", studyFolder, exportProtocolValues[0] );
	
	// Export raw image
	if ( 0 != (*exportProtocolValues[1]) )  retCode +=exportSingleFile(fullStudyPath, "*.im3", exportProtocolValues[1]);

	// Export histogram
	if ( 0 != (*exportProtocolValues[2]) )  retCode +=exportSingleFile(fullStudyPath, "*.scndat", exportProtocolValues[2]);

  	
	return retCode;
}


typedef int handlerFunction (void);

static const int numCommands=3;
static char* commands[]={"EDIT_PROTOCOL", "RUN_PROTOCOL", "EXPORT"};
static handlerFunction* commandHandlerArray[]={editProtocolHandler, runProtocolHandler, exportHandler};

static inline int getCommandIndex(char* token){
	for (int k=0; k<numCommands; k++){
		if ( 0 == strcmp(token, commands[k]) ) return k;
	}
	return -1;
}

// my version of strtok that handles being in between quotes
static char* curStr=0;
static char* myStrTok(char* string, char* delims){
	
	char* curDelim=0;
	char* start=0;
	
	// Initialize on initial call, otherwise use old value
	if (string) curStr = string;
	start=curStr;
	
	if ( (*curStr)=='"' )
		while ( (*(++curStr))!='"' ) ;
	
	while (*curStr){
		curDelim = delims;
		// Inspect this string position
		while(*curDelim){
			if ( (*curStr) == (*curDelim++) ){
				// Found delimiter
				(*curStr++)=0; // end string here and reposition for next call
				return start;  // return
			}
		}
		// no delimiter here, inspect next position
		curStr++;
	}
	if (*start) return start;
	else        return NULL;
}


// Parse a line keeping the command and stacking all other tokens
static int parseLine(char* orgLine){
	char* token;
	char  line[512];
	
	myStrcpy(line, orgLine);	// Copy original line (strtok is destructive)
	token = myStrTok(line, " \t");
	
	numToken = 0;
	while (token!=NULL){
		int cmdIdx = getCommandIndex(token);
		if (cmdIdx<0) return 0;                 // skip this line if it does not begin with a command
	
		// Skip command name and add remaining tokens from this line to the stack
		token = myStrTok(NULL,"=, \t");
		while (token!=NULL){
			if ((*token)!=0) tokenStack[numToken++]=token;  // add only if non-null token
			token = myStrTok(NULL,"=, \t");
		}
		if (commandHandlerArray[cmdIdx] ()) return 8;	   // execute command handler   
	}
	return 0;
	
}
