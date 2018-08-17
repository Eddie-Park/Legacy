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


// @@@@@ USE STANDARD CVI COMPILER (64 bits)

// Builds


// 1292 - Minor changes to peek window (clip fields, acq.time below admin. time)
// 1291 - Changed study data display to increase precision on assay activity
// 1290 - Crop image for respiration monitoring: row 101-400, cols 51-550  -> (300x500) i.e. 50% reduction in bandwitdh
//        Records movie only in physics mode
// 1289 - Added debugging info to cameraManagement::openView.  Also, ignores image consistency error at grabber thread startup.
//        Added an invokation to PGRIdleStateFix at GUI startup to disable CPU idle states
// 1288 - Restructuration of runManagement::reconstruction to begin to introduce atlas program.  Requires DICOM 1009.
// 1287 - Recompile to accomodate new xraydeamon.h that came with xray daemon 0921
// 1286 - Changed skewness threshold value to 0.5 and breathing duty cycle to 0.180s
// 1285 - Save breathing history for research purposes
// 1284 - Fixed the scrollbar on protocols
// 1283 - Added build number on main display
//        Latest recon program (low count fix)
//        Added lock to isYoungEnough()
// 1282 - Added "Show Recent"/"Show All" in study tree
// 1281 - Minor changes to put image in transparent buttons
// 1280 - Introduce "default" protocol (in physics mode only)
//        Camera settings available only in physics mode
//        Updates in breathing monitoring algorithm and display
// 1279 -
// 1278 - Color palette changes; minor functional changes to user mode.
// 1277 - Mainly documentation additions along with some code cleanup,
//        and minor bug fix.
// 1276 - Major changes to include dose assay time in addition to administration time.
//        Works with new hist program (1229) and new DICOM program (1006)
//        Includes a separate conversion program for files studyData.txt and taskList.bin. 
//        The program is launched automatically (and transparently) at startup if needed
//        (see conversion1276 folder at the same level as the GENISYS4 GUI folder)
// 1275 - Reinstalled FlyCapture 2.1.3.5 x64
//        Zapped FlyCapture2.dll to call 'flyiomp5md.dll' instead of 'libiomp5md.dll'
//        (see program in dllZap sub folder in the main GENISYS4 GUI folder)
// 1274 -
// 1273 - Fixed a bug in readLatestValues
// 1272 - Fixed the branching fraction on initial isotope list
//        Investigate weird behavior with breathing monitoring (with Genysis computer, works fine with mine ...?)
//        Changed initialization of buffers (for loop instead of memset)
//        Breathing Monitoring now uses actual recorded time stamps (instead of nominal interval between samples)
// 1271 - 
// 1270 - Background color change in isotope table 
// 1269 - Implements branching fraction for isotopes (!)
// 1268 - Fixed bug with study edit (if stray clicks intervene) with the introduction of newStudyParentIndex
//        New breathing monitoring algorithm.
//        Camera consistency errors treated differently
//        Increased the number of local buffers from 600 to 800 (grabber thread in monitor management)

// 1267 - New parameter settings for the Derivative of a Gaussian filter (23,8) [ was (45,16) ]
// 1266 - (new version number only)
// 1265 - Monitor management now records a short 10sec movie for further analysis
//        Modified algorithm due to new findings
// 1264 - Activated breathing monitoring with new algorithm
// 1263 - Now the 'view' option is offered when right-clicking on the study name
// 1262 - Fixed: when running hist only, xray was attempted
//        Fixed bug with missing tag when updating study (possibly causing all columns green and yellow);
// 1261 - Add [temporary] OK button for xray acquisition until I get the right signal.
//        Removed the button (!)
//        Added copy of files.
// 1260 - 
// 1259 - New PMatrix key in registry; 
//        activated recording of breathing signal;
//        build id is now from _TARGET_FILE_VERSION_
// 1258 - ...
// 1257 - Continuation of xray daemon
// 1256 : Introducing XRAy Daemon
// 1255 : Barchart with more bars and colors / Study data display with dose and units	
// 1254D  Demo version
// 1254 : Repackaging of communications with xray server.
// 1253 : Changed acq recovery: do not fail hist/recon if acquiring
// 1252 : check mark BG color, LED display maximum changed
// 1251 : Fixed .im3 sequence number with new format 'm' in sprintfWithLock
// 1250 : Changed the breathing frequqnecy to text control
// 1249 : Added the more/less arrow on protocol edit.
// 1248 : Changed hour,min,sec display in acquisition panel from numeric to text
// 1247 : User edit protocol in separate UI file
// 1246 : Removed hist and recon rescheduling.  New camera settings panel.
// 1245 : Camera stuff completed.  Next: breathing signal (when we have the real camera)
// 1244 : fixed camera adjustments (using valueA instead of absolute value)
// 1243 : .. continuation of camera work. 
// 1242 : .. continuation of camera work. (note: 64 bits changed the length of the task list)
// 1241 : Now using FlyCapture 2.1 Release 5 
// 1240 : first 64 bits version (includes PGR)
// (...)
// 1236 - Numerous appearance changes, embryo of xray daemon communications
// (...)
// 1225 - new hist call
// 1224 - Enters "rename" mode after creating new directory ...
// 1223 - Tags implemented for protocols.  Nicer protocol: new dir , rename and delete.
// 1222 - TAG implemented for studies (in struct studyData) Started with protocol tags ...
// 1221 - introducing study TAG as a permanent and unique identifier
// 1220 -
// 1219 - 
// 1218 - Cleanup protocol versions and reintroduce a simpler physics mode
// 1217 - First version for Genisys 4

