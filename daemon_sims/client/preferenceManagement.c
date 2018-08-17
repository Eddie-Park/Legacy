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
#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include "typedefs.h"
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "preferenceManagement.h"
#include "cameraManagement.h"
#include "utilities.h"
#include "logManagement.h"

static int  readPreferences(void);
static void makeDefaultPreferences(void);
static void inCoreToPanel(void);
static void panelToInCore(void);
static void ipToShortInt(char *ip, unsigned short int *ip1, unsigned short int *ip2, unsigned short int *ip3, unsigned short int *ip4);
static void registryInitialization(void);

int preferenceManagementInitialization(void){
	int retCode=0;	 
	DWORD disposition;
	
	sprintfWithLock(MAX_PATHNAME_LEN, appDataDirectory,"%s\\PETBox",getenv("APPDATA"));
	setDefaultPreferences(); // in case a later read does not work
	
	RegCreateKeyEx(
			HKEY_CURRENT_USER,
			PET_BOX_REG_KEY, 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&petBoxRegKey,
			&disposition);
	
	/*
		If PETBox key absent, this is the very first run:
		initialize with default values
	*/
	if (disposition==1) {						 
		registryInitialization();
		MakeDir(appDataDirectory);	
		MakeDir(studyFolder);
		MakeDir(protFolder);
		MakeDir(normFolder);
	} else{
		retCode = readPreferences();
	}
	
	/* determine if we're running in "physics mode" */
	DWORD size    = 1;
	RegQueryValueEx(petBoxRegKey, "PhysicsMode",       NULL, NULL, (LPBYTE)&physicsMode,       &size);
	
	/* Read the default calibration constant*/
	char cCalibConstant[32]="0.00";
	size    = (DWORD) sizeof(cCalibConstant);;
	retCode = RegQueryValueEx(petBoxRegKey, "DefaultCalibrationConstant",    NULL, NULL, (LPBYTE)&cCalibConstant,       &size);
	if (retCode == 0) defaultCalibrationConstant = atof(cCalibConstant);
	if (retCode !=0 || defaultCalibrationConstant == 0){
		MessagePopup("Error", "Please set default calibration constant in Registry");
		RegSetValueEx(petBoxRegKey, "DefaultCalibrationConstant",        0, REG_SZ,      (BYTE*) cCalibConstant,    size);
		return 8;
	}
		
	
	/* determine the locale */
	char localeChar[3]="";
	size = 3;
	RegQueryValueEx(petBoxRegKey, "Locale",            NULL, NULL, (LPBYTE)localeChar,       &size);
	
	for (int i=0; i<maxLocale; i++){
		if (0==strcmp(localeChar, localeArray[i]->id)) locale=i;
	}
	
	return retCode;
}

bool checkForConversionKey(char *convKey){
	DWORD size      = 1;
	bool conversion = 0;

	RegQueryValueEx(petBoxRegKey, convKey,       NULL, NULL, (LPBYTE)&conversion,       &size);
	return conversion ;
}


static int  readPreferences(void){
	DWORD size;
	int retCode=0;

	size    = MAX_PROTOCOL_NAME_LENGTH-1;
	retCode += RegQueryValueEx(petBoxRegKey, "IPAddress",   NULL, NULL, (LPBYTE) ipAddress, &size);	

	size    = MAX_PATHNAME_LEN-1;
	retCode += RegQueryValueEx(petBoxRegKey, "Institution",   NULL, NULL, (LPBYTE)institutionName, &size);	

	size    = MAX_PATHNAME_LEN-1;
	retCode += RegQueryValueEx(petBoxRegKey, "DICOMViewer",   NULL, NULL, (LPBYTE)DICOMViewer, &size);	

	size = 2;
	retCode += RegQueryValueEx(petBoxRegKey, "PortNumber",  NULL, NULL, (LPBYTE)&portNumber, &size);
	
	size    = MAX_PATHNAME_LEN-1;
	retCode += RegQueryValueEx(petBoxRegKey, "studyFolder", NULL, NULL, (LPBYTE)studyFolder, &size);	
	
	size    = MAX_PATHNAME_LEN-1;
	retCode += RegQueryValueEx(petBoxRegKey, "protFolder",  NULL, NULL, (LPBYTE)protFolder,  &size);	
	
	size    = MAX_PATHNAME_LEN-1;
	retCode += RegQueryValueEx(petBoxRegKey, "normFolder",  NULL, NULL, (LPBYTE)normFolder,  &size);	
	
	size    = MAX_PATHNAME_LEN-1;
	retCode += RegQueryValueEx(petBoxRegKey, "pMatrix",     NULL, NULL, (LPBYTE)pMatrix,     &size);	
	
	size    = 1;
	retCode += RegQueryValueEx(petBoxRegKey, "Trace",       NULL, NULL, (LPBYTE)&trace,       &size);

	size    = 4;
	retCode += RegQueryValueEx(petBoxRegKey, "GlobalSequenceNumber",       NULL, NULL, (LPBYTE)&globalSequenceNumber,       &size);
	
	if (retCode){
		writeToLog("preferenceManagement::readPreferences - Error reading registry");
	}
	
	return retCode;
}

