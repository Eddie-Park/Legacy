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

#include "constants.h"
#include "typedefs.h"
#include "FlyCapture2_C.h"
#include "commonHeader.h"


//#undef _TARGET_FILE_VERSION_
//#define _TARGET_FILE_VERSION_ "1.2.7.8"

// Registry key
HKEY petBoxRegKey;
int dicomRegID =    114469;                         // DICOM registration number for Sofie
int productID  =   1010004;                        // Genisys Product number.  Genisy4
int globalSequenceNumber=0;                        // Global sequence number used for creating unique DICOM file IDs (makeUniqueFileID)

/* default values for protocols */
unsigned char defaultHistOptions;				   // Default histogramming options (from GUI user panel)
unsigned short int defaultIterations;			   // Default number of MLEM iterations (from GUI user panel)
int defaultBreathingMonitoring;					   // Default check box value for breathing monnitoring
int defaultXray;								   // Default check box value for xray
float  defaultCalibrationConstant;				   // yes, the default calibration constant (from registry)

FILE *logFile=NULL;								   // The log file pointer
CmtThreadLockHandle sprintfLock=0;				   // A global lock for non-reentrant C functions like sprintf, sscanf, memcpy, etc 
CmtThreadLockHandle acquisitionLock=0;			   // A global lock acquired when acquisition is running
CmtThreadLockHandle scriptingLock=0;			   // A global lock acquired when a script is running

// Panel handles

int splash;										   // The handle to the splah panel (at startup)
int mainPanel;									   // The handle to the main panel
int acquisitionPanel;							   // Handle to acquisition tab (panel)
int protocolEditPanel;	     					   // Handle to protocol edit tab (panel)
int isotopePanel;								   // Handle to the small isotope edit/display panel
int aboutPanel;  								   // Handle to the about tab
int studyTabHandle;								   // Handle to the study tree tab
int protocolTabHandle;							   // Handle to the protocol tree tab 
int idleTabHandle;								   // Handle to the idle tab
int animationPanel;								   // Handle to the tab (panel) used to fade in/out
int confirmationPanel;                             // Handle to the PET acquisition confirmation tab
int xrayPanel;									   // Handle to the Xray acquisition confirmation tab
int camSettingsPanel;							   // Handle to the camera settings tab
int studyEditPanel;								   // Handle to the sudy edit tab
int userProtPanel;
int exportStudyPanel;
int peekPanel;									   // Handle to the small yellow window for a quick display (peek) of study data
												   
int acquisitionConfirmation;					   // Acquisition confirmation flag (0-none requested, 1-requested)  
int answerGiven;

// This is for the "in-progress" wheel while waiting for a file copy operation
int waitTickCounter;
int waitImageCounter;

int mainMenuBarHandle;
int mainMenuBarSettingsMenu;
int settingsMenuPrefItem;
int settingsMenuCameraItem;
int settingsMenuScriptItem;
int settingsMenuExitItem;

volatile bool breathingMonitoringRequested=0;			// flag indicating the wish for the thread ro run
volatile bool breathingMonitoringThreadRunning=0;		// flag indicating if the actual thread running
volatile bool acquisitionRequested=0;
volatile bool acquisitionRunning=0;

bool trace=0;									   // Trace mode flag (1-on, 0-off).  In trace mode, mode info is written to the log 
bool physicsMode=0;								   // Physics mode flag (1:on, 0:off).  In physics mode, more protocol options are available 


/* Fading Panels */
struct fadingPanel editPanelFading;				   // Structure to allow the fade in/out when displaying the protocol edit panel
struct fadingPanel acqPanelFading;
struct fadingPanel aboutPanelFading;
struct fadingPanel prefPanelFading;
struct fadingPanel camSetPanelFading;
struct fadingPanel confirmPanelFading;
struct fadingPanel xrayPanelFading;
struct fadingPanel studyEditPanelFading;

// Fancy Controls
struct LEDControl   promptsRateMeter; 			   // Structure describing the LED prompts rate meter

// injection time field (study panel) state machine
enum enumTimeStates timeState;

// Variables to support the frame sequence validation (state machine)
enum action string_CB_retVal;
int  mustReplay=0;

/* TCP Communications variables */
volatile int TCPServerHandle  = -1;                // PET Daemon handle on the embedded PC side.  NOTE: zero is a valid handle; -1 means not connected
volatile int xrayDaemonHandle = -1;				   // XRAY Daemon handle on the embedded PC side
volatile int startAckReceived;
volatile int inquireAckReceived;
struct structInquireAcknowledge serverStatus;		   // Server (embedded PC) status at startup
struct structUnsollicited       unsocStats;            // Unsollicited statistics sent during acquisition

/* Study TAB POPUP menu variables */
int studyBarHandle;
int studyTabPopupMenu;