#include "windows.h"
#include "toolbox.h"
#include "animate.h"
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "genisys4GUI.h"
#include "utilities.h"
#include "logManagement.h"

#include "globalDefs.h"

#include "preferenceManagement.h"
#include "studyTreeManagement.h"
#include "menuManagement.h"
#include "protocolTreeManagement.h"
#include "protocolEdit.h"
#include "cameraManagement.h"
#include "acquisitionManagement.h"
#include "runManagement.h"
#include "monitorManagement.h"
#include "communicationsManagement.h"
#include "logManagement.h"
#include "scriptManagement.h"
#include "export.h"
#include "xrayDaemonManagement.h"
#include "studyEdit.h"


// STUDY-PROTOCOL TABS
#define INDEX_STUDY_TAB      0
#define INDEX_PROTOCOL_TAB   1

// SIDE TABS	 (manually maintained)
#define INDEX_ACQPANEL_TAB   0
#define INDEX_IDLE_TAB       1
#define INDEX_PROTEDIT_TAB   2
#define INDEX_ANIMATION_TAB  3
#define INDEX_ABOUT_TAB      4
#define INDEX_CAMERA_TAB     5
#define INDEX_STUDYEDIT_TAB  6  
#define INDEX_PET_TAB        7  
#define INDEX_XRAY_TAB       8  

DeclareThreadSafeScalarVar (int, ProtocolHandlerThreadCounter);

// Local functions
static int GUIPreInitialization(void);
static int  GUIInitialization(char* argv[]);
static void GUITermination(void);

static char uirFileName[100];

static int topSplash;
static int leftSplash;

/*
	This function displays the splash screen and animates the wait icon
	It has to run in its own thread otherwise no initialization
	will run.
	
	Also, because the panel is loaded in that thread, it works.
											
*/
int CVICALLBACK theSplasher (void *functionData){
	int splashPanel;
	int count;
	int i=0;
	
	if ((splashPanel  = LoadPanel (0, "genisys4GUI.uir", SPLASH_PNL)) < 0) return -1;
	char temp[64];
	sprintf(temp, "Version 2.0 Build %s", _TARGET_FILE_VERSION_ );
	SetCtrlVal(splashPanel, SPLASH_PNL_versionID, temp);
	
	SetPanelAttribute(splashPanel, ATTR_TOP,  topSplash);
	SetPanelAttribute(splashPanel, ATTR_LEFT, leftSplash);
	
	DisplayPanel(splashPanel);
	GetNumListItems(splashPanel, SPLASH_PNL_WAIT_CTRL, &count);
	
	while(splash){
		SetCtrlVal(splashPanel, SPLASH_PNL_WAIT_CTRL, i++);
		if (i>=count) i=0;
		Delay(0.080);
	}
	
	DiscardPanel(splashPanel);
	return 0;
}
				
void resetPETBox(void){
	// Delete registry
	RegDeleteKey(HKEY_CURRENT_USER, PET_BOX_REG_KEY);
}

LONG CALLBACK theExceptionHandler(PEXCEPTION_POINTERS exc){
	
	DWORD theCode = exc->ExceptionRecord->ExceptionCode ;
	if (logFile){
		fprintf(logFile, "EXCEPTION HANDLER - code %08X\n", theCode); 
		fflush(logFile);
//		fclose(logFile);
	}
 
	return 0;
}

