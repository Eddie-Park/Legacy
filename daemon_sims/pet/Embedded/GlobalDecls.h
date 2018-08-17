//*******************************************************************************
//*                                                                             *
//*  PETBox Server Software                                                     *
//*                                                                             *
//*  Copyright - 2009                                                           *
//*                                                                             *
//*  This code is the property of the Regents of UCLA.                          *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Richard Taschereau and Hui Zhang                                *
//*                                                                             *
//*  Created: September 2009                                                    *
//*                                                                             *
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************

#ifndef globalDecls_H
#define globalDecls_H

// Global variables
extern FILE *logFile;		   // log file
extern FILE *rawDataFile;	   // file for the raw data

extern char systemFilesPath[MAX_FILE_NAME_LENGTH]; 
extern char rawDataFilePath[MAX_FILE_NAME_LENGTH];    // path for the raw data file
extern char lutFilePath[MAX_FILE_NAME_LENGTH];        // path for the crystal lookup table
extern char lutLoaderFilePath[MAX_FILE_NAME_LENGTH];  // path for the LUT loader program
extern char FPGABitstreamFilePath[MAX_FILE_NAME_LENGTH];  //path for bitstream file

extern void writeToLog(char* string);

extern int  acquisitionThreadFunctionID;
extern int  acquisitionInProgress;
extern int  acquisitionThreadRunning;
extern int  mainPanel;
extern int  serialPanel;

extern int serialNumber;
extern int dicomRegID;
extern int productID;    //PETBox
extern int globalSequenceNumber;

extern HKEY  petBoxRegKey;
extern HKEY  petBoxAcqRegKey;

// Global variables for acquisition 
extern int acqHardwareRdy; 
extern unsigned int msTimerPeriod;   // in ms unit
extern unsigned short panelUpdateFreq;  // every second 
extern char rawDataFileName[MAX_FILE_NAME_LENGTH];

// PET event acqusition parameters
extern int FPGA_LLD; 				// Lower level (energy) discrimination (in keV)
extern int FPGA_ULD; 				// Upper level (energy) discrimination (in keV)
extern int FPGA_Noise_Threshold; 
extern int FPGA_Event_Threshold; 
extern int FPGA_Blocking_Time; 
extern int FPGA_Integration_Time;
extern int FPGA_Coinc_Window;      //in Q6.4 format
extern int FPGA_Coinc_Window_d;    //in decimal format

//6008
extern uInt8 PET_On;

extern TaskHandle  taskHandle6;


#endif
