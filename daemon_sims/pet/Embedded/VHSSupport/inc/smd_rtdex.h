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
 *  Project     : RTDEX
 *  File        : rtdex.h
 *  Description : SignalMaster Dual Project
 *
 *  Author(s)   : Jean-François Robert
 *
 *            Copyright (c) Lyrtech RD inc. 2006
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *  $Name: 1.3 $
 *  $Revision: 1.3 $
 *  $Date: 2009/01/05 16:36:25 $
 *  $Author: francois.blackburn $
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file smd_rtdex.h
    \brief .

    This API is used to make DMA transfer between the host and the FPGAs on the card.
    TX and RX are used according to the host device. Thus, TX indicates a
    transmission from the host device to RTDEx, while RX indicates a transfer
    from RTDEx to the host device.
*/

#ifndef _RTDEX_H_
#define _RTDEX_H_

#ifdef  __cplusplus
extern "C" {
#endif

/***************************************************************************
 *                           I N C L U D E S                               *
 ***************************************************************************/

#include "smd.h"

/***************************************************************************
 *                             M A C R OS                                  *
 ***************************************************************************/


/***************************************************************************
 *                          C O N S T A N T E S                            *
 ***************************************************************************/

/** \example rtdex_demo.c
 * This is an example of how to use the RTDEx host API.
 * Check the module for details.
 */

/**
* RTDEx return status.
*/
typedef enum {
    RTDEX_RETURN_ERROR,         /*!< Returned with error. */
    RTDEX_RETURN_NORMAL,        /*!< Returned without error. */
    RTDEX_RETURN_BUSY,          /*!< Returned while RTDEx is already working. */
    RTDEX_RETURN_NOT_INIT,      /*!< Returned when the module has not been initialize. */
    RTDEX_RETURN_NOT_DETECTED,  /*!< Returned when RTDEx is not present on a FPGA. */
    RTDEX_RETURN_TIMEOUT        /*!< Returned when a timeout occurs on a transfer. */
} RTDEX_RETURN;

/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

/**
* RTDEx error information.
*/
typedef struct __RTDEX_ERROR_INFO {
  ULONG rxOverrun;              /*!< RX OVERRUN Errors Count */
  ULONG txUnderrun;             /*!< TX UNDERRUN Errors Count */
} RTDEX_ERROR_INFO, *PRTDEX_ERROR_INFO;

/***************************************************************************
 *                           V A R I A B L E S                             *
 ***************************************************************************/

/***************************************************************************
 *                       P U B L I C  F U N C T I O N S                    *
 ***************************************************************************/

/** Aborts an RX from RTDEx.
This function is used to abort an RX from RTDEx.

\param p Board device structure.

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smd_RTDExAbortRead(PSMD_DEVICE p);

/** Aborts a TX to RTDEx.
This function is used to abort a TX to RTDEx.

\param p Board device structure.

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smd_RTDExAbortWrite(PSMD_DEVICE p);


/** Terminates RTDEx communications with a board.
This function terminates and cleans RTDEx communications to a board.

\param p Board device structure.

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize or
        RTDEX_RETURN_NORMAL.

 Note This function must be call after a RTDEx timeout.
*/
RTDEX_RETURN smd_RTDExClose(PSMD_DEVICE p);


/** Acquires RTDEx communications errors.
This function retrieves the error status register from RTDEx.

\param p Board device structure.

\param pErrorInfo Pointer to RTDEX_ERROR_INFO structure.

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smd_RTDExGetError(PSMD_DEVICE p, PRTDEX_ERROR_INFO pErrorInfo);


/** Establishes RTDEx communications with a board.
This function establishes and configures RTDEx communications with a board.

\param p Board device structure.

\return RTDEX_ERROR on error, RTDEX_BUSY if RTDEx communications are already configured
        or RTDEX_NORMAL.
*/
RTDEX_RETURN smd_RTDExOpen(PSMD_DEVICE p);


/** RX from RTDEx.
The function reads data from a board using RTDEx.

\param p Board device structure.

\param buffer Pointer to the buffer to receive the data.
\param length Length, in bytes, of the buffer. Must be a multiple of four.

\return RTDEX_ERROR on error, RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected, RTDEX_BUSY if RTDEx is already transferring data
        or RTDEX_NORMAL.
*/
RTDEX_RETURN smd_RTDExReadBuffer(PSMD_DEVICE p, PUCHAR buffer, ULONG length);

/** Resets an RTDEx communications channel.
The function interrupts and cleans all communications from RTDEx.

\param p Board device structure.


\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smd_RTDExReset(PSMD_DEVICE p);


/** TX to RTDEx.
The function yields a return even when transfers are not complete. Users
can verify the transfer progress with \a smd_RTDExGetWriteStatus or
wait for the end of the transfer with \a smd_RTDExWaitWriteComplete.

\param p Board device structure.

\param buffer Pointer to the buffer to send to RTDEx.
\param length Length, in bytes, of the buffer. Must be a multiple of four.

\return RTDEX_ERROR on error, RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected, RTDEX_BUSY if RTDEx is already transferring data
        or RTDEX_NORMAL.
*/
RTDEX_RETURN smd_RTDExWriteBuffer(PSMD_DEVICE p, PUCHAR buffer, ULONG length);

#ifdef  __cplusplus
}
#endif

#endif