//

static void conditionalFileConversion(char* convKey){
	char temp[64];
	int convHandle;
	int retCode;
	
	if ( 0 == checkForConversionKey(convKey)){
		sprintfWithLock(64, temp,"Converting files (%s)", convKey);
		writeToLog(temp);
		sprintfWithLock(64, temp,"%s.exe",convKey);
		retCode = LaunchExecutableEx(temp, LE_HIDE, &convHandle);
		if (retCode == 0){
			while ( 0 == ExecutableHasTerminated(convHandle)) Delay(1.0);
		}  else {
			writeToLog("ERROR Launching conversion program");
		}
		RetireExecutableHandle (convHandle);
	}
	
}


// -------------------------------------- MAIN --------------------------------------------

int main (int argc, char *argv[]){ 
	
	AddVectoredExceptionHandler(0, theExceptionHandler);

	int theSplasherFunctionID;
	int keyModifier;
	int rc;
	
	// Make sure only one Genisys is running
	int thereIsAnother;
	if (CheckForDuplicateAppInstance (ACTIVATE_OTHER_INSTANCE, &thereIsAnother) < 0) return -1; 
	if (thereIsAnother) return 0; /* prevent duplicate instance */

	/* INITIALIZATION SECTION */
	
	GetGlobalMouseState(NULL, NULL, NULL, NULL, NULL, &keyModifier);
	if (keyModifier == VAL_MENUKEY_MODIFIER)
		if(ConfirmPopup("Confirm Reset","Are you sure you want to reset all PETBox parameters?"))
			resetPETBox();

	
	if ( 0!= CmtNewLock(NULL, 0, &sprintfLock)){
		MessagePopup("Error","Main - Can't create sprintf lock");
		return -1;
	}
	
	GUIPreInitialization();
	
	splash = 1;
 	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, theSplasher, 0, &theSplasherFunctionID);
	
	if (preferenceManagementInitialization() != 0){
		splash=0;
		MessagePopup("Error", "Error while initializing preferences.\nAborting.");
		return -1;
	}
	
	
	if (0 != logManagementInitialization()){
		splash=0;
		MessagePopup("Error", "Error while initializing log file.\nAborting.");
		return -1;
	}
 	
	// Check for possible conversion requirements (after pref management init)
	conditionalFileConversion("conversion1276");
	
	// Disable CPU idle states for faster frame grabbing
	int pgrHandle;
	rc = LaunchExecutableEx("PGRIdleStateFix.exe disable", LE_HIDE, &pgrHandle);
	if (rc !=0 ) {
		writeToLog("main - PGRIdleStateFix launch failed; ignored.");
	}
	else{
		writeToLog("main - PGRIdleStateFix launch successful.");
		while ( 0 == ExecutableHasTerminated(pgrHandle)) Delay(0.050);
		RetireExecutableHandle(pgrHandle);
	}
	
	traceToLog("* calling communicationsInitialization");
	if ( 0!= communicationsInitialization()){   
	// do nothing, we'll work locally
	}

	traceToLog("* calling GUIInitialization");
	if (GUIInitialization(argv) !=0){
		splash=0;
		MessagePopup("Error", "Error while initializing User Interface.\nAborting.");
		return -1;
	}

	/* Initialize each "manager" */
	traceToLog("* calling monitorManagementInitialization");
	if ( 0!= monitorManagementInitialization()){
		splash=0;
		MessagePopup("Error", "Error while initializing monitor management.\nAborting.");
		return -1;
	}
	traceToLog("* calling acquisitionManagementInitialization");
	if ( 0!= acquisitionManagementInitialization()){
		splash=0;
		MessagePopup("Error", "Error while initializing acquisition.\nAborting.");
		return -1;
	}
	traceToLog("* calling protocolEditInitialization");
	if ( 0!= protocolEditInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing protocol edition.\nAborting.");
		return -1;
	}
	
	traceToLog("* calling studyEditInitialization");
	if ( 0!= studyEditInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing study edition.\nAborting.");
		return -1;
	}

	
	traceToLog("* calling studyTreeManagementInitialization");
	if ( 0!= studyTreeManagementInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while gathering info about studies.\nAborting.");
		return -1;
	}
	traceToLog("* calling protocolTreeManagementInitialization");
	if ( 0!= protocolTreeManagementInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while gathering info about protocols.\nAborting.");
		return -1;
	}

	traceToLog("* calling menuManagementInitialization");
	if ( 0!= menuManagementInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing menus.\nAborting.");
		return -1;
	}
	traceToLog("* calling cameraInitialization");
	if ( 0!= cameraInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing cameras.\nAborting.");
		return -1;
	}
	
	///traceToLog("* calling communicationsInitialization");
	///if ( 0!= communicationsInitialization()){   
	// do nothing, we'll work locally
	///}
	
	traceToLog("* calling xrayDaemonInitialization");
	if ( 0!= xrayDaemonInitialization()){   
	// do nothing, we'll work locally
	}

	traceToLog("* calling GUIInitialization");
	if ( 0!= runManagementInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing run manager.\nAborting.");
		return -1;
	}
	
	traceToLog("* calling scriptManagementInitialization");
	if ( 0!= scriptManagementInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing script manager.\nAborting.");
		return -1;
	}
	
	traceToLog("* calling exportInitialization");
	if ( 0!= exportInitialization()){   
		splash=0;
		MessagePopup("Error", "Error while initializing export manager.\nAborting.");
		return -1;
	}
	
	// Tell splasher to stop and wait for completion
	splash = 0;
	traceToLog("* waiting for splasher to end");
	CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE,theSplasherFunctionID, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
	traceToLog("* releasing thread pool");
	CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,theSplasherFunctionID);
 
	traceToLog("* setting Protocol TAB page attributes");
	SetTabPageAttribute (mainPanel,MAIN_PANEL_mainTAB, INDEX_PROTOCOL_TAB,  ATTR_DIMMED, 0);
	traceToLog("* setting Study    TAB page attributes");
	SetTabPageAttribute (mainPanel,MAIN_PANEL_mainTAB, INDEX_STUDY_TAB,     ATTR_DIMMED, 0);
 	
	/* RUN */
	SetWaitCursor(0);
	writeToLog("RunUserInterface");
	
	RunUserInterface ();
	
	/* 
		TERMINATION SECTION 
	
		Seems like a good idea to have a counterpart to each initialization
		call, that is a termination function, even if the function does
		nothing (for now), and to call then in reverse order to make sure any
		dependency is respected.
		
	*/
	
	exportTermination();
	scriptManagementTermination();
	runManagementTermination();
	xrayDaemonTermination();
	communicationsTermination();
	cameraTermination();
	studyEditTermination();
	protocolEditTermination();
	menuManagementTermination();
	protocolTreeManagementTermination();
	studyTreeManagementTermination();
	acquisitionManagementTermination();
	monitorManagementTermination();

	DiscardPanel (mainPanel);
	GUITermination();
	logManagementTermination();
	
	CmtDiscardLock(sprintfLock);

	return 0;
}


