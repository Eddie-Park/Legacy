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
 *  File        : smd_core.h
 *  Description : SignalMaster Dual Project
 *
 *  Author(s)   : Francois Blackburn
 *
 *            Copyright (c) Lyrtech RD inc. 2007
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *  $Name: 1.1 $
 *  $Revision: 1.1 $
 *  $Date: 2008/06/20 19:22:17 $
 *  $Author: francois.blackburn $
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file smd_core.h
    \brief SignalMaster Dual host API.

    This section describes all the functions used by SignalMaster Dual host API.
    Refer to this API first when developing applications meant to communicate
    with the SignalMaster Dual, as they contain application-specific routines.
*/

#ifndef __SMD_COMMON_INC
#define __SMD_COMMON_INC

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

// Supported DSP types
#define SMD_DSP_TYPE_UNKNOWN    0       /*!< DSP type not defined. */
#define SMD_DSP_TYPE_TMS320C6416T 6416  /*!< TMS320C6416T DSP type. */



/*!
   GBits Ethernet on link 2 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the GBits Ethernet on link 2.
*/
#define SMD_HW_MODULE_INTERFACE_GBIT_ETHERNET_LNK_2 0x0001

/*!
   GBits Ethernet on link 1 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the GBits Ethernet on link 1.
*/
#define SMD_HW_MODULE_INTERFACE_GBIT_ETHERNET_LNK_1 0x0002

/*!
   FPGA load masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the module FPGA serial loading port.
*/
#define SMD_HW_MODULE_INTERFACE_FPGA_LOAD           0x0004

/*!
   Rapid channel RX masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Rapid channel RX interface.
*/
#define SMD_HW_MODULE_INTERFACE_RAPID_CHANNEL_RX    0x0008

/*!
   Rapid channel TX masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Rapid channel TX interface.
*/
#define SMD_HW_MODULE_INTERFACE_RAPID_CHANNEL_TX    0x0010

/*!
   Synchronous bus 2 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the synchronous bus 2 interface.
*/
#define SMD_HW_MODULE_INTERFACE_SYNC_BUS_2          0x0020

/*!
   Synchronous bus 1 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the synchronous bus 1 interface.
*/
#define SMD_HW_MODULE_INTERFACE_SYNC_BUS_1          0x0040

/*!
   Asynchronous bus masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the asynchronous bus interface.
*/
#define SMD_HW_MODULE_INTERFACE_ASYNC_INT           0x0080
#define SMD_HW_MODULE_INTERFACE_UNDEFINED1          0x0100
#define SMD_HW_MODULE_INTERFACE_UNDEFINED2          0x0200
#define SMD_HW_MODULE_INTERFACE_UNDEFINED3          0x0400

/*!
   Serial port 2 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Serial port 2 interface.
*/
#define SMD_HW_MODULE_INTERFACE_SERIAL_PORT_2       0x0800

/*!
   Serial port 1 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the Serial port 1 interface.
*/
#define SMD_HW_MODULE_INTERFACE_SERIAL_PORT_1       0x1000

/*!
   Backplane I/O 16 to 30 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the backplane I/O 16 to 30.
*/
#define SMD_HW_MODULE_INTERFACE_BACKPLANE_IO_16_30  0x2000

/*!
   Backplane I/O 5 to 15 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the backplane I/O 5 to 15.
*/
#define SMD_HW_MODULE_INTERFACE_BACKPLANE_IO_5_15   0x4000

/*!
   Backplane I/O 0 to 7 masking bit. Use to find, in the hardware module
   info structure \a usBmInterface,  if hardware module use the backplane I/O 0 to 7.
*/
#define SMD_HW_MODULE_INTERFACE_BACKPLANE_IO_0_7    0x8000


/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

typedef struct __SMD_PLX9656_DEVICE
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
SMD_PLX9656_DEVICE, *PSMD_PLX9656_DEVICE;


#ifdef  __cplusplus
}
#endif

#endif /* End of SMCXX.h */

