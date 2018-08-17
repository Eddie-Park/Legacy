//*******************************************************************************
//*                                                                             *
//*  PETBox Acquisition and Management Graphical User Interface                 *
//*                                                                             *
//*  Copyright - 2011                                                           *
//*                                                                             *
//*  This code is the property of the Regents of UCLA.                          *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Richard Taschereau                                              *
//*                                                                             *
//*  Created: May 2011                                                          *
//*                                                                             *
//*  Change log:  																*
//*               RT    08/01/2011 - Changed #defines                           *
//*               Shang 08/01/2011 												*
//*               Shang 01/11/2012-deleted pathToLocalXray_raw 					*
//*									in "structInquireAcknowledge" and			*
//*									 "structUnsollicited"						*
//*                                                                             *
//*******************************************************************************
/*
										  -------------------------------
                                          CLIENT <-> SERVER Communication
										  -------------------------------

										  
  There are 5 possible "conversations" between the client and the server. 3 are initiated by the client
  and 2 are initiated by the server.
  
  
 1) A start command is issued to request acquisition of an xray and/or a photograph 
 
	    Start          -> 
	  		           <-  ACK (positive acknowledge) or NACK (negative acknowledge)
			
 
 2) An inquires status command is issued when the GUI starts up, to know what is going on.
 
	    Inquire status  ->
	  		 	       <-  ACK  (Note: no negative acknowledge allowed here)

					   
 3) A stop command is issued when the GUI shuts down or following a user request
 
	    Stop            ->
	                   <-  ACK or NACK

					   
 4) Unsollicited status is sent to the GUI at the end of acquisition
 
	                   <-  UNSOC (Unsollicited status)

					   
 5) An attention is sent by the server whenever something unexpected happened.
 
	                   <-  ATTN (Attention)
	    Inquire status  ->     
	                   <-  ACK (Note: no negative acknowledge allowed here)   
			
*/

#ifndef xrayDaemon_H
#define xrayDaemon_H

#define MAX_STUDY_LENGTH           50
#define MAX_SERIAL_NUMBER_LENGTH   50
#define MAX_FILE_NAME_LENGTH       262


// Message type code
enum {
	XDMN_MSG_TYPE_COMMAND              = 0x00,       
	XDMN_MSG_TYPE_ACK                  = 0x80,     
	XDMN_MSG_TYPE_NACK                 = 0x81,    
	XDMN_MSG_TYPE_ATTN                 = 0x82,   
	XDMN_MSG_TYPE_UNSOC                = 0x83         
};

// Command codes
enum {
	XDMN_COMMAND_START                 = 1,      
	XDMN_COMMAND_STOP                  = 2,      
	XDMN_COMMAND_INQUIRE_STATUS        = 3
};

// start command request flags 
enum {
	XDMN_START_FLAG_XRAY               = 0x80, 	   // xray  requested
	XDMN_START_FLAG_PHOTO              = 0x40	   // photo requested
};

// Inquire status: status byte
enum {
	XDMN_INQ_STAT_ACQ_IN_PROGRESS      = 0x80,     // XRAY acquisition in progress
	XDMN_INQ_STAT_XRAY_ON              = 0x40,     // XRAY on
	XDMN_INQ_STAT_CHAMBER_PRESENT      = 0x20	   // Chamber present, safety interlock open
};

// Inquire status error flag byte 
enum {
	XDMN_INQ_STAT_ERR_XSRC_FAIL        = 0x80,     // XRAY source failure
	XDMN_INQ_STAT_ERR_XDET_FAIL        = 0x40,     // XRAY detector failure
	XDMN_INQ_STAT_ERR_CAM_FAIL         = 0x20,	   // Camera failure
	XDMN_INQ_STAT_ERR_INTERLOCK_OPEN   = 0x10,     // Safety interlock open										*****added*****
	XDMN_INQ_STAT_ERR_OTHER_FAIL       = 0x01	   // Other failure
};


// Negative acknowledge reason flags for a start command	                                                                        /****changed****/
enum {
	    
	XDMN_NACK_START_ACQ_INPROGRESS     = 0x40,	   // an acquisition is already in progress 
	//XDMN_NACK_START_INTERLOCK_OPEN     = 0x20,	   // safety interlock open									 ***deteled****
	XDMN_NACK_START_XRAY_FAILURE       = 0x10,	   // a failure occured with the xray source
	XDMN_NACK_START_DETECTOR_FAILURE   = 0x08,	   // a failure occured with the detector
	XDMN_NACK_START_SIDE_CAM_FAILURE   = 0x04,	   // a failure occured with the side camera 	   
	XDMN_NACK_START_OTHER_FAILURE      = 0x02	   // other, unspecified reason for not starting, including request flag is all zero 
};

