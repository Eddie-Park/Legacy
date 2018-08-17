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



#ifndef globalDecls_H
#define globalDecls_H




// Global variables
extern FILE *logFile;   
extern FILE *rawDataFile; // file for the raw data 

extern HKEY  xrayRegKey;
extern HKEY  xrayAcqRegKey;

extern char systemFilesPath[MAX_FILE_NAME_LENGTH]; 
extern char correctedOffsetImageFilePath[MAX_FILE_NAME_LENGTH];   // path for the corrected offset image
extern char rawOffsetImageFilePath[MAX_FILE_NAME_LENGTH];   // path for the raw offset image
extern char correctedGainImageFilePath[MAX_FILE_NAME_LENGTH];   // path for the corrected gain image
extern char rawGainImageFilePath[MAX_FILE_NAME_LENGTH];   // path for the raw gain image 
extern char detectorPixelMap[MAX_SERIAL_NUMBER_LENGTH];//detector pixel map


extern char xrayDefaultFolder[MAX_FILE_NAME_LENGTH];  // default folder for the xray raw data file
extern char photoDefaultFolder[MAX_FILE_NAME_LENGTH]; //default folder for the side photo file
extern char xraySrcSerNum[MAX_SERIAL_NUMBER_LENGTH]; //xray source serial number
extern char xrayDetSerNum[MAX_SERIAL_NUMBER_LENGTH]; //xray detector serial number
extern char sideCameraSerNum[MAX_SERIAL_NUMBER_LENGTH];//side camera serial number
extern char detectorPixelMap[MAX_SERIAL_NUMBER_LENGTH];//detector pixel map
extern char offsetImage[MAX_SERIAL_NUMBER_LENGTH];//offset image
extern char gainImage[MAX_SERIAL_NUMBER_LENGTH];// gain image

extern char pathToLocalXray[MAX_FILE_NAME_LENGTH]; 
extern char pathToLocalPhoto[MAX_FILE_NAME_LENGTH];

extern void writeToLog(char* string);
extern void putToHex(char* out,void* in);

//status indicators
extern int  acquisitionInProgress;
extern int  acquisitionThreadFunctionID; 
extern int  acquisitionThreadRunning;
extern int  otherFail;
extern int  XrayAcqSuccess;
extern int  sidePhotoAcqSuccess;
extern int xraySourceRdy;
extern int detectorRdy;
extern int sideCameraRdy;
extern int xrayRequested;
extern int sidePhotoRequested;

//extern int xraysourceFail;
//extern int detectorFail;
//extern int sideCameraFail;

extern int safetyStatus;
extern int filamentRdy;

extern int globalSequenceNumber;
extern int panelHandle;

// Global variables for acquisition 

extern int  xray_exposure;
extern int xray_target_current;
extern int xray_max_current;
extern int xray_target_hv;
extern int xray_max_hv;
extern int xray_current;   //preset current/HV value for x-ray source
extern int xray_voltage;
extern double Current_Data;
extern double HV_Data;
extern float64  CurrentSetVal;		  // DAQ-6008 output voltage value in V for X-ray current preset
extern double 	HVSetVal;
extern unsigned long nBufSize; //raw image size; 

extern short errCode;				  // detector error code
extern char errMessage[128];   //detector error message

extern unsigned int msTimerPeriod;   // in ms unit 
extern unsigned short nIntTime;
extern short nTimingMode;   

extern uInt8   HV_Enable;
extern uInt8   safety_Enable;
extern uInt8   statusBuf[8];
extern int32 	read, bytesPerSamp;
extern float64 Current_HV_Read[2];

extern float currentRead[1024];
extern float HVRead[1024];
extern int timeLine[1024];	  

//taskhandle definition
extern	TaskHandle  taskHandle0;     //Read X-ray back panel safety interlock/chamber safety interlock/filament ready status 
extern	TaskHandle  taskHandle1;	 //Set X-ray current value
extern	TaskHandle  taskHandle2;	 //Set X-ray HV value		   
extern	TaskHandle  taskHandle3;     //Enable/Disable HV; Turn on/off amber LED "X-RAY ON"
extern	TaskHandle  taskHandle4;	 //Read X-ray current and HV
extern  TaskHandle  taskHandle5;     //Send interlock signal out 


extern FlyCaptureError   error;
extern FlyCaptureImage   image;
extern FlyCaptureContext      sideContext;  // context is actually a pointer
//global variables for side photo

extern long side_camera_frameRate;
extern long side_camera_shutter;
extern bool side_camera_autoShutter;
extern long side_camera_brightness;
extern bool side_camera_autoBrightness;
extern long side_camera_gain;
extern bool side_camera_autoGain;
extern long side_camera_exposure;
extern bool side_camera_autoExposure;

extern struct cameraSettings{
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
extern int outputPix;

extern struct para_Img
{
float fc[2];
float cc[2];
float alpha_c;
float kc[5];
float TC[3];
float RC[3][3];
} para_Cam,para_Xray;

extern int Dim_In_xray[2];
extern int Dim_Out_xray[2]; 
extern long numPix_out_xray;
extern float **cord_xray;

extern int Dim_In_cam[2];
extern int Dim_Out_cam[2]; 
extern long numPix_out_cam;
extern float **cord_cam;

extern float Spc_Xray_in[2] ;  
extern float Spc_Cam_in[2];
extern int Dim_Xray_in[2];  
extern int Dim_Cam_in[2];
extern float Spc_Xray_out[2]; 
extern float Spc_Cam_out[2];
  
extern float BndBx_Xray[4];
extern float BndBx_Cam[4];

#endif 
