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
 *  Description : SignalMaster Quad Project
 *
 *  Author(s)   : Jean-François Robert
 *
 *            Copyright (c) Lyrtech RD inc. 2006
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *  $Name: 1.6 $
 *  $Revision: 1.6 $
 *  $Date: 2009/01/05 16:36:17 $
 *  $Author: francois.blackburn $
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file rtdex.h
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

#include "smquadcxx.h"

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
\param fpga_id FPGA id (0 to 1).

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExAbortRead(PSMQUADC6XX_DEVICE p,  int fpga_id);

/** Aborts a TX to RTDEx.
This function is used to abort a TX to RTDEx.

\param p Board device structure.
\param fpga_id FPGA id (0 to 1).

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExAbortWrite(PSMQUADC6XX_DEVICE p,  int fpga_id);


/** Terminates RTDEx communications with a board.
This function terminates and cleans RTDEx communications to a board.

\param p Board device structure.

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize or
        RTDEX_RETURN_NORMAL.

 Note This function must be call after a RTDEx timeout.
*/
RTDEX_RETURN smquadcxx_RTDExClose(PSMQUADC6XX_DEVICE p);


/** Acquires RTDEx communications errors.
This function retrieves the error status register from RTDEx.

\param p Board device structure.
\param fpga_id FPGA id (0 to 1).
\param pErrorInfo Pointer to RTDEX_ERROR_INFO structure.

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExGetError(PSMQUADC6XX_DEVICE p,  int fpga_id, PRTDEX_ERROR_INFO pErrorInfo);


/** Establishes RTDEx communications with a board.
This function establishes and configures RTDEx communications with a board.

\param p Board device structure.

\return RTDEX_ERROR on error, RTDEX_BUSY if RTDEx communications are already configured
        or RTDEX_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExOpen(PSMQUADC6XX_DEVICE p);


/** RX from RTDEx.
The function reads data from a board using RTDEx.

\param p Board device structure.
\param fpga_id FPGA id (0 to 1).
\param buffer Pointer to the buffer to receive the data.
\param length Length, in bytes, of the buffer. Must be a multiple of four.

\return RTDEX_ERROR on error, RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected, RTDEX_BUSY if RTDEx is already transferring data
        or RTDEX_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExReadBuffer(PSMQUADC6XX_DEVICE p,  int fpga_id, PUCHAR buffer, ULONG length);

/** Resets an RTDEx communications channel.
The function interrupts and cleans all communications from RTDEx.

\param p Board device structure.
\param fpga_id FPGA id (0 to 1).

\return RTDEX_RETURN_ERROR on error, RTDEX_RETURN_NOT_INIT if module has not been initialize,
        RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected or RTDEX_RETURN_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExReset(PSMQUADC6XX_DEVICE p,  int fpga_id);


/** TX to RTDEx.
The function yields a return even when transfers are not complete. Users
can verify the transfer progress with \a smquadcxx_RTDExGetWriteStatus or
wait for the end of the transfer with \a smquadcxx_RTDExWaitWriteComplete.

\param p Board device structure.
\param fpga_id FPGA id (0 to 1).
\param buffer Pointer to the buffer to send to RTDEx.
\param length Length, in bytes, of the buffer. Must be a multiple of four.

\return RTDEX_ERROR on error, RTDEX_RETURN_NOT_DETECTED when RTDEx is not detected, RTDEX_BUSY if RTDEx is already transferring data
        or RTDEX_NORMAL.
*/
RTDEX_RETURN smquadcxx_RTDExWriteBuffer(PSMQUADC6XX_DEVICE p,  int fpga_id, PUCHAR buffer, ULONG length);

#ifdef  __cplusplus
}
#endif

#endif