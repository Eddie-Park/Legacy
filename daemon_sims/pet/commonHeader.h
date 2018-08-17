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


#ifndef commonHeader_H
#define commonHeader_H


#define PORT_NUMBER           65535
#define MAX_STUDY_LENGTH      50
#define MAX_FILE_NAME_LENGTH  262

#define DEFAULT_RAW_DATA_PATH "N:\\PETbox_StudyData"

enum messageTypeValues        {TYPE_COMMAND=0,       TYPE_ACK=128,     TYPE_NACK,    TYPE_ATTN,   TYPE_UNSOC         };
enum messageCommandCodeValues {COMMAND_START=1,      COMMAND_STOP,      COMMAND_INQUIRE_STATUS};
enum statusValues             {STATUS_ACQ_RUNNING=1, STATUS_LIGHT=2, STATUS_CHAMBER_DOCKED=4};  // keep powers of 2


// -----------------   Command structures
struct structStartCommand{
	unsigned int duration;  
	char studyID[MAX_STUDY_LENGTH];
	char studyPathName[MAX_FILE_NAME_LENGTH];	   //  full path of target study directory
};
struct structStopCommand{
	char filler;
};
struct structInquireCommand{
	char filler;
};


// -----------------    Control structures
struct structStartAcknowledge{
	char lmfFileName  [MAX_FILE_NAME_LENGTH];	   //  full path of local raw data (lmf file)
};
struct structStopAcknowledge{
	char filler;
};
struct structInquireAcknowledge{
	int  serialNumber;
	unsigned short int status;
	char studyID[MAX_STUDY_LENGTH];				  
	char studyPathName[MAX_FILE_NAME_LENGTH];
	char lmfFileName  [MAX_FILE_NAME_LENGTH];	   //  full path of local raw data (lmf file)
};
struct structAttention{
	char filler;
};
struct structUnsollicited{
	unsigned short int status;
	unsigned int promptsRate;
	unsigned int timeLeft;
};



// -------------------   MESSAGE
struct PETBoxMessage{
	// Common segment
	unsigned short length;
	unsigned char  messageType;	  // command or control (ack, nack, attention, )
	unsigned char  commandCode;
	
	// Variable data segment
	union{
		struct structStartCommand       startCmd;
		struct structStopCommand        stopCmd;
		struct structInquireCommand     inqCmd;
		struct structStartAcknowledge   startAck;
		struct structStopAcknowledge    stopAck;
		struct structInquireAcknowledge inqAck;
		struct structAttention          attn;
		struct structUnsollicited       unsoc;
	};
};

#endif