void setDefaultPreferences(void){
	// Other (than camera settings) preferences
	myStrcpy(ipAddress, "127.0.0.1");
	portNumber = 65535;
	sideCamSerialNumber = 0;
	trace = 0;

	sprintfWithLock(MAX_PATHNAME_LEN, studyFolder,  "%s\\Studies",           appDataDirectory);
 	sprintfWithLock(MAX_PATHNAME_LEN, protFolder,   "%s\\Protocols",         appDataDirectory);
	sprintfWithLock(MAX_PATHNAME_LEN, normFolder,   "%s\\NormalizationFiles",appDataDirectory);
};

/*
	Create default values in the registry
*/
static void registryInitialization(){
	
	int retCode=0;
	struct cameraSettings cam;
	float fValue;
	struct isotope isotopeTable[] ={
		"18F",       109.71,    0.9673,
		"11C",        20.39,    0.998, 
		"64Cu",      762.00,    0.174,
		"68Ge",   389952.00,    0.891,
		"124I",     6013.44,    0.228
	};
	
	
	// Camera views settings
	cam.shutter             = 50;
	cam.gain                = 20;
	cam.brightness          = 50;
	cam.autoShutter         = 1;
	cam.autoGain            = 1;
	cam.autoBrightness      = 1;
	cam.camMode.mode        = 0;
	cam.camMode.pixFormat   = FC2_PIXEL_FORMAT_MONO8;    
	cam.camMode.Hunit       = 4;
	cam.camMode.Vunit       = 4;
	cam.camMode.maxWidth    = 640;
	cam.camMode.maxHeight   = 480;
	cam.displayArea.left    = 0;
	cam.displayArea.top     = 0;
	cam.displayArea.width   = 640;
	cam.displayArea.height  = 480;
	cam.frameRate           = 30;
	
	myStrcpy(cam.name, "Side");
	retCode += RegSetValueEx(petBoxRegKey, "SideView",    0, REG_BINARY, (BYTE*)&cam,      (DWORD)sizeof(cam));
	
	cam.frameRate           = 60;
	myStrcpy(cam.name, "Mon");
	retCode = RegSetValueEx(petBoxRegKey, "sideView",     0, REG_BINARY, (BYTE*)&cam,      (DWORD)sizeof(cam));

	sideCamSerialNumber = 0;
	retCode = RegSetValueEx(petBoxRegKey, "SideCamSerialNum", 0, REG_DWORD,  (BYTE*)&sideCamSerialNumber,  4);
	
	// Miscellaneous
	fValue  = 0;
	retCode += RegSetValueEx(petBoxRegKey, "IsotopeList",      0, REG_BINARY, (BYTE*)&isotopeTable,      (DWORD) sizeof(isotopeTable));
	retCode += RegSetValueEx(petBoxRegKey, "RecentProtocols",  0, REG_SZ    , (BYTE*)"!!!",             4);
	retCode += RegSetValueEx(petBoxRegKey, "GlobalSequenceNumber",  0, REG_DWORD,  (LPBYTE)&globalSequenceNumber,   4);
	retCode += RegSetValueEx(petBoxRegKey, "Institution",      0, REG_SZ    , (BYTE*)"Unknown",             8);
	retCode += RegSetValueEx(petBoxRegKey, "DICOMViewer",      0, REG_SZ    , (BYTE*)"",                    1);
	retCode += RegSetValueEx(petBoxRegKey, "PMatrix",          0, REG_SZ    , (BYTE*)pMatrix,              (DWORD) strlen(pMatrix)+1);
 	retCode += RegSetValueEx(petBoxRegKey, "IPAddress",        0, REG_SZ,      (BYTE*) ipAddress,                     16);
	retCode += RegSetValueEx(petBoxRegKey, "PortNumber",       0, REG_BINARY,  (BYTE*)&portNumber,                     2);
	retCode += RegSetValueEx(petBoxRegKey, "studyFolder",      0, REG_SZ,      (BYTE*)studyFolder,  (DWORD) (1+strlen(studyFolder))  );
	retCode += RegSetValueEx(petBoxRegKey, "protFolder",       0, REG_SZ,      (BYTE*)protFolder,   (DWORD) (1+strlen(protFolder))   );
	retCode += RegSetValueEx(petBoxRegKey, "normFolder",       0, REG_SZ,      (BYTE*)normFolder,   (DWORD) (1+strlen(normFolder))   );
	retCode += RegSetValueEx(petBoxRegKey, "Trace",            0, REG_BINARY,  (BYTE*)&trace,                          1);

	if (retCode){
		writeToLog("registryInitialization - Error writing registry");
	}
}


