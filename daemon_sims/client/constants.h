//*******************************************************************************
//*                                                                             *
//*  Genisys 4 Graphical User Interface                                         *
//*                                                                             *
//*  Copyright - 2009, 2010, 2011                                               *
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
				   

#ifndef constants_H
#define constants_H

#define PET_BOX_REG_KEY   "SOFTWARE\\Sofie Inc.\\Genisys\\2.0"
#define CURRENT_STUDYDATA_VERSION  2
#define CURRENT_TASKLIST_VERSION   5
#define CURRENT_PROTOCOL_VERSION   4

#define MAX_PROTOCOL_NAME_LENGTH   50
#define MAX_STUDY_NAME_LENGTH      50
#define MAX_FILE_NAME_LENGTH       262
#define MAX_FRAME_SEQUENCE_LENGTH  200
#define MAX_RECENT_PROTOCOLS       3
#define MAX_LEVEL                  10

#define NO_IMAGE_INDEX            -1
#define ALL_CATEGORY_INDEX         0
#define DIR_IMAGE_INDEX            1
#define PROT_IMAGE_INDEX           2
#define STUDY_IMAGE_INDEX          2


	/* 
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

#define BIT_ATTNCORR 0x80
#define BIT_DCYCORR  0x40
#define BIT_DTIMCORR 0x20
#define BIT_RNDCORR  0x10
#define BIT_NORM     0x08


enum editReasons     {NEW,  EDIT  };
enum enumPostCodes   {QUIT=1000,     POST           };
enum completionCodes {SUCCESS,       FAILURE        };
 
#endif