int otherMenuBarHandle;
int reconnectMenu;
int reconnectMenuReconnectItem; 

struct protocolSubMenu* subMenuArray;
int    subMenuCount;

int runProtocolMenuID;
int studyPopNew;
int studyPopRename;
int studyPopShow;
int studyPopRun;
int studyPopEdit;
int studyPopArchive;
int studyPopDelete;
int studyPopView;

/* Protocol TAB POPUP menu variables */
int	protocolBarHandle;
int	protocolTabPopupMenu ;
int	protocolPopNewDir;
int protocolPopNewProt;
int	protocolPopEdit ;
int	protocolPopRename;
int	protocolPopDelete;

// These are in the Webdings character set (for the study tree columns)
char checkMark[] ="a";						       // ASCII character for the check mark (success)
char errorMark[] ="r";						       // ASCII character an "X" (error)
char circleMark[]="n";						       // ASCII character a circle (running and scheduled states)

// Preference variables (read from registry, except appData, which is an environment variable)
char appDataDirectory [MAX_PATHNAME_LEN];  		   // Application data directory name (full path)
char studyFolder      [MAX_PATHNAME_LEN];		   // Study folder dir name
char protFolder       [MAX_PATHNAME_LEN]; 		   // Protocol folder dir name
char normFolder       [MAX_PATHNAME_LEN]; 		   // System files (formerly normalization files) folder
char ipAddress        [MAX_PROTOCOL_NAME_LENGTH];  // The static IP address of the embedded PC
char institutionName  [MAX_PATHNAME_LEN]; 		   // Customer's institution name
char DICOMViewer      [MAX_PATHNAME_LEN]=""; 	   // full path to the executable handling the 'view' menu command (currently amide.exe) 
char pMatrix          [MAX_PATHNAME_LEN]="p.pmatrix";  // Name of the system matrix   

unsigned short int portNumber;
unsigned int sideCamSerialNumber;

int camSettingsAcquire;
int camSettingsBitMapDiscarded;
int displayLoopThreadFunctionID;
Rect *viewArea;

fc2Context contextSideCam;			               // The "context" structure for the Firefly camera
struct cameraSettings sideView;                    // a structure containing the (saved) camera settings for breathing monitoring (formerly side view)

Rect displayRect;		                           // This is the area displayed in the canvas in the display loop (possibly cropped)
Rect canvasRect;	                               // The actual (full) size of the canvas control to display an image
Rect croppedRect;	                               // The cropped rectangle (mon view)
int  colorTable[256];                              // The gray levels to display B&W images

// Recently used protocol stuff
int  numRecentProt;								   // Number of recently used protocols in the list
char *recentProtocols[MAX_RECENT_PROTOCOLS];	   // List of recently used protocols

int theBGColor, theColor;

// Definitions of colors that reflect the status of a task as displayed in the study tree pane
int successColor;	                               //  task has been completed with success
int scheduledColor;	                               //  task is scheduled to run 
int runningColor;		                           //  task is currently running
int errorColor;			                           //  an error has occured whit that task
int grayColor;			                           //  medium gray (for check marks)
int redColor;			                           //  red for error "X"
int sofieBlue;                                     // Sofie Blue 


// Where in the tree (study or protocol) was the last right click performed
Point lastRightClickPoint;						   // Actual coordinates of the last right-click
int lastIndex;	                                   // Row index of last right click (index is zero-based line number)
int newStudyParentIndex;                           // Index of the parent study when creating a new study
int lastArea;
int lastColumn;
int lastImageIndex;                                // level of the last item right-clicked  (0- top most, 1-category, 2-study or protocol)
char lastTag[MAX_TREE_ITEM_TAG_LEN];               // Tag of the last study that was clicked on
char oldName[MAX_PATHNAME_LEN];                    // saved, old item name in case of a rename operation
																
enum editReasons editProtocolPanelReason;          // The reason why this protocol is being edited
enum editReasons editStudyPanelReason;	           // The reason why this study is being edited
char editPanelTag[MAX_TREE_ITEM_TAG_LEN];          // 

int studyCategoryIndex;
int protocolCategoryIndex;

int maxStudyAge = 30;                              // Maximum study age (in days) allowed to be displayed in the study tree

// locale definitions: country or region identification, units for activity, multiplication factor to obtain Bq
int    locale=0;										       // current index in localeArray
struct locale USlocale={"US", "( \265Ci )", 37000.0};		   // Entry for United States,  uCi,  Bq = 37000 * uCi
struct locale EUlocale={"EU", "( kBq )",     1000.0};		   // Entry for European Union, kBq,  Bq =  1000 * kBq
struct locale* localeArray[]={&USlocale, &EUlocale};		   // The actual array of predefined entries
int    maxLocale = (int) (sizeof(localeArray)/sizeof(void*) ); // Size of ther array