// Negative acknowledge reason flags for a stop command
enum {
	XDMN_NACK_STOP_NO_ACQ              = 0x80,    // there is no acquisition in progress
	XDMN_NACK_STOP_OTHER_FAILURE       = 0x01     // other, unspecified reason for not stopping
};

// Unsollicited status completion codes
enum {
	XDMN_UNSOC_COMP_SUCCESS            = 0,
	XDMN_UNSOC_COMP_FAILURE            = 8
};



// -----------------   Command structures  (from client to server )
struct structStartCommand{
	unsigned char  startCommandRequest;			   // start command request flags
	char studyID[MAX_STUDY_LENGTH];				   // study (short) identification
	char studyPathName[MAX_FILE_NAME_LENGTH];	   // full path of target study directory
};

// -----------------  Control structures (from server to client)

// ACK and NACK for start
struct structStartAcknowledge{
    unsigned char reasonCode;						// reason code if NACK
};

// ACK and NACK for stop
struct structStopAcknowledge{
    unsigned char reasonCode;						// reason code if NACK
};

// ACK for Inquire status (Note: there is no NACK for inquire status)
struct structInquireAcknowledge{
	unsigned char status;				            // current system status
	unsigned char error;				            // error flags
	
	// Acquisition Data (when acquisition in progress)
	char studyID[MAX_STUDY_LENGTH];				    // study identification (from GUI) 
	char studyPathName[MAX_FILE_NAME_LENGTH];	    // study full path name on external PC (from GUI)
	char pathToLocalXray[MAX_FILE_NAME_LENGTH];     // full path name to local xray on local (shared) disk
	//char pathToLocalXray_raw[MAX_FILE_NAME_LENGTH];  //full path name to local xray raw data on disk        /**************deleted**********/
	char pathToLocalPhoto[MAX_FILE_NAME_LENGTH];    // full path name to local photograph on local (shared) disk
	
	// Xray settings
	char xraySrcSerNum[MAX_SERIAL_NUMBER_LENGTH];   // xray source serial number
	char xrayDetSerNum[MAX_SERIAL_NUMBER_LENGTH];   // xray detector serial number
	int  xray_voltage;					            // xray kilo-voltage peak setting (in kilo-volts)
	int  xray_current;					            // xray current settings (in milli-ampere)
	int  xray_exposure;				                // xray exposure time (in milliseconds)
	int  xray_current_read;                         // xray current readed from 6008	   /********add**********/
	int  xray_voltage_read;							// xray exposure readed from 6008	   /********add**********/
	int  safety_status_read;                        // safety interlock status readed from 6008 /********add**********/
	
	
	// Camera settings
	char camSerNum[MAX_SERIAL_NUMBER_LENGTH];       // camera serial number
	int  cam_exposure;
	int  cam_gain;
	int  cam_brightness;
	 
};

struct structUnsollicited{
	unsigned char error;                            // error flags (same as inquire status error flags) 
	int completionCode;                             // acquisition completion code 
	char studyID[MAX_STUDY_LENGTH];				    // study (short) identification
	char studyPathName[MAX_FILE_NAME_LENGTH];	    // full path of target study directory
	char pathToLocalXray[MAX_FILE_NAME_LENGTH];     // full path name to local xray on local (shared) disk
//	char pathToLocalXray_raw[MAX_FILE_NAME_LENGTH];		//********deleted*************/
	char pathToLocalPhoto[MAX_FILE_NAME_LENGTH];    // full path name to local photograph on local (shared) disk
	};



struct PETBoxMessage{
	
	// Common segment
	unsigned short length;	           // total message length
	unsigned char  messageType;	       // message type, i.e. command or control (ack, nack, attention, )
	
	// Variable data segment
	union{
		struct structUnsollicited           unsoc;
		struct{
			unsigned char  commandCode;
			union{
				struct structStartCommand       startCmd;
				struct structStartAcknowledge   startAck;
				struct structStopAcknowledge    stopAck;
				struct structInquireAcknowledge inqAck;
			};
			
		};
		
	};
	
	
};


#endif



