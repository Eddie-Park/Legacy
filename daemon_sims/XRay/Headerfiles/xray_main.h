
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


#ifndef xray_main_H
#define xray_main_H

void serverTermination(void);
int serverInitialization(void);

int hardwareInitialization(void);
int hardwareTermination(void); 
int xraySourceInitialization(void);
int detectorInitialization(void);
int sideCamInitialization(void);
int xraySourceTermination(void);
int detectorTermination(void);
int sideCamTermination(void);

void fly_ErrorMessage(FlyCaptureError error, char *errMessage); 

void imageCorrection (unsigned short* ImgBuf);
void pinhole_Xray ( unsigned short* imgBuf);
void pinhole_SidePhoto ( unsigned char *imgBuf);
void pinhole_Xray_Para_Initial(void);
void pinhole_Cam_Para_Initial(void);


#define PORT_NUMBER            65534
#define MAX_IMAGE_LENGTH      2048000


#define PRESET_CURRENT_VALUE 100
#define PRESET_HV_VALUE 40
#define PRESET_EXPOSURE_TIME_VALUE 3000
#define TARGET_CURRENT_VALUE 94
#define MAX_CURRENT_VALUE 101
#define TARGET_HV_VALUE  37
#define MAX_HV_VALUE 41 
#define TARGET_RESOLUTION 1         // 1-millisecond timer resolution 
#define MAX_SOURCE_START_UP_TIME	5000
#define IMAGE_WIDTH 1024
#define IMAGE_HEIGHT 1000
#define TIMING_MODE 0

#define FRAME_RATE 15
#define SHUTTER 28
#define AUTO_SHUTTER 1
#define BRIGHTNESS 176
#define AUTO_BRIGHTNESS 1
#define GAIN 0
#define AUTO_GAIN 1
#define EXPOSURE 11
#define AUTO_EXPOSURE 1

#define DEFAULT_XRAY_DATA_PATH "N:\\PETbox_StudyData" 
#define DEFAULT_PHOTO_DATA_PATH "N:\\PETbox_StudyData"
#define DEFAULT_XRAY_SOURCE_SN "40KV MAGNUM"
#define DEFAULT_XRAY_DETECTOR_SN "RADICON RADEYE 2000"
#define DEFAULT_SIDE_CAMERA_SN "FlyCapture"



#endif 