// --------------------------------------- GUI Initialization -----------------------------------------------

static int GUIPreInitialization(void){
	
	SetWaitCursor(1);
	
	// Determine User or Physics mode
	unsigned int size;
	RegReadBinary (0x80000001, PET_BOX_REG_KEY, "PhysicsMode", (unsigned char*) &physicsMode, sizeof(physicsMode), &size); 
	if (physicsMode) strcpy(uirFileName, "genisys4GUI.uir");  
	else             strcpy(uirFileName, "genisys4GUIUser.uir");  

	if ( (mainPanel              = LoadPanel (0, uirFileName, MAIN_PANEL)) < 0)	return 8;
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_IDLE_TAB, &idleTabHandle)){   
		return 8;
	}

	char temp[32];
	sprintf(temp, "Build %s", _TARGET_FILE_VERSION_ );
	SetCtrlVal(idleTabHandle, IDLE_PANEL_build, temp);

	// make things nice
	int retCode=0;
	retCode += SetCtrlAttribute(mainPanel, MAIN_PANEL_sideTAB, ATTR_CTRL_MODE, VAL_INDICATOR);
	retCode += SetCtrlAttribute(mainPanel, MAIN_PANEL_sideTAB, ATTR_TABS_VISIBLE, 0); 
	retCode += SetCtrlAttribute(mainPanel, MAIN_PANEL_sideTAB, ATTR_LEFT,        -1); 
	retCode += SetCtrlAttribute(mainPanel, MAIN_PANEL_sideTAB, ATTR_TOP,        -18); 
	retCode += SetActiveTabPage(mainPanel, MAIN_PANEL_sideTAB, INDEX_IDLE_TAB);
	
	
	retCode += SetCtrlAttribute(mainPanel, MAIN_PANEL_scriptBanner, ATTR_TOP, 17);
