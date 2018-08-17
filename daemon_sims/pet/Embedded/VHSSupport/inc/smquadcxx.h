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
 *  Project     : SignalMaster Quad
 *  File        : smquadcxx.h
 *  Description : SignalMaster Quad Project
 *
 *  Author(s)   : Jean-François Robert
 *
 *            Copyright (c) Lyrtech RD inc. 2007
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *  $Name: 1.12 $
 *  $Revision: 1.12 $
 *  $Date: 2008/07/22 21:24:25 $
 *  $Author: francois.blackburn $
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file smquadcxx.h
    \brief SignalMaster Quad host API.

    This section describes all the functions used by SignalMaster Quad host API.
    Refer to this API first when developing applications meant to communicate
    with the SignalMaster Quad, as they contain application-specific routines.
*/

#ifndef __SMQUADCXX_INC
#define __SMQUADCXX_INC

#ifdef  __cplusplus
extern "C" {
#endif


/***************************************************************************
 *                           I N C L U D E S                               *
 ***************************************************************************/

#include "fpgaMgr.h"
#include <windows.h>

/***************************************************************************
 *                             M A C R OS                                  *
 ***************************************************************************/


/***************************************************************************
 *                          C O N S T A N T E S                            *
 ***************************************************************************/

#define SMQUADC6XX_MAX_DEVICES  16
#define SMQUADC6XX_MAX_PIPES  256

// Supported DSP types
#define DSP_TYPE_UNKNOWN    0       /*!< DSP type not defined. */
#define DSP_TYPE_TMS320C6713  6713  /* @deprecated TMS320C6713 DSP type (Used with SignalMaster Quad Virtex-II). */
#define DSP_TYPE_TMS320C6416T 6416  /*!< TMS320C6416T DSP type. */



/*!
   GBits Ethernet on link 2 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the GBits Ethernet on link 2.
*/
#define HW_MODULE_INTERFACE_GBIT_ETHERNET_LNK_2 0x0001

/*!
   GBits Ethernet on link 1 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the GBits Ethernet on link 1.
*/
#define HW_MODULE_INTERFACE_GBIT_ETHERNET_LNK_1 0x0002

/*!
   FPGA load masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the module FPGA serial loading port.
*/
#define HW_MODULE_INTERFACE_FPGA_LOAD           0x0004

/*!
   Rapid channel RX masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Rapid channel RX interface.
*/
#define HW_MODULE_INTERFACE_RAPID_CHANNEL_RX    0x0008

/*!
   Rapid channel TX masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Rapid channel TX interface.
*/
#define HW_MODULE_INTERFACE_RAPID_CHANNEL_TX    0x0010

/*!
   Synchronous bus 2 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the synchronous bus 2 interface.
*/
#define HW_MODULE_INTERFACE_SYNC_BUS_2          0x0020

/*!
   Synchronous bus 1 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the synchronous bus 1 interface.
*/
#define HW_MODULE_INTERFACE_SYNC_BUS_1          0x0040

/*!
   Asynchronous bus masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the asynchronous bus interface.
*/
#define HW_MODULE_INTERFACE_ASYNC_INT           0x0080
#define HW_MODULE_INTERFACE_UNDEFINED1          0x0100
#define HW_MODULE_INTERFACE_UNDEFINED2          0x0200
#define HW_MODULE_INTERFACE_UNDEFINED3          0x0400

/*!
   Serial port 2 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Serial port 2 interface.
*/
#define HW_MODULE_INTERFACE_SERIAL_PORT_2       0x0800

/*!
   Serial port 1 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Serial port 1 interface.
*/
#define HW_MODULE_INTERFACE_SERIAL_PORT_1       0x1000

/*!
   Backplane I/O 16 to 30 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the backplane I/O 16 to 30.
*/
#define HW_MODULE_INTERFACE_BACKPLANE_IO_16_30  0x2000

/*!
   Backplane I/O 5 to 15 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the backplane I/O 5 to 15.
*/
#define HW_MODULE_INTERFACE_BACKPLANE_IO_5_15   0x4000

/*!
   Backplane I/O 0 to 7 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the backplane I/O 0 to 7.
*/
#define HW_MODULE_INTERFACE_BACKPLANE_IO_0_7    0x8000


/**
* Hardware module version.
*/
typedef enum {
    HW_MODULE_VERSION_LYRIO = 0,    /*!< Hardware Module version without RapidCHANNEL. */
    HW_MODULE_VERSION_LYRIO_PLUS    /*!< Hardware Module version with RapidCHANNEL. */
} SMQUADC6XX_HW_MODULE_VERSION;

/**
* Hardware module identification.
*/
typedef enum {
    HW_MODULE_ID_ADAC_MASTER_III = 0,   /*!< ADAC Master III. */
    HW_MODULE_ID_DRC_V4,                 /*!< DRC V4. */
    HW_MODULE_ID_CAMERA_LINK
} SMQUADC6XX_HW_MODULE_ID;

/**
* Hardware module revision.
*/
typedef enum {
    HW_MODULE_REVISON_A = 0,        /*!< Revision A. */
    HW_MODULE_REVISON_B,            /*!< Revision B. */
    HW_MODULE_REVISON_C,            /*!< Revision C. */
    HW_MODULE_REVISON_D             /*!< Revision D. */
} SMQUADC6XX_HW_MODULE_REVISON;

/**
* FPGA type on the hardware module.
*/
typedef enum {
    HW_MODULE_FPGA_ID_NONE = 0,     /*!< No FPGA. */
    HW_MODULE_FPGA_ID_XC4VLX25,     /*!< FPGA XC4VLX25. */
    HW_MODULE_FPGA_ID_XC4VLX35      /*!< FPGA XC4VLX35. */
} SMQUADC6XX_HW_MODULE_FPGA_ID;

/**
* Host IRQ Sources.
*/
typedef enum {
    HOSTIRQ_DSP1    = 0,    /*!< IRQ from DSP 1. */
    HOSTIRQ_DSP2,           /*!< IRQ from DSP 2. */
    HOSTIRQ_DSP3,           /*!< IRQ from DSP 3. */
    HOSTIRQ_DSP4,           /*!< IRQ from DSP 4. */
    HOSTIRQ_FPGA1,          /*!< IRQ from FPGA 1. */
    HOSTIRQ_FPGA2           /*!< IRQ from FPGA 2. */
} SMQUADC6XX_HOSTIRQ;


/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

typedef struct __PLX9656_DEVICE
{
  // WIN32 handles
  HANDLE  hHandle;  // General Purpose Handle (WINNT specific)

  // PLX9656 Information
  unsigned int pciBase0Addr;  /* Run-Time Regs (Memory Space) */
  unsigned int pciBase1Addr;  /* Run-Time Regs (I/O Space) */
  unsigned int pciBase2Addr;  /* Control CPLD (Memory Space) */
  unsigned int pciBase3Addr;  /* FPGAs (Memory Space) */
  unsigned int pciMap2;
  unsigned int pciMap3;
  unsigned int irqPin;
  unsigned int irqLine;
  unsigned int system;

}
PLX9656_DEVICE, *PPLX9656_DEVICE;


/**
* Hardware module Information.
*/
typedef struct __SMQUADC6XX_HW_MODULE_INFO
{
    unsigned char   ucIsPresent;        /*!< TRUE if a module is detected */
    unsigned char   ucVersion;          /*!< See HW_MODULE_VERSION_XXX */
    unsigned char   ucModuleId;         /*!< See HW_MODULE_ID_XXX*/
    unsigned char   ucHardOpt;          /*!< Variable */
    unsigned char   ucHwRevision;       /*!< See HW_MODULE_REVISON_XXX */
    unsigned char   ucAlign1;
    unsigned short  usBmInterface;      /*!< Bitmask. See HW_MODULE_INTERFACE_XXXX */
    unsigned short  usEepromLenght;     /*!< Eeprom length */
    unsigned char   ucFpgaId;           /*!< See HW_MODULE_FPGA_ID_XXXX */
    unsigned char   ucAlign2;
    unsigned short  usVarious;          /*!< User defined */
    unsigned short  usChecksum;         /*!< Checksum */
} SMQUADC6XX_HW_MODULE_INFO, *PSMQUADC6XX_HW_MODULE_INFO;


// SMQUADC6XX Device Information
typedef struct __SMQUADC6XX_DEVICE_INFO
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
SMQUADC6XX_DEVICE_INFO, *PSMQUADC6XX_DEVICE_INFO;

// All SMC6XX Devices Information
typedef struct __SMQUADC6XX_ALL_DEVICES_INFO
{
  unsigned int NumberOfBoards;
  unsigned int BoardID;
  SMQUADC6XX_DEVICE_INFO DeviceInfo[SMQUADC6XX_MAX_DEVICES];
}
SMQUADC6XX_ALL_DEVICES_INFO, *PSMQUADC6XX_ALL_DEVICES_INFO;


// SMCXX Device Definition
/**
* SignalMaster Quad device definition.
*/
typedef struct __SMQUADC6XX_DEVICE
{
  // Magic Number (public)
#define SMQUADCXX_DEVICE_MAGIC_NUMBER 0x51433658  // ASCII => "QC6X"
  int MagicNumber;

  // Version Control (public)
  // (changing this version will programs to be recompiled and verified)
#define SMQUADCXX_DEVICE_VERSION_MAJOR  0

#define SMQUADCXX_DEVICE_VERSION_MINOR  0
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

  PLX9656_DEVICE  plx9656;
  SMFPGA_DEVICE fpga[2];

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
  int     IsV4; // indicate if the quad support direct addressing on plx bus

  // Addressing stuff
  int ControlRegOffset[8];
  int DeviceBaseAddr[4];
  int DspHpiOffset[4][5]; /* 4 DSP */

  int bModuleEepromIsInit;

  // WIN32 handles
  HANDLE  hHandle;  // General Purpose Handle (WINNT specific)
  HANDLE  hMutex;   // Mutex for Conflict Prevension

  // Pipe Information
  int   NumberOfPipes;
  int   PipeReadSize[SMQUADC6XX_MAX_PIPES];
  int   PipeWriteSize[SMQUADC6XX_MAX_PIPES];

  //FCO
  HANDLE  ReadPipeEvents[128];

    // Hardware module
    SMQUADC6XX_HW_MODULE_INFO     hwModuleInfo[2];

  // Pointer to private data structures
  void  *pPrivate;
}
SMQUADC6XX_DEVICE, *PSMQUADC6XX_DEVICE;

// SMCXX Device Information for EPK
typedef struct __SMQUADC6XX_DEVICE_FOR_EPK
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
SMQUADC6XX_DEVICE_FOR_EPK, *PSMQUADC6XX_DEVICE_FOR_EPK;


typedef struct __SMQUADC6XX_BUFFER
{
  unsigned int  Target;
  unsigned int  Channel;
  unsigned int  Length;
  unsigned int  *pMemory;
  void      (*pCallback)( PSMQUADC6XX_DEVICE pDev, struct __SMQUADC6XX_BUFFER *pBuffer );
  unsigned int  TransferLength;
  unsigned int  TransferStatus;
  struct __SMC6XX_BUFFER *pNext;
}
SMQUADC6XX_BUFFER, *PSMQUADC6XX_BUFFER;

/**
* Host IRQ callback function.
*/
typedef void (*SmQuadcxxCallbackProc)( PSMQUADC6XX_DEVICE p, void *user );


/***************************************************************************
 *                           V A R I A B L E S                             *
 ***************************************************************************/

/***************************************************************************
 *                       P U B L I C  F U N C T I O N S                    *
 ***************************************************************************/

/** Initialize the library and the SignalMaster Quad board.
This function must be call before using any other function. The function detect
and initialize all the card detected.

\return 0 on error.
*/
int SmQuadcxxInit( void );

/** Find a device handle.
This function finds a SignalMaster Quad card using an identifier that goes
from 0 to the number of detected SignalMaster Quad cards minus one. The user can determine the number
of cards detected by calling the function repeatedly, incrementing the identifier from 0 until
a NULL pointer is returned. Please refer to the example.
\param id Identificator of a board
\return Pointer of a device handle of a SignalMaster Quad board.
*/
PSMQUADC6XX_DEVICE SmQuadcxxFindDevice( int id );

/** Quit the library.
This function free all the resources allocated in the initialization of the library.
\param id Identificator of a board
\return 0 on error.
*/
int SmQuadcxxQuit( void );


/* DSP Routines */
/** Reset a DSP.
This function reset the DSP.
\param p Device handle of a SignalMaster Quad board.
\param dsp_id DSP id (1 to 4).
\return 0 on error.
*/
int SmQuadcxxDspReset( PSMQUADC6XX_DEVICE p, int dsp_id );

/** Load an executable file to a DSP.
This function load an executable file (a COFF file) to the DSP and execute it.
\param p Device handle of a SignalMaster Quad board.
\param dsp_id DSP id (1 to 4).
\param coff_file Pointer to the data of an executable file for a DSP.
\param len Size of the execute file in bytes.
\return 0 on error.
*/
int SmQuadcxxDspLoad( PSMQUADC6XX_DEVICE p, int dsp_id, unsigned char *coff_file, unsigned int len );

/** Write to DSP memory.
This function write data in the DSP memory.
\param p Device handle of a SignalMaster Quad board.
\param dsp_id DSP id (1 to 4).
\param address Address in the DSP memory.
\param size Number of unsigned long to write in the DSP memory.
\param inc Increment to make for the address for each value (default value is 1).
\param val  pointer to the buffer to write in the DSP memory.
\return 0 on error.
*/
int SmQuadcxxWriteMemBlock( PSMQUADC6XX_DEVICE p, int dsp_id, int address, unsigned size, unsigned short inc, unsigned long *val );

/** Read from the DSP memory.
This function read data in the DSP memory.
\param p Device handle of a SignalMaster Quad board.
\param dsp_id DSP id (1 to 4).
\param address Address in the DSP memory.
\param size Number of unsigned long to read from the DSP memory.
\param inc Increment to make for the address for each value (default value is 1).
\param val  pointer to the buffer to read from the DSP memory.
\return 0 on error.
*/
int SmQuadcxxReadMemBlock( PSMQUADC6XX_DEVICE p, int dsp_id, int address, unsigned size, unsigned short inc, unsigned long *val );

/** Reset and program a FPGA.
This function reset and program a FPGA DSP.
\param p Device handle of a SignalMaster Quad board.
\param fpga_id FPGA id (1 to 2).
\param fpgaType FPGA type VIRTEX is only supported (Refer to constant VIRTEX in fpgaMgr.h).
\param configMethod FPGA configuration mode SELECT_MAP_MODE is only supported (Refer to constant SELECT_MAP_MODE in fpgaMgr.h).
\param bitstream Pointer to the data of the bitstream file.
\param lenght Size of the bitstream in bytes.
\return 0 on error.
*/
int SmQuadcxxFpgaReset( PSMQUADC6XX_DEVICE p, int fpga_id, int fpgaType, int configMethod, unsigned char bitstream[], int length);

/** Read memory from a FPGA.
This function read data in the FPGA memory.
\param p Device handle of a SignalMaster Quad board.
\param fpga_id FPGA id (0 to 1).
\param address Address in the FPGA memory.
\param size Number of DWORD to read from the FPGA memory.
\param inc Increment to make for the adress for each value (default value is 1).
\param val  pointer to the buffer to read from the FPGA memory.
\return 0 on error.
*/
int SmQuadcxxReadFpgaDword( PSMQUADC6XX_DEVICE p, int fpga_id, unsigned int address, unsigned short size, unsigned short inc, unsigned long *val);

/** Write memory to a FPGA.
This function write data in the FPGA memory.
\param p Device handle of a SignalMaster Quad board.
\param fpga_id FPGA id (0 to 1).
\param address Address in the FPGA memory.
\param size Number of DWORD to write in the FPGA memory.
\param inc Increment to make for the address for each value (default value is 1).
\param val  pointer to the buffer to write in the FPGA memory.
\return 0 on error.
*/
int SmQuadcxxWriteFpgaDword( PSMQUADC6XX_DEVICE p, int fpga_id, unsigned int address, unsigned short size, unsigned short inc, unsigned long *val);

/** Get the hardware module information.
This function retreive information of the hardware module installed with the card.
\param p Device handle of a SignalMaster Quad board.
\param fpga_id FPGA id (0 to 1).
\param pHwInfo Pointer to a structure that contains the information of the hardware module.
\return 0 on error.
*/
int SmQuadcxxHardwareModuleGetBoardInfo( PSMQUADC6XX_DEVICE p, int fpgaid, PSMQUADC6XX_HW_MODULE_INFO pHwInfo);

/** Program the FPGA on the hardware module.
This function program the FPGA on the hardware module.
\param p Device handle of a SignalMaster Quad board.
\param fpga_id FPGA id (0 to 1).
\param bitstream Pointer to the data of the bitstream file.
\param lenght Size of the bitstream in bytes.
\return 0 on error.
*/
int SmQuadcxxHardwareModuleFpgaProgram( PSMQUADC6XX_DEVICE p, int fpgaid, unsigned char bitstream[], unsigned int length);

/** Reset the FPGA on the hardware module.
This function reset the FPGA on the hardware module.
\param p Device handle of a SignalMaster Quad board.
\param fpga_id FPGA id (0 to 1).
\return 0 on error.
*/
int SmQuadcxxHardwareModuleFpgaReset( PSMQUADC6XX_DEVICE p, int fpgaid);


/** Enable a host IRQ.
This function enable and IRQ from the card to the host.
\param p Device handle of a SignalMaster Quad board.
\param irq_id Interrupt source (refer to HOSTIRQ_XXX constant).
\param event Handle from an event if the user want to be signaled with an event (can be null if the callback is used).
\param proc Pointer to a callback function if the user want that a function should be call when the interrupt occurs (can be null if the event is used).
\param user Pointer to a user informations that will be pass the the callback function (can be null).
\param hook_idx Identificator of the host IRQ.
\return 0 on error.
*/
int SmQuadcxxEnableHostIrq( PSMQUADC6XX_DEVICE p, int irq_id, HANDLE event, SmQuadcxxCallbackProc *proc, void *user, int *hook_idx );

/** Disable a host IRQ.
This function disable and IRQ from the card that was previously enable.
\param p Device handle of a SignalMaster Quad board.
\param irq_id Interrupt source (refer to HOSTIRQ_XXX constant).
\param hook_idx Identificator of the host IRQ.
\return 0 on error.
*/
int SmQuadcxxDisableHostIrq( PSMQUADC6XX_DEVICE p, int irq_id, int hook_idx );

/** Generate a IRQ to a DSP.
This function generate an interrupt from the host to the DSP.
\param p Device handle of a SignalMaster Quad board.
\param dsp_id DSP id (1 to 4).
\return 0 on error.
*/
int SmQuadcxxGenerateDspIrq( PSMQUADC6XX_DEVICE p, int dsp_id );

#ifdef  __cplusplus
}
#endif

#endif /* End of SMCXX.h */

