
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
//*  Written by Shang Shang                                                     *
//*                                                                             *
//*  Created: May 2011                                                          *
//*                                                                             *
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************

// Global variables
FILE *logFile;   
FILE *rawDataFile; // file for the raw data 

HKEY  xrayRegKey;
HKEY  xrayAcqRegKey;

char systemFilesPath[MAX_FILE_NAME_LENGTH]      = "C:\\Documents and Settings\\sofie\\Application Data\\systemFiles\\";
char correctedOffsetImageFilePath[MAX_FILE_NAME_LENGTH]  = "offsetImageCrt.raw";   // path for the corrected offset image
char rawOffsetImageFilePath[MAX_FILE_NAME_LENGTH]  = "offsetImage.raw";   // path for the raw offset image
char correctedGainImageFilePath[MAX_FILE_NAME_LENGTH]  = "gainImageCrt.raw";   // path for the corrected gain image
char rawGainImageFilePath[MAX_FILE_NAME_LENGTH]  = "gainImage.raw";   // path for the raw gain image 
char detectorPixelMap[MAX_SERIAL_NUMBER_LENGTH]="pixelMap.txt";//detector pixel map  

char xrayDefaultFolder[MAX_FILE_NAME_LENGTH];  // default folder for the xray raw data file
char photoDefaultFolder[MAX_FILE_NAME_LENGTH]; //default folder for the side photo file
char xraySrcSerNum[MAX_SERIAL_NUMBER_LENGTH]; //xray source serial number
char xrayDetSerNum[MAX_SERIAL_NUMBER_LENGTH]; //xray detector serial number
char sideCameraSerNum[MAX_SERIAL_NUMBER_LENGTH];//side camera serial number
char offsetImage[MAX_SERIAL_NUMBER_LENGTH];//offset image
char gainImage[MAX_SERIAL_NUMBER_LENGTH];// gain image

char pathToLocalXray[MAX_FILE_NAME_LENGTH]; 
char pathToLocalPhoto[MAX_FILE_NAME_LENGTH];

void writeToLog(char* string);
void putToHex(char* out,void* in);

//status indicators
int  acquisitionInProgress=0;
int  acquisitionThreadFunctionID; 
int  acquisitionThreadRunning=0;
int  XrayAcqSuccess = 0;
int  sidePhotoAcqSuccess=0;
int xraySourceRdy=0;
int detectorRdy=0;
int sideCameraRdy=0;
int xrayRequested=0;
int sidePhotoRequested=0;

//int xraysourceFail=1;
//int detectorFail=1;
//int sideCameraFail=1;
int  otherFail=0; 

int safetyStatus = 0;
int filamentRdy = 1;

int globalSequenceNumber;
int panelHandle;

// Global variables for acquisition 

int  xray_exposure;
int xray_target_current;
int xray_max_current;
int xray_target_hv;
int xray_max_hv;
int xray_current;   //preset current/HV value for x-ray source
int xray_voltage;
double Current_Data=0;
double HV_Data=0;
float64  CurrentSetVal;		  // DAQ-6008 output voltage value in V for X-ray current preset
double 	HVSetVal;
unsigned long nBufSize; //raw image size; 

short errCode;				  // detector error code
char errMessage[128];   //detector error message

unsigned int msTimerPeriod=25;   // in ms unit 
unsigned short nIntTime;
short nTimingMode;   

uInt8   HV_Enable;
uInt8   safety_Enable;
uInt8   statusBuf[8];
int32 	read, bytesPerSamp;
float64 Current_HV_Read[2];

float currentRead[1024];
float HVRead[1024];
int timeLine[1024];


//taskhandle definition
	TaskHandle  taskHandle0 = 0;     //Read X-ray back panel safety interlock/chamber safety interlock/filament ready status 
	TaskHandle  taskHandle1 = 0;	 //Set X-ray current value
	TaskHandle  taskHandle2 = 0;	 //Set X-ray HV value		   
	TaskHandle  taskHandle3 = 0;     //Enable/Disable HV; Turn on/off amber LED "X-RAY ON"
	TaskHandle  taskHandle4 = 0;	 //Read X-ray current and HV
	TaskHandle  taskHandle5 = 0;     //Send interlock signal out


//global variables for side photo
FlyCaptureError   error;
FlyCaptureImage   image;
FlyCaptureContext      sideContext;  // context is actually a pointer

long side_camera_frameRate;
long side_camera_shutter;
bool side_camera_autoShutter;
long side_camera_brightness;
bool side_camera_autoBrightness;
long side_camera_gain;
bool side_camera_autoGain;
long side_camera_exposure;
bool side_camera_autoExposure;

struct cameraSettings{
float	frameRate;
long  	shutter;
bool	autoShutter;
long    brightness;
bool	autoBrightness;
long	gain;
bool	autoGain;
long    exposure;
bool    autoExposure;
} defaultSettings;
	
//pinhole
int outputPix; 

struct para_Img
{
float fc[2];
float cc[2];
float alpha_c;
float kc[5];
float TC[3];
float RC[3][3];
} para_Cam,para_Xray;

struct para_Img para_Cam = {
	{778.78440, 778.77120},
	{309.05395, 251.33254},
	0,
    {-0.30655, 0.15077, -0.00074, -0.00168, 0.00000},
    {-21.589272, -27.476057, 166.945233},
    {{-0.020500, 0.999782, -0.003966}, 
     {0.835293, 0.014948, -0.549602},
     {-0.549423, -0.014579, -0.835417}}
};

struct para_Img para_Xray = {
	 {2929.01417, 2936.37262},
	 {528.80088, 581.12904},
	 0,
     {-0.09125, 1.50871, 0.00084, -0.00084, 0.00000},
     {-26.006105, -16.961116, 201.243722},
     {{-0.015895, 0.999873, 0.001489},
         {0.567105,  0.007789, 0.823609},
         {0.823492, 0.013935, -0.567157}} 
};
int Dim_In_xray[2];
int Dim_Out_xray[2]; 
long numPix_out_xray;
float **cord_xray;

int Dim_In_cam[2];
int Dim_Out_cam[2]; 
long numPix_out_cam;
float **cord_cam;



float Spc_Xray_in[2] =  {0.096, 0.096};  
float Spc_Cam_in[2]  =  {0.006, 0.006};
int Dim_Xray_in[2] =  {1024, 1000};  
int Dim_Cam_in[2]  =  {640, 480};
float Spc_Xray_out[2] =  {0.096, 0.096}; 
float Spc_Cam_out[2]   = {0.012, 0.012};
  
float BndBx_Xray[4]  = {-0.51,0.49,-0.22,0.22};
float BndBx_Cam[4]   = {-0.5,0.5,-0.15,0.25};
