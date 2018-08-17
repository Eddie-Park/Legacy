/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SignalMaster                                              *
 *  Processor   : Processor independent                                     *
 *  File        : errmgr.h                                                  *
 *  Description : Error Management Routines.                                *
 *                                                                          *
 ****************************************************************************
 *
 * "$Revision: 1.1 $"
 * "$Name: 1.1 $"
 * "$Date: 2007/04/24 21:02:58 $"
 * "$Author: jf.robert $"
 *
 ****************************************************************************/

#ifndef _ERRMGR_H_
#define _ERRMGR_H_

#include "smtypes.h"

typedef enum {
	ERR_NONE,

	ERR_SM_INVALID_SMTYPE,
	ERR_SM_INVALID_TRANSPORT,
	ERR_SM_LOCKED,
	ERR_SM_UNLOCKED,
	ERR_SM_COMM_FAILURE,
	ERR_SM_NOT_PRESENT,
	ERR_SM_CONNECTED,
	ERR_SM_DISCONNECTED,
	ERR_SM_INVALIDGETID,
	ERR_SM_INVALIDSETID,
	ERR_SM_DSPRESET,
	ERR_SM_UNSUPPORTED_TRANSPORT,
	ERR_SM_BAD_ADC_CHANNEL,
	ERR_SM_BAD_DAC_CHANNEL,
	ERR_SM_BADHANDLE,
	ERR_SM_TRANSACTION_TIMEOUT,
	ERR_COMM_OPEN,
	ERR_COMM_RCV_FULL,
	ERR_COMM_TIMEOUT,
	ERR_COMM_BADCHANNEL,
	ERR_COMM_BADQUERY,
	ERR_COMM_NOTIDLE,
	ERR_COMM_NOTFULL,
	ERR_COMM_BADTRANSPORT,
	ERR_COMM_BADLENGTH,
	ERR_MSG_ALLOC_FAILURE,
	ERR_PPP_OPEN,
	ERR_PPP_RCV_FULL,
	ERR_PPP_TIMEOUT,
	ERR_PPP_BADCHANNEL,
	ERR_PPP_BADQUERY,
	ERR_PPP_TXNOTEMPTY,
	ERR_PPP_RXNOTFULL,
	ERR_PPP_BADCRC,
	ERR_UART_OPEN,
	ERR_UART_BADPORT,
	ERR_UART_RCV_FULL,
	ERR_UART_TIMEOUT,
	ERR_UART_BADCHANNEL,
	ERR_UART_BADQUERY,
	RET_CMD_UNKNOWN,
	RET_CMD_OBSOLETE,
	RET_CMD_CORRUPTED,
	RET_GENERAL_ERROR,
	RET_SM_LOCKED,
	RET_SM_UNLOCKED,
	RET_DSPFIRMWARE_FAILURE,
	RET_DSPFIRMWARE_NOTRDY,
	RET_DSP_BADPAGE,
	RET_SREC_UNSUPPORTED,
	RET_SREC_FAILURE,
	RET_SREC_INVALID,
	RET_DSPRAM_ERROR,
	RET_DSPRAM_NOTRDY,
	RET_FLASH_ERROR,
	RET_MAILBOX_TX_TIMEOUT,
	RET_MAILBOX_RX_TIMEOUT,
	ERR_SM_UNSUPPORTED_OPERATION,
	RET_SM_UNSUPPORTED_BAUDRATE,
	ERR_FILE_NOT_FOUND,
	ERR_SREC_INVALID,
	RET_MALLOC_ERROR,
	ERR_IP_OPEN,
	ERR_IP_BADCHANNEL,
	ERR_IP_BADQUERY,
	ERR_SOCKAPI_SOCKET,
	ERR_SOCKAPI_BIND,
	ERR_SOCKAPI_LISTEN,
	ERR_SOCKAPI_ACCEPT,
	ERR_SOCKAPI_CONNECT,
	ERR_SOCKAPI_IOCTLSOCKET,
	ERR_IP_CONNECT,
	ERR_IP_DISCONNECT,
	ERR_IP_MAXCONNECTIONS,
	ERR_IP_TXNOTEMPTY,
	ERR_IP_RXNOTFULL,
	ERR_COMM_TXUNDERRUN,
	ERR_COMM_RXOVERRUN,
	RET_FPGA_INVALID,
	ERR_FILE_INVALID,
	RET_BANK_INVALID,
	RET_DSPAPP_TOO_LARGE,
	ERR_WINSOCK_ERROR,
	ERR_DSPID_INVALID,
	RET_DSPID_INVALID,
	RET_FOPEN_ERROR,
	RET_DSP_INVALID,
	RET_USER_INVALID,

	// USB Error code
	ERR_USB_OPEN,
	ERR_USB_RCV_FULL,
	ERR_USB_TIMEOUT,
	ERR_USB_BADCHANNEL,
	ERR_USB_BADQUERY,
	ERR_USB_TXNOTEMPTY,
	ERR_USB_RXNOTFULL,
	ERR_USB_BADCRC,
	ERR_USB_MAXCONNECTIONS,

    ERR_HARDWARE_MODULE_NOT_DETECTED,
	ERR_QTY
} ErrorCodes;

#define RET_OK  0

#if defined(SMDLL_EXPORTS)
#define FUNC_DECL __declspec(dllexport)
#elif defined(SMDLL_IMPORTS)
#define FUNC_DECL __declspec(dllimport)
#else
#define FUNC_DECL
#endif
FUNC_DECL char *error_getDescriptor( int errorCode );

#endif /* _ERRMGR_H_ */
