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

#ifndef globalDecls_H
#define globalDecls_H

#include "typedefs.h"
#include "FlyCapture2_C.h"
#include "stdio.h"

//#undef _TARGET_FILE_VERSION_
//#define _TARGET_FILE_VERSION_ "1.2.7.8"

// See "globalDefs.h" for definitions of those variables

extern HKEY petBoxRegKey;
extern int dicomRegID; 
extern int productID;    
extern int globalSequenceNumber;

extern unsigned char defaultHistOptions;
extern unsigned short int defaultIterations;
extern float  defaultCalibrationConstant;
extern int defaultBreathingMonitoring;					   
extern int defaultXray;							

extern FILE *logFile;
extern CmtThreadLockHandle sprintfLock;
extern CmtThreadLockHandle acquisitionLock;
extern CmtThreadLockHandle scriptingLock;

extern int mainPanel;
extern int acquisitionPanel;
extern int protocolEditPanel;
extern int isotopePanel;
extern int preferencesPanel;
extern int aboutPanel;
extern int studyTabHandle;
extern int idleTabHandle;
extern int animationPanel;
extern int confirmationPanel;
extern int xrayPanel;
extern int camSettingsPanel;
extern int studyEditPanel;
extern int userProtPanel;
extern int peekPanel;

extern int mainMenuBarHandle;
extern int mainMenuBarSettingsMenu;
extern int settingsMenuPrefItem;
extern int settingsMenuCameraItem;
extern int settingsMenuScriptItem;
extern int settingsMenuExitItem;

extern int acquisitionConfirmation;
extern int answerGiven;
extern int exportStudyPanel;

// This is for the "in-progress" wheel while waiting for a file copy operation
extern int waitTickCounter;
extern int waitImageCounter;

extern volatile bool breathingMonitoringRequested;
extern volatile bool breathingMonitoringThreadRunning;
extern volatile bool acquisitionRequested;
extern volatile bool acquisitionRunning;

extern bool trace;
extern bool physicsMode;

/* Variable size controls */
extern struct sizeableControl hours      ;
extern struct sizeableControl hourMinSep  ;
extern struct sizeableControl mins       ;
extern struct sizeableControl seconds    ;
extern struct sizeableControl minSecSep   ;
extern struct sizeableControl breathFreq ;
extern struct sizeableControl hourLabel   ;
extern struct sizeableControl minLabel   ;
extern struct sizeableControl secLabel   ;
extern struct sizeableControl freqLabel  ;
extern struct sizeableControl timeTitle  ;
extern struct sizeableControl freqTitle  ;

/* Fading Panels */
extern struct fadingPanel editPanelFading;
extern struct fadingPanel acqPanelFading;
extern struct fadingPanel aboutPanelFading;
extern struct fadingPanel camSetPanelFading;
extern struct fadingPanel confirmPanelFading;
extern struct fadingPanel xrayPanelFading;
extern struct fadingPanel studyEditPanelFading;

extern enum enumTimeStates timeState;
extern enum action string_CB_retVal;
extern int  mustReplay;

// Fancy Controls
extern struct LEDControl   promptsRateMeter; 

extern int studyBarHandle;
extern int runProtocolMenuID;
extern int studyPopNew;
extern int studyPopRename;
extern int studyPopShow;
extern int studyPopRun;
extern int studyPopEdit;
extern int studyPopArchive;
extern int studyPopDelete;
extern int studyPopView;

extern int studyTabHandle;
extern int protocolTabHandle;
extern int studyTabPopupMenu;


extern int otherMenuBarHandle;
extern int reconnectMenu;
extern int reconnectMenuReconnectItem; 


extern struct protocolSubMenu* subMenuArray;
extern int    subMenuCount;

/* TCP */
extern volatile int TCPServerHandle;
extern volatile int startAckReceived;
extern volatile int inquireAckReceived;
extern struct structInquireAcknowledge serverStatus;		   // Server (embedded PC) status at startup
extern struct structUnsollicited       unsocStats;            // Unsollicited statistics sent during acquisition

extern volatile int xrayDaemonHandle;

/* Protocol TAB POPUP menu variables */
extern int	protocolBarHandle;
extern int	protocolTabPopupMenu ;
extern int	protocolPopNewDir ;
extern int	protocolPopNewProt ;
extern int	protocolPopEdit ;
extern int	protocolPopRename;
extern int	protocolPopDelete;

// Preferences 
extern  char appDataDirectory [];  
extern  char studyFolder  [];
extern  char protFolder   []; 
extern  char normFolder   []; 
extern  char ipAddress    [];
extern  char institutionName  []; 
extern  char DICOMViewer  []; 
extern  char pMatrix      [];    

extern unsigned short int portNumber;
extern unsigned int sideCamSerialNumber;

extern int camSettingsAcquire;
extern int camSettingsBitMapDiscarded;
extern int displayLoopThreadFunctionID;

extern Rect displayRect;
extern Rect croppedRect;
extern Rect canvasRect;

extern fc2Context contextSideCam;

extern struct cameraSettings sideView;

extern Rect *viewArea;
extern int colorTable[256];

// Recent stuff
extern int   numRecentProt;
extern char *recentProtocols[];

extern  char checkMark    [];
extern  char errorMark    [];
extern  char circleMark    [];


extern int theColor;
extern int theBGColor;

extern int successColor;
extern int scheduledColor;
extern int runningColor;
extern int errorColor;
extern int grayColor;			                           
extern int redColor;			                         
extern int sofieBlue;

extern	Point lastRightClickPoint;
extern	int lastIndex;
extern  int newStudyParentIndex;
extern	int lastArea;
extern	int lastColumn;
extern	int lastImageIndex;
extern  char lastTag[];
extern  char oldName[];

extern int studyCategoryIndex;
extern int protocolCategoryIndex;
extern int maxStudyAge;

extern enum editReasons editProtocolPanelReason;
extern enum editReasons editStudyPanelReason;
extern char editPanelTag[];
extern int locale;
extern struct locale* localeArray[];
extern int maxLocale;


#endif
