/***************************************************************************
 *
 *
 *         **  **     **  ******  ********  ********  ********  **    **
 *        **    **   **  **   ** ********  ********  ********  **    **
 *       **     *****   **   **    **     **        **        **    **
 *      **       **    ******     **     ****      **        ********
 *     **       **    **  **     **     **        **        **    **
 *    *******  **    **   **    **     ********  ********  **    **
 *   *******  **    **    **   **     ********  ********  **    **
 *
 *            L Y R T E C H   S I G N A L   P R O C E S S I N G
 *
 ***************************************************************************
 *
 *  Project     : SignalMaster Dual
 *  File        : smd.h
 *  Description : SignalMaster Dual Project
 *
 *  Author(s)   : Jean-François Robert
 *
 *            Copyright (c) Lyrtech RD inc. 2007
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *  $Name: 1.3 $
 *  $Revision: 1.3 $
 *  $Date: 2008/08/05 15:57:09 $
 *  $Author: michel.mercier $
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file smd.h
    \brief SignalMaster Dual host API.

    This section describes all the functions used by SignalMaster Dual host API.
    Refer to this API first when developing applications meant to communicate
    with the SignalMaster Dual, as they contain application-specific routines.
*/

#ifndef __SMD_INC
#define __SMD_INC

#ifdef  __cplusplus
extern "C" {
#endif


/***************************************************************************
 *                           I N C L U D E S                               *
 ***************************************************************************/

#include "fpgaMgr.h"
#include <windows.h>
#include "smd_common.h"

#define SMD_MAX_DEVICES  16
#define SMD_MAX_PIPES  256

/**
* Hardware module version.
*/
typedef enum {
    SMD_HW_MODULE_VERSION_LYRIO = 0,    /*!< Hardware Module version without RapidCHANNEL. */
    SMD_HW_MODULE_VERSION_LYRIO_PLUS    /*!< Hardware Module version with RapidCHANNEL. */
} SMD_HW_MODULE_VERSION;

/**
* Hardware module identification.
*/
typedef enum {
    SMD_HW_MODULE_ID_ADAC_MASTER_III = 0,   /*!< ADAC Master III. */
    SMD_HW_MODULE_ID_DRC_V4 =1,             /*!< DRC V4. */
    SMD_HW_MODULE_ID_CAMERA_LINK = 2
} SMD_HW_MODULE_ID;

/**
* Hardware module revision.
*/
typedef enum {
    SMD_HW_MODULE_REVISON_A = 0,        /*!< Revision A. */
    SMD_HW_MODULE_REVISON_B,            /*!< Revision B. */
    SMD_HW_MODULE_REVISON_C,            /*!< Revision C. */
    SMD_HW_MODULE_REVISON_D             /*!< Revision D. */
} SMD_HW_MODULE_REVISON;

/**
* FPGA type on the hardware module.
*/
typedef enum {
    SMD_HW_MODULE_FPGA_ID_NONE = 0,     /*!< No FPGA. */
    SMD_HW_MODULE_FPGA_ID_XC4VLX25 = 1,     /*!< FPGA XC4VLX25. */
    SMD_HW_MODULE_FPGA_ID_XC4VLX35 = 2     /*!< FPGA XC4VLX35. */
} SMD_HW_MODULE_FPGA_ID;

/**
* Host IRQ Sources.
*/
typedef enum {
    SMD_HOSTIRQ_DSP1    = 0,    /*!< IRQ from DSP 1. */
    SMD_HOSTIRQ_DSP2,           /*!< IRQ from DSP 2. */
    SMD_HOSTIRQ_DSP3,           /*!< IRQ from DSP 3. */
    SMD_HOSTIRQ_DSP4,           /*!< IRQ from DSP 4. */
    SMD_HOSTIRQ_FPGA1,          /*!< IRQ from FPGA 1. */
    SMD_HOSTIRQ_FPGA2           /*!< IRQ from FPGA 2. */
} SMD_HOSTIRQ;


/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

/**
* Hardware module Information.
*/
typedef struct __SMD_HW_MODULE_INFO
{
    unsigned char   ucIsPresent;        /*!< TRUE if a module is detected */
    unsigned char   ucVersion;          /*!< See SMD_HW_MODULE_VERSION_XXX */
    unsigned char   ucModuleId;         /*!< See SMD_HW_MODULE_ID_XXX*/
    unsigned char   ucHardOpt;          /*!< Variable */
    unsigned char   ucHwRevision;       /*!< See SMD_HW_MODULE_REVISON_XXX */
    unsigned char   ucAlign1;
    unsigned short  usBmInterface;      /*!< Bitmask. See SMD_HW_MODULE_INTERFACE_XXXX */
    unsigned short  usEepromLenght;     /*!< Eeprom length */
    unsigned char   ucFpgaId;           /*!< See HW_MODULE_FPGA_ID_XXXX */
    unsigned char   ucAlign2;
    unsigned short  usVarious;          /*!< User defined */
    unsigned short  usChecksum;         /*!< Checksum */
} SMD_HW_MODULE_INFO, *PSMD_HW_MODULE_INFO;


// SMD Device Information
typedef struct __SMD_DEVICE_INFO
{
  int PciBase0;
  int PciBase1;
  int PciBase2;
  int PciBase3;
  int PciBusNumber;
  int PciDevice;
  int PciFunction;
  unsigned int irqPin;
  unsigned int irqLine;
  unsigned int BoardID;
}
SMD_DEVICE_INFO, *PSMD_DEVICE_INFO;

// All SMD Devices Information
typedef struct __SMD_ALL_DEVICES_INFO
{
  unsigned int NumberOfBoards;
  unsigned int BoardID;
  SMD_DEVICE_INFO DeviceInfo[SMD_MAX_DEVICES];
}
SMD_ALL_DEVICES_INFO, *PSMD_ALL_DEVICES_INFO;


// SMCXX Device Definition
/**
* SignalMaster Dual device definition.
*/
typedef struct __SMD_DEVICE
{
  // Magic Number (public)
#define SMD_DEVICE_MAGIC_NUMBER 0x51433658  // ASCII => "QC6X"
  int MagicNumber;

  // Version Control (public)
  // (changing this version will programs to be recompiled and verified)
#define SMD_DEVICE_VERSION_MAJOR  0

#define SMD_DEVICE_VERSION_MINOR  0
  int VersionMajor;
  int VersionMinor;

  // PCI location (public)
  int PciBusNumber;
  int PciDevice;
  int PciFunction;
  int PciBase0;
  int PciBase1;
  int PciBase2;
  int PciBase3;

  SMD_PLX9656_DEVICE  plx9656;
  SMFPGA_DEVICE fpga;

  int bModuleEepromIsInit;

  // DSP Specifications (public)
  int   DspType;
  float DspClock;
  int   DspClockMode;
  int   DspBootMode;
  int   DspMap;
  int   DspEndianess;
  int   DspJtagSource;
  int   AddressSize;  /* 0-> Byte, 1->Word, 2->Dword */
  int   FpgaBoot;   /* unused, for now */
  int   FpgaModel;
  int     IsV4; // indicate if the Dual support direct addressing on plx bus

  // Addressing stuff
  int ControlRegOffset[8];
  int DeviceBaseAddr[4];
  int DspHpiOffset[2][5]; /* 2 DSP */

  // WIN32 handles
  HANDLE  hHandle;  // General Purpose Handle (WINNT specific)
  HANDLE  hMutex;   // Mutex for Conflict Prevension

  // Pipe Information
  int   NumberOfPipes;
  int   PipeReadSize[SMD_MAX_PIPES];
  int   PipeWriteSize[SMD_MAX_PIPES];

  //FCO
  HANDLE  ReadPipeEvents[128];

  // Hardware module
  SMD_HW_MODULE_INFO     hwModuleInfo;

  // Pointer to private data structures
  void  *pPrivate;
}
SMD_DEVICE, *PSMD_DEVICE;

// SMCXX Device Information for EPK
typedef struct __SMD_DEVICE_FOR_EPK
{
  // Magic Number (public)
  int MagicNumber;

  // Version Control (public)
  int VersionMajor;
  int VersionMinor;

  // PCI location (public)
  int PciBusNumber;
  int PciDevice;
  int PciFunction;

  // DSP Specifications (public)
  int   DspType;
  float DspClock;
  int   DspClockMode;
  int   DspMap;
}
SMD_DEVICE_FOR_EPK, *PSMD_DEVICE_FOR_EPK;


typedef struct __SMD_BUFFER
{
  unsigned int  Target;
  unsigned int  Channel;
  unsigned int  Length;
  unsigned int  *pMemory;
  void      (*pCallback)( PSMD_DEVICE pDev, struct __SMD_BUFFER *pBuffer );
  unsigned int  TransferLength;
  unsigned int  TransferStatus;
  struct __SMD_BUFFER *pNext;
}
SMD_BUFFER, *PSMD_BUFFER;

/**
* Host IRQ callback function.
*/
typedef void (*SmDCallbackProc)( PSMD_DEVICE p, void *user );


/***************************************************************************
 *                           V A R I A B L E S                             *
 ***************************************************************************/

/***************************************************************************
 *                       P U B L I C  F U N C T I O N S                    *
 ***************************************************************************/

/** Initialize the library and the SignalMaster Dual board.
This function must be call before using any other function. The function detect
and initialize all the card detected.

\return 0 on error.
*/
int SmDInit( void );

/** Find a device handle.
This function finds a SignalMaster Dual card using an identifier that goes
from 0 to the number of detected SignalMaster Dual cards minus one. The user can determine the number
of cards detected by calling the function repeatedly, incrementing the identifier from 0 until
a NULL pointer is returned. Please refer to the example.
\param id Identificator of a board
\return Pointer of a device handle of a SignalMaster Dual board.
*/
PSMD_DEVICE SmDFindDevice( int id );

/** Quit the library.
This function free all the resources allocated in the initialization of the library.
\param id Identificator of a board
\return 0 on error.
*/
int SmDQuit( void );


/* DSP Routines */
/** Reset a DSP.
This function reset the DSP.
\param p Device handle of a SignalMaster Dual board.
\param dsp_id DSP id (1 to 4).
\return 0 on error.
*/
int SmDDspReset( PSMD_DEVICE p, int dsp_id );

/** Load an executable file to a DSP.
This function load an executable file (a COFF file) to the DSP and execute it.
\param p Device handle of a SignalMaster Dual board.
\param dsp_id DSP id (1 to 4).
\param coff_file Pointer to the data of an executable file for a DSP.
\param len Size of the execute file in bytes.
\return 0 on error.
*/
int SmDDspLoad( PSMD_DEVICE p, int dsp_id, unsigned char *coff_file, unsigned int len );

/** Write to DSP memory.
This function write data in the DSP memory.
\param p Device handle of a SignalMaster Dual board.
\param dsp_id DSP id (1 to 4).
\param address Address in the DSP memory.
\param size Number of unsigned long to write in the DSP memory.
\param inc Increment to make for the address for each value (default value is 1).
\param val  pointer to the buffer to write in the DSP memory.
\return 0 on error.
*/
int SmDWriteMemBlock( PSMD_DEVICE p, int dsp_id, int address, unsigned size, unsigned short inc, unsigned long *val );

/** Read from the DSP memory.
This function read data in the DSP memory.
\param p Device handle of a SignalMaster Dual board.
\param dsp_id DSP id (1 to 4).
\param address Address in the DSP memory.
\param size Number of unsigned long to read from the DSP memory.
\param inc Increment to make for the address for each value (default value is 1).
\param val  pointer to the buffer to read from the DSP memory.
\return 0 on error.
*/
int SmDReadMemBlock( PSMD_DEVICE p, int dsp_id, int address, unsigned size, unsigned short inc, unsigned long *val );

/** Reset and program a FPGA.
This function reset and program a FPGA DSP.
\param p Device handle of a SignalMaster Dual board.
\param fpgaType FPGA type VIRTEX is only supported (Refer to constant VIRTEX in fpgaMgr.h).
\param configMethod FPGA configuration mode SELECT_MAP_MODE is only supported (Refer to constant SELECT_MAP_MODE in fpgaMgr.h).
\param bitstream Pointer to the data of the bitstream file.
\param lenght Size of the bitstream in bytes.
\return 0 on error.
*/
int SmDFpgaReset( PSMD_DEVICE p, int fpgaId, int fpgaType, int configMethod, unsigned char bitstream[], int length);

/** Read memory from a FPGA.
This function read data in the FPGA memory.
\param p Device handle of a SignalMaster Dual board.
\param address Address in the FPGA memory.
\param size Number of DWORD to read from the FPGA memory.
\param inc Increment to make for the adress for each value (default value is 1).
\param val  pointer to the buffer to read from the FPGA memory.
\return 0 on error.
*/
int SmDReadFpgaDword( PSMD_DEVICE p, unsigned int address, unsigned short size, unsigned short inc, unsigned long *val);

/** Write memory to a FPGA.
This function write data in the FPGA memory.
\param p Device handle of a SignalMaster Dual board.
\param address Address in the FPGA memory.
\param size Number of DWORD to write in the FPGA memory.
\param inc Increment to make for the address for each value (default value is 1).
\param val  pointer to the buffer to write in the FPGA memory.
\return 0 on error.
*/
int SmDWriteFpgaDword( PSMD_DEVICE p, unsigned int address, unsigned short size, unsigned short inc, unsigned long *val);

/** Get the hardware module information.
This function retreive information of the hardware module installed with the card.
\param p Device handle of a SignalMaster Dual board.
\param pHwInfo Pointer to a structure that contains the information of the hardware module.
\return 0 on error.
*/
int SmDHardwareModuleGetBoardInfo( PSMD_DEVICE p, PSMD_HW_MODULE_INFO pHwInfo);

/** Program the FPGA on the hardware module.
This function program the FPGA on the hardware module.
\param p Device handle of a SignalMaster Dual board.
\param bitstream Pointer to the data of the bitstream file.
\param lenght Size of the bitstream in bytes.
\return 0 on error.
*/
int SmDHardwareModuleFpgaProgram( PSMD_DEVICE p, unsigned char bitstream[], unsigned int length);

/** Reset the FPGA on the hardware module.
This function reset the FPGA on the hardware module.
\param p Device handle of a SignalMaster Dual board.
\return 0 on error.
*/
int SmDHardwareModuleFpgaReset( PSMD_DEVICE p );


/** Enable a host IRQ.
This function enable and IRQ from the card to the host.
\param p Device handle of a SignalMaster Dual board.
\param irq_id Interrupt source (refer to HOSTIRQ_XXX constant).
\param event Handle from an event if the user want to be signaled with an event (can be null if the callback is used).
\param proc Pointer to a callback function if the user want that a function should be call when the interrupt occurs (can be null if the event is used).
\param user Pointer to a user informations that will be pass the the callback function (can be null).
\param hook_idx Identificator of the host IRQ.
\return 0 on error.
*/
int SmDEnableHostIrq( PSMD_DEVICE p, int irq_id, HANDLE event, SmDCallbackProc *proc, void *user, int *hook_idx );

/** Disable a host IRQ.
This function disable and IRQ from the card that was previously enable.
\param p Device handle of a SignalMaster Dual board.
\param irq_id Interrupt source (refer to HOSTIRQ_XXX constant).
\param hook_idx Identificator of the host IRQ.
\return 0 on error.
*/
int SmDDisableHostIrq( PSMD_DEVICE p, int irq_id, int hook_idx );

/** Generate a IRQ to a DSP.
This function generate an interrupt from the host to the DSP.
\param p Device handle of a SignalMaster Dual board.
\param dsp_id DSP id (1 to 4).
\return 0 on error.
*/
int SmDGenerateDspIrq( PSMD_DEVICE p, int dsp_id );

#ifdef  __cplusplus
}
#endif

#endif /* End of SMD.h */

