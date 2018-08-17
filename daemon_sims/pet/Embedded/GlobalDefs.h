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


// Global variables
FILE *logFile;

FILE *rawDataFile; // file for the raw data 

char rawDataFilePath[MAX_FILE_NAME_LENGTH];            // path for the raw data file

char systemFilesPath[MAX_FILE_NAME_LENGTH]      = "C:\\Documents and Settings\\sofie\\Application Data\\systemFiles\\"; 
char lutFilePath[MAX_FILE_NAME_LENGTH]          = "CELUT.bin";                   // path for the crystal lookup table
char lutLoaderFilePath[MAX_FILE_NAME_LENGTH]    = "PETBox4_LUTloading_v17.bit";  // path for the LUT loader program
char FPGABitstreamFilePath[MAX_FILE_NAME_LENGTH]= "PETBox4_Acq_v68.bit";         //path for bitstream file

void writeToLog(char* string);

int  acquisitionThreadFunctionID;
int  acquisitionInProgress;
int  acquisitionThreadRunning;
int  mainPanel;
int  serialPanel;

int serialNumber=0;
int dicomRegID =   114469;    //  true DICOM ID 
int productID  =  1010004;    //  true Genisys product ID
int globalSequenceNumber;

HKEY  petBoxRegKey;
HKEY  petBoxAcqRegKey;

// Global variables for acquisition 
int acqHardwareRdy=0; 
unsigned int msTimerPeriod=25;   // in ms unit
unsigned short panelUpdateFreq=13;  // every second 
char rawDataFileName[MAX_FILE_NAME_LENGTH]="";

// PET event acqusition parameters
int FPGA_LLD; 				// Lower level (energy) discrimination (in keV)
int FPGA_ULD; 				// Upper level (energy) discrimination (in keV)
int FPGA_Noise_Threshold; 
int FPGA_Event_Threshold; 
int FPGA_Blocking_Time; 
int FPGA_Integration_Time;
int FPGA_Coinc_Window;      //in Q6.4 format
int FPGA_Coinc_Window_d;    //in decimal format


//6008
uInt8 PET_On;

TaskHandle taskHandle6 = 0;


