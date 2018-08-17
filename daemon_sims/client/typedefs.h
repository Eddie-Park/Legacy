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

#ifndef typedefs_H
#define typedefs_H

typedef int bool; // that is for PGR FlyCapture

#include "constants.h"
#include "FlyCapture2_C.h"

//
enum eventDisposition {eventAccept, eventSwallow};

// actions for the frame sequence state machine
enum action{ ACTION_ACCEPT=1, ACTION_REJECT=2, ACTION_BEEP=4, ACTION_PUSH_CHAR=8, ACTION_RESET_NUM_FRAMES=16, ACTION_MUST_REPLAY=32};

// States for the time state machine
enum enumTimeStates {timeState_h1,timeState_h2,timeState_m1,timeState_m2,timeState_s1,timeState_s2,timeState_done};

struct isotope{
	char name[8];
	float halfLife;
	float branchingFraction;
};

struct protocolSubMenu{
	int menuBarHandle;
	int subMenuID;
};

struct cameraMode{
	int mode;
	int pixFormat;
	int maxWidth;
	int maxHeight;
	int Hunit;
	int Vunit;
};

struct cameraSettings{
	char  name[8];
	int   gain;
	int   shutter;
	int   brightness;
	unsigned int frameRate;
	
	int autoGain;
	int autoShutter;
	int autoBrightness;
	
	Rect displayArea;
	struct cameraMode camMode;
};

enum studyTreeColumn{ COLUMN_STUDYNAME=0, COLUMN_ACQ=1, COLUMN_HIST=2, COLUMN_RECON=4};
enum taskStatus{ TASK_STATUS_COMPLETED=1, TASK_STATUS_ERROR, TASK_STATUS_NOT_REQUESTED, TASK_STATUS_SCHEDULED, TASK_STATUS_RUNNING};

/* 
	A protocol is a generic set of parameters 
	ready to be applied to a study
	
*/

// CURRENT protocol
struct protocol{
	unsigned short int version;
	char   tag          [MAX_TREE_ITEM_TAG_LEN];     // unique identifier for protocol         
	char   name         [MAX_PROTOCOL_NAME_LENGTH];
	
	/* requested tasks */
 	int acq;
 	int hist;
  	int coreg;
 	int recon;
	int tskRsvd1;
	int tskRsvd2;

	/* Acquisition related parameters */
	unsigned int		duration;
	struct  isotope     theIsotope;
	int		breathingMon;
	int     xray;
	int     acqRsvd2;
	
	/* Histogramming related parameters 
	
		Histogramming options (bit string) 
		
		80 : attenuation correction
		40 : decay correction
		20 : dead-time correction
		10 : randoms correction (subtract)
		08 : normalization
		04 : reserved
		02 : reserved
		01 : reserved
	*/

	unsigned char histOptions;  
	char	normFile[MAX_FILE_NAME_LENGTH ];
	char    histFileRootName[MAX_FILE_NAME_LENGTH];
	char    lmfFile[MAX_FILE_NAME_LENGTH];
	char    attnCorrFile[MAX_FILE_NAME_LENGTH];
	char	frameSequence[MAX_FRAME_SEQUENCE_LENGTH];
	int     histRsvd1;
	int     histRsvd2;
	
	/* Co registration parameters */
	int     coregRsvd1;
	int     coregRsvd2;
	
	/* recon */
	unsigned short int iterations;
	float   calibrationConstant;
	float   beta;
	int     reconRsvd1;
	int     reconRsvd2;
	
	
};

/* 
	a study is a specific investigation
	with a specific subject
*/
struct studyData{
	unsigned short int version;
	char   tag          [MAX_TREE_ITEM_TAG_LEN];     // unique identifier for study         
	char   name         [MAX_STUDY_NAME_LENGTH];
	char   investigator [MAX_FILE_NAME_LENGTH];
	char   subject      [MAX_STUDY_NAME_LENGTH];
	char   compound     [MAX_STUDY_NAME_LENGTH];
	time_t assayTime;
	float  assayActivity;
	time_t adminTime;
};

struct taskInfo{
	time_t startTime;
	time_t endTime;
	int    completionCode;
};

/*
	The task list is the instanciation of a protocol 
	associated with a study.  The task list is saved
	in the study directory and serves as a road map
	of things done/to do.
*/



// CURRENT tasklist
struct taskList{
	unsigned short int version;
	struct studyData study;
	struct protocol  prot;
	
	/* task Specific info */
	struct taskInfo  acqInfo;
	struct taskInfo  histInfo;
	struct taskInfo  coregInfo;
	struct taskInfo  reconInfo;
};


struct fadingPanel{
	int panel;       // the panel handle
	int numSteps;    // number of steps in animation
	unsigned char **alphaArray;   // array of images
	int bitMapID; 	  // the bitmapID
};

struct vector{
	double x;
	double y;
}; 


struct LEDControl{
	int panelID;
	int vmin;			  // Min value in the bar graph
	int vmax;			  // Max value
	int numLED;			  // Number of "leds" in the bar graph
	
	int *LEDIDarray;	  // Control IDs (one per bar)
	int *LEDColorArray;   // Color array (one color per bar)
	int valueInterval;	  // Interval between bars
	
	int bgColor;		  // color when off
	int overflowColor;    // Color when greater than max
	int lastNumLEDtoLite;
};


struct waitPanelControlPair{
	int panel;
	int control;
	int imageCount;
	int imageIndex;
	int tickCounter;
};

struct exportData{
	char exportDirectory[MAX_FILE_NAME_LENGTH];
	int checkLMF;
	int checkHIST;
	int checkPHOTOS;
	int checkDICOM;
	int checkRAWIMG;
	char studyTag[MAX_TREE_ITEM_TAG_LEN];
	int batchMode;

};

struct locale{
	char  id[3];   // actually 2 + 1 (null char)
	char  activityUnits[16];
	float activityToBqFactor;
};

#endif