//	retCode += SetCtrlAttribute(mainPanel, MAIN_PANEL_MAIN_BANNER,  ATTR_LEFT, 0);
															
	retCode += DisplayPanel (mainPanel);
	
	
	// Determine the location of the splash panel
	int leftPanel, topPanel;
			  
	retCode += GetCtrlAttribute(idleTabHandle, IDLE_PANEL_NOT_CONNECTED, ATTR_LEFT, &leftSplash);
	retCode += GetCtrlAttribute(idleTabHandle, IDLE_PANEL_NOT_CONNECTED, ATTR_TOP,  &topSplash);
	
	GetPanelAttribute(mainPanel, ATTR_LEFT, &leftPanel);
	GetPanelAttribute(mainPanel, ATTR_TOP, &topPanel);
 	
	leftSplash+=leftPanel;
	topSplash +=topPanel;

	GetCtrlAttribute(mainPanel, MAIN_PANEL_sideTAB, ATTR_LEFT, &leftPanel);
	GetCtrlAttribute(mainPanel, MAIN_PANEL_sideTAB, ATTR_TOP,  &topPanel);
	
	leftSplash+=leftPanel;
	topSplash +=topPanel;
	
	return retCode;
	
}

static int GUIInitialization(char* argv[]){
	int retCode;
	
	//successColor   = MakeColor(30,   28,   17);  
	//scheduledColor = MakeColor(255, 200,    0);  
	//runningColor   = MakeColor(0,   255,    0);
	//errorColor     = MakeColor(255,   0,    0); 
	
	successColor   = MakeColor(255, 255,  255);  
	scheduledColor = MakeColor(255, 200,    0);  
	runningColor   = MakeColor(0,   255,    0);
	errorColor     = MakeColor(255, 255,  255); 
	grayColor      = MakeColor( 48,  48,   48); 
	redColor       = MakeColor(255,   0,    0); 
	sofieBlue      = MakeColor(38, 178,  237);           // Sofie Blue   
	
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_mainTAB, INDEX_STUDY_TAB, &studyTabHandle)){   
		writeToLog("GUIInitialization - Error getting study tab handle");
		return 8;
	}
	
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_mainTAB, INDEX_PROTOCOL_TAB, &protocolTabHandle)){   
		writeToLog("GUIInitialization - Error getting protocol tab handle");
		return 8;
	}
	
	// Bug getaround
	//   for some reason, the protocol tab height is not what is set in the GUI (?)
	int tabHeight;
	GetCtrlAttribute(studyTabHandle,    TAB_STUDY_TREE,  ATTR_HEIGHT, &tabHeight);  
	SetCtrlAttribute(protocolTabHandle, TAB_PROT_TREE,   ATTR_HEIGHT, tabHeight);  

	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_ACQPANEL_TAB, &acquisitionPanel)){
		writeToLog("GUIInitialization - Error getting acquisition panel handle");
		return 8;
	}
	
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_PROTEDIT_TAB, &protocolEditPanel)){   
		writeToLog("GUIInitialization - Error getting protocol edit panel handle");
		return 8;
	}
	
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_ANIMATION_TAB, &animationPanel)){   
		writeToLog("GUIInitialization - Error getting animation panel handle");
		return 8;
	}
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_ABOUT_TAB, &aboutPanel)){   
		writeToLog("GUIInitialization - Error getting about panel handle");
		return 8;
	}
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_CAMERA_TAB, &camSettingsPanel)){   
		writeToLog("GUIInitialization - Error getting camera settings panel handle");
		return 8;
	}
 
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_STUDYEDIT_TAB, &studyEditPanel)){   
		writeToLog("GUIInitialization - Error getting study edit panel handle");
		return 8;
	}

	/*
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_USERPROT_TAB, &userProtPanel)){   
		writeToLog("GUIInitialization - Error getting user prot panel handle");
		return 8;
	}
	*/
	
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_PET_TAB, &confirmationPanel)){   
		writeToLog("GUIInitialization - Error getting confirmation panel handle");
		return 8;
	}
	
	if (0>GetPanelHandleFromTabPage(mainPanel,MAIN_PANEL_sideTAB, INDEX_XRAY_TAB, &xrayPanel)){   
		writeToLog("GUIInitialization - Error getting xray panel handle");
		return 8;
	}
	SetCtrlAttribute(xrayPanel,   TAB_XRAY_OK_BUTTON,     ATTR_VISIBLE, 0);

	SetCtrlVal(studyEditPanel, STUDY_EDIT_ACT_UNITS, localeArray[locale]->activityUnits);
	
	if ((isotopePanel           = LoadPanel (protocolEditPanel, uirFileName, ISOT_PANEL)) < 0) {
		writeToLog("GUIInitialization - Error loading isotope panel");
		return 8;
	}
	
	if ((exportStudyPanel        = LoadPanel (0, uirFileName, EXPORT_PNL)) < 0) {
		writeToLog("GUIInitialization - Error loading export study panel");
		return 8;
	}
 	
	if ((peekPanel        = LoadPanel (0, uirFileName, PEEK_PANEL)) < 0) {
		writeToLog("GUIInitialization - Error loading study peek panel");
		return 8;
	}
 
	/* Put background images in transparent buttons */
	// Do this before fading panels so that everything is visually as expected *)
	putImageInTransparentButton(acquisitionPanel,   ACQ_PANEL_STOP_BUTTON);
	putImageInTransparentButton(protocolEditPanel,  PROT_EDIT_SAVE_BUTTON);
	putImageInTransparentButton(protocolEditPanel,  PROT_EDIT_CANCEL_BUTTON);
	
	putImageInTransparentButton(studyEditPanel,     STUDY_EDIT_SAVE_BUTTON);
	putImageInTransparentButton(studyEditPanel,     STUDY_EDIT_CANCEL_BUTTON);
	SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_LEFT,     130);  // Move save button out of place
	SetCtrlAttribute(studyEditPanel, STUDY_EDIT_SAVE_BUTTON, ATTR_VISIBLE,    0);  //  .. and hide
	SetCtrlAttribute(studyEditPanel, STUDY_EDIT_saveMask,    ATTR_LEFT,     200);  // Move mask on top of save button image 
		
	putImageInTransparentButton(confirmationPanel,  TAB_PET_PROCEED_BUTTON);
	putImageInTransparentButton(confirmationPanel,  TAB_PET_CANCEL_BUTTON);
	
	putImageInTransparentButton(xrayPanel,          TAB_XRAY_CANCEL_BUTTON);
	putImageInTransparentButton(camSettingsPanel,   CAM_PANEL_DONE_BUTTON);
	

	/* Fading Panels */
	retCode  = 0;
	retCode += fadingPanelInitialization(&editPanelFading,      10, protocolEditPanel );
	retCode += fadingPanelInitialization(&acqPanelFading,       10, acquisitionPanel  );
	retCode += fadingPanelInitialization(&aboutPanelFading,     10, aboutPanel        );
	retCode += fadingPanelInitialization(&camSetPanelFading,    10, camSettingsPanel  );
	retCode += fadingPanelInitialization(&studyEditPanelFading, 10, studyEditPanel    );
	retCode += fadingPanelInitialization(&confirmPanelFading,   10, confirmationPanel );
	retCode += fadingPanelInitialization(&xrayPanelFading,      10, xrayPanel );
		
	// If something did not work out during communications initialization, attempt reconnection regularly
	if ( TCPServerHandle <0 || xrayDaemonHandle <0){
		writeToLog("genisys4GUI::GUIInitialization - Enabling reconnection timer");
		SetCtrlAttribute( idleTabHandle , IDLE_PANEL_reconTimer, ATTR_ENABLED, 1); // Schedule automatic reconnection
	}

	
	return retCode;
	
}

static void  GUITermination(void){
	fadingPanelTermination(&editPanelFading);
	fadingPanelTermination(&acqPanelFading);
	writeToLog("PETBox GUI Terminated");
}

/*
	Quit callback from close box
*/
int CVICALLBACK quitButton_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if ( 0<GetProtocolHandlerThreadCounter() ){
				if( !ConfirmPopup("Exit Confirm", "There are protocols running. Do you want\nto exit and kill those tasks?")) 
					return 0;
			}
			shutdown();
			break;
	}
	return 0;
}

int CVICALLBACK about_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			fadeInPanel(&aboutPanelFading);
			break;
	}
	return 0;
}

int CVICALLBACK closeAbout_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			fadeOutPanel(&aboutPanelFading);
			break;
	}
	return 0;
}
