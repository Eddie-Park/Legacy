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
 *  Project     : VHS-ADC/DAC HOST API
 *  File        : vhsadac16_host_api.h
 *  Description : WIN32 API for VHS-ADC/DAC Boards.
 *
 *  Author(s)   : Jérémie Plante
 *
 *						Copyright (c) Lyrtech inc. 2005
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *
 *--------------------------------------------------------------------------
 *  Date       Author    Version    Description
 *--------------------------------------------------------------------------
 *  02/09/05   JPLA      1.00       WDM Driver port & cleanup.
 *  09/09/05   JPLA      1.10       RTDEx Profiling IOCTLs added.
 *  10/10/05   JPLA      2.00       Major Refactoring...
 *  19/04/06   JPLA      2.20       Doxygenification (API DOC!)
 ***************************************************************************
 *  "$RCSfile: vhsadac16_host_api.h,v $"
 *  "$Author: michel.mercier $"
 *  "$Date: 2008/12/03 15:51:49 $"
 ***************************************************************************/
/*! \file vhsadac16_host_api.h
  \brief VHS-ADC/DAC Win32 API.

  This section describes all the functions used by the low-level host API.<BR>
  The low-level host API is specific to host CPU applications with which
  you can create and manage handles to communicate with the VHS-ADC/DAC,
  as well as access FPGA and CPLD registers directly through the handles.
  <BR>Refer to the VHS-ADC/DAC documentation for details about
  available registers.
*/
#ifndef _VHSADAC16_API_H_
#define _VHSADAC16_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>


#define VHSADC16_XC2V3000_ID		0x82
#define VHSADC16_XC2V4000_ID		0x83
#define VHSADC16_XC2V6000_ID		0x84
#define VHSADC16_XC2V8000_ID		0x85
#define VHSDAC16_XC2V3000_ID		0x86
#define VHSDAC16_XC2V4000_ID		0x87
#define VHSDAC16_XC2V6000_ID		0x88
#define VHSDAC16_XC2V8000_ID		0x89
#define VHSDAC16_XC4LX100_ID		0x8A
#define VHSDAC16_XC4LX160_ID		0x8B
#define VHSDAC16_XC4SX55_ID			0x8C
#define VHSADC16_XC4LX100_ID		0x8D
#define VHSADC16_XC4LX160_ID		0x8E
#define VHSADC16_XC4SX55_ID			0x8F

/**
 * All VHS-ADC/DAC types.
 */
typedef enum{
	BT_VHSADC16,                    /*!< VHS-ADC */
	BT_VHSDAC16                     /*!< VHS-DAC */
} VHSADAC16_BOARD_TYPE;

/*
 * VHS CPLD status.
 */
typedef struct _VHSADAC_CPLD_REGS
{
	UCHAR	virtex_pgm;             /*!< VIRTEX PGM */
	UCHAR	adc_speed_status;       /*!< ADC SPEED STATUS */
	UCHAR	cpld_status;            /*!< CPLD STATUS */
	UCHAR	cpld_revision;          /*!< CPLD REVISION */
	UCHAR	pcb_revision;           /*!< PCB BOARD REVISION */
	UCHAR	sm_board_id;            /*!< BOARD IDENTIFIER */
}
VHSADAC_CPLD_REGS,*PVHSADAC_CPLD_REGS;

/*
 * VHS ERROR (Overrun/Underrun) OutInfo.
 */
typedef struct _VHSADAC_GET_RTDEX_ERROR_OUTINFO {
	ULONG	rxOverrun;              /*!< RX OVERRUN Errors Count */
	ULONG	txUnderrun;             /*!< TX UNDERRUN Errors Count */
} VHSADAC_RTDEX_ERROR_INFO, *PVHSADAC_RTDEX_ERROR_INFO;


//////////////////////////////////////////////////////////////////////////////////////////////
// VHS Boards Node struct. Used by global vhsBoards LinkedList
// INTERNAL USE ONLY!
typedef struct _VHSADAC_BOARD_Node
{
	HANDLE  h; /// Board HANDLE (VALID & OPENED SINGLE INSTANCE)

	HANDLE  hMutex;      // Mutual Exclusion handle

	UINT    boardId;     // Board IDENTIFIER.
	UINT    boardNumber; // Board # (ie DAC #3 and ADC #2...)

	INT		status;      // VHSADAC16_BOARD_STATUS Current Board Status...

	INT     lastError;   // Last VHSADACreported for this board.

	VHSADAC16_BOARD_TYPE boardType;   // Type of board ADC OR DAC.

	ULONG   rxCnt, txCnt;// RX/TX # of bytes transfered so far
	ULONG   rxQty, txQty;// TX/TX Total # of bytes to transfer

	struct _VHSADAC_BOARD_Node *next; // Linked List next pointer...
} VHSADAC_BOARD_NODE, *PVHSADAC_BOARD_NODE;

// VHS Profiling struct
// INTERNAL USE ONLY!
typedef struct _VHSADAC_PROFILING_OUTINFO {
	ULONG	rxCnt;
	ULONG	txCnt;
	ULONG	rxTime; // milliseconds!
	ULONG	txTime; // milliseconds!
	ULONG   systemTimeIncrement;
} VHSADAC_PROFILING_INFO, *PVHSADAC_PROFILING_INFO;

/**
 * VHS-ADC/DAC status.
 */
typedef enum{
	INVALID_STATUS=0,               /*!< Invalid status */
	IDLE,                           /*!< Idle status */
	READING,                        /*!< Reading status */
	WRITING,                        /*!< Writing status */
	QUERYING,                       /*!< Querying status */
	RESETTING,                      /*!< Resetting status */
	ABORTING,                       /*!< Aborting status */
	CONFIGURING,                    /*!< Configuring status */
	PROGRAMMING                     /*!< Programming status */
} VHSADAC16_BOARD_STATUS;

/**
 * VHS-ADC/DAC errors.
 */
typedef enum{
	BUSY=0,                         /*!< VHS-ADC/DAC is busy */
	ABORTED,                        /*!< Aborted command */
	FAILED_IOCTL,                   /*!< IOCTL failed */
	READ_ERROR,                     /*!< VHS-ADC/DAC read error */
	WRITE_ERROR,                    /*!< VHS-ADC/DAC write error */
	PRELOAD_ERROR,                  /*!< VHS-ADC/DAC preload write error */
	UNKNOWN_ERROR,                  /*!< Unknown VHS-ADC/DAC error */
	BOARD_NOT_FOUND                 /*!< VHS-ADC/DAC is not found */
} VHSADAC16_BOARD_ERROR;

// INTERNAL USE ONLY!
typedef struct _VHSADAC_V360_DEBUG_OUTINFO{
	USHORT	pciDevice;
	USHORT	pciVendor;
	USHORT  pciStat;
	USHORT  pciCmd;
	ULONG   pciCcRev;
	ULONG   pciHdrCfg;
	ULONG   pciIntStat;
	ULONG   pciIntCfg;

	USHORT  fifoCfg;
	USHORT  lbCfg;
	USHORT  system;
	USHORT  pciCfg;

	UCHAR   dmaCsr0;
	UCHAR   dmaCsr1;
	ULONG   dmaLength0;
	ULONG   dmaLength1;
} VHSADAC_V360_DEBUG_INFO, *PVHSADAC_V360_DEBUG_INFO;

/**
* Vhsadac16_FpgaLoad return values
*/
typedef enum {
    RETURN_SUCCESS=0,               /*!< No error */
    BITSTREAM_FILENOTFOUND,         /*!< File not found */
    BOARD_NOT_IDLE,                 /*!< VHS-ADC/DAC is not idle */
    UNMONITORED_TRANSFER,           /*!< Unmonitored transfer not supported */
    UNKNOW_PLATFORM,                /*!< Unknow platform */
    FPGALOADRETURNRESERVED,         /*!< Reserved */
    GETCPLDREGSERR,                 /*!< Could not read CPLD */
    INVALIDBITSTREAM,               /*!< The bitstream was not built for the current type of FPGA */
    INVALID_PREAMBLE,               /*!< Invalid preamble in the header of the bitstream */
} FPGALOADRETURN;


// Useless struct left there so there's no changes to Vhsadac16_Open() API...
// UNUSED!
typedef struct _VHSADC16_Context
{
	HANDLE		hIRQEvent;
} VHSADC16CONTEXT,*PVHSADC16CONTEXT;

//////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Performs the basic initialization of all the devices.
 *
 * \param[in] pContext Unused.
 *
 * \return Number of detected VHS-ADC/DAC.
 *
 */
extern ULONG	Vhsadac16_Open(PVHSADC16CONTEXT pContext);

/**
 * Returns the handle of the requested board with the board_num (counting from the left) to initialize communications.
 *
 * \param[in] board_type VHS-ADC/DAC Type.
 * \param[in] board_num VHS-ADC/DAC number in the system.
 *
 * \return 1 if successful, 0 otherwise.
 *
 * \todo Return valid Handle or INVALID_HANDLE instead!
 */
extern ULONG    Vhsadac16_OpenBoard(VHSADAC16_BOARD_TYPE board_type, UCHAR board_num);

/**
 * Returns the opened handle of a specific VHS-ADC/DAC, if found.
 *
 * This function will lookup every opened handle to find a board in particular.
 *
 * \param[in] board_type VHS-ADC/DAC Type.
 * \param[in] board_num VHS-ADC/DAC number in the system.
 *
 * \return A valid VHS-ADC/DAC handle if the board is found or INVALID_HANDLE_VALUE.
 */
extern HANDLE	Vhsadac16_getHandle(VHSADAC16_BOARD_TYPE board_type, UCHAR board_num);

/**
 * Returns the ID of the specified type and number of VHS-ADC/DAC.
 *
 * \param[in] board_type VHS-ADC/DAC Type.
 * \param[in] board_num VHS-ADC/DAC number in the system.
 *
 * \return VHS-ADC/DAC Identifier.
 */
extern LONG	Vhsadac16_getBoardID(VHSADAC16_BOARD_TYPE board_type, UCHAR board_num);


/**
* Verifies whether the programmable gain is available for RF inputs/outputs
*
* \param[in] h VHS-ADC/DAC handle.
*
* \return 1 if programmable gain is available, 0 if it is not
*/
extern LONG Vhsadac16_HasProgrammableGain(HANDLE h);

/**
 * Programs VHS-ADC/DAC FPGA using a bitstream filename.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] bitstream Name of the bitstream file to be loaded, with complete path information.
 *
 * \return 0 if VHS-ADC/DAC FPGA is successfully programmed.
 */
extern LONG	Vhsadac16_FpgaLoad(HANDLE h, UCHAR* bitstream);

/**
 * Low-level VHS-ADC/DAC FPGA programming routine using a pointer to a valid bitstream.
 *
 * To program a VHS-ADC/DAC, load its FPGA with a bitstream. This function does not validate the bitstream before programming it.
 * Use the function with care.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] pBitstream Pointer to the bitstream to be programmed.
 * \param[in] length Length of the bitstream.
 *
 * \return 0 if the VHS-ADC/DAC FPGA is successfully programmed.
 */
extern LONG	Vhsadac16_FpgaReset(HANDLE h, UCHAR* pBitstream, ULONG length);

/**
 * Returns the contents of a VHS-ADC/DAC CPLD registers.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[out] pRegs Pointer to the returned VHSADAC_CPLD_REGS struct.
 *
 * \return 0 on success. A non zero value means there has been an error reading the CPLD registers.
 */
extern LONG	Vhsadac16_GetCpldRegs(HANDLE h, PVHSADAC_CPLD_REGS pRegs);

/**
 * Reads a CPLD register.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] RegId Address of the CPLD register to be read.
 * \param[out] pRegValue pointer to value read from the CPLD register
 *
 * \return 0 on success. A non zero value means there has been an error reading the CPLD register.
 *
 */
extern LONG	Vhsadac16_GetCpldReg(HANDLE h, ULONG RegId, PUCHAR pRegValue);

/**
 * Writes to a CPLD register.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] RegId Address of the CPLD register to be written to.
 * \param[in,out] pRegValue pointer to value to be written to the CPLD register, with read-back
 *
 * \return 0 on success. A non zero value means there has been an error writing to the CPLD register.
 *
 */
extern LONG	Vhsadac16_SetCpldReg(HANDLE h, ULONG RegId, PUCHAR pRegValue);

/**
 * Reads an FPGA register.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] reg_id Address of the FPGA register to be read.
 *
 * \return Signed 32-bit dwords.
 *
 * \todo ADD AN EXTRA ARGUMENT[out] FOR RETURN VALUE OF THE REG AND RETURN 0 ON SUCCESS!!!
 */
extern LONG	Vhsadac16_ReadFpgaReg(HANDLE h, ULONG reg_id);

/**
 * Writes to an FPGA register.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] reg_id Address of the FPGA register to be read.
 * \param[in] reg_value Unsigned 32-bit dwords to be written in the FPGA register.
 *
 * \return Signed written register value on success, -1 otherwise.
 *
 * \todo RETURN 0 ON SUCCESS, DO NOT RETURN THE WRITTEN REG VALUE!!!
 */
extern LONG	Vhsadac16_WriteFpgaReg(HANDLE h, ULONG reg_id, ULONG reg_value);

/**
 * Reads data from the FPGA memory.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] offset Offset in the FPGA memory.
 * \param[in] size Number of bytes to be written.
 * \param[out] pBuffer Pointer to the input buffer.
 *
 * \return Number of bytes read from the FPGA memory.
 */
extern LONG	Vhsadac16_ReadFpgaMem(HANDLE h, ULONG offset, ULONG size, ULONG* pBuffer);

/**
 * Writes data into the FPGA memory.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] offset Offset in the FPGA memory.
 * \param[in] size Number of bytes to be written.
 * \param[in] pBuffer Pointer to the output buffer.
 *
 * \return Number of bytes written into the FPGA memory.
 */
extern LONG	Vhsadac16_WriteFpgaMem(HANDLE h, ULONG offset, ULONG size, ULONG* pBuffer);

/**
 * Closes a VHS-ADC/DAC handle.
 *
 * \param[in] h VHS-ADC/DAC handle.
 */
extern void	Vhsadac16_Close(HANDLE h);

/**
 * Closes all VHS-ADC/DAC handles.
 */
extern void Vhsadac16_CloseAllBoards();

/**
 * Waits n milliseconds.
 *
 * This function imposes a delay, in milliseconds, to the host computer.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] delay_msec Delay in milliseconds.
 */
extern void	Vhsadac16_Wait(HANDLE h, ULONG delay_msec);


/**
 * Detects whether the VHS-ADC/DAC is a Virtex-4 model.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 1 if the VHS-ADC/DAC is a Virtex-4 model, 0 otherwise.
 */
extern LONG  Vhsadac16_isVhsBoardV4(HANDLE h);


/**
 * Detects whether the VHS-ADC/DAC is a ADC model.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 1 if the VHS-ADC/DAC is a ADC model, 0 otherwise.
 */
extern BOOLEAN Vhsadac16_isVhsADC(HANDLE h);

////////////////////////////////////// RTDEx API /////////////////////////////////////////////

/**
 * Detects the presence of RTDEx in the VHS-ADC/DAC FPGA specified by the handle.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 1 if RTDEx is present, 0 otherwise.
 */
extern LONG  Vhsadac16_isRTDExDetected(HANDLE h);

/**
 * Resets RTDEx FIFO, counters, and error counters.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] rxEnable Enables RTDEx reception.
 * \param[in] txEnable Enables RTDEx transmission.
 *
 * \return 0 on success.
 */
extern LONG  Vhsadac16_RTDExReset(HANDLE h, BOOLEAN rxEnable, BOOLEAN txEnable);

/**
 * Reads data from a VHS-ADC/DAC using RTDEx.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] buffer Pointer to the input buffer.
 * \param[in] length Length of the input buffer.
 *
 * \return Number of bytes read from RTDEx.
 */
extern ULONG Vhsadac16_RTDExReadBuffer (HANDLE h, PUCHAR buffer, ULONG length);

/**
 * Writes data to a VHS-ADC/DAC using RTDEx.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] buffer Pointer to the output buffer.
 * \param[in] length Length of the output buffer.
 *
 * \return Number of bytes written to RTDEx.
 */
extern ULONG Vhsadac16_RTDExWriteBuffer(HANDLE h, PUCHAR buffer, ULONG length);

/**
 * Reads the RTDEx error counters for overrun and underrun errors.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[out] errorInfo Enables RTDEx reception.
 *
 * \return 0 on success.
 */
extern LONG  Vhsadac16_GetRTDExError(HANDLE h, PVHSADAC_RTDEX_ERROR_INFO errorInfo);

/**
 * Initializes a VHS-ADC/DAC SDRAM at the default frequency.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 1 on success, 0 otherwise.
 *
 * \todo FIX THIS RETURN VALUE!!!
 */
extern ULONG	Vhsadac16_Sdram_AutoInit( HANDLE h );

/**
 * Returns the SDRAM bus frequency in Hz.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return The SDRAM bus frequency in Hz.
 */
extern ULONG	Vhsadac16_Sdram_Frequency( HANDLE h );

/**
 * Returns the SDRAM size in dwords (32 bits).
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return The SDRAM size in 32-bit dwords.
 */
extern ULONG	Vhsadac16_Sdram_Size( HANDLE h );

/**
 * Sets the SDRAM usage in single-page mode.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 0 on success.
 */
extern ULONG	Vhsadac16_Sdram_SinglePageMode( HANDLE h );

/**
 * Sets the SDRAM usage in dual-page mode.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 0 on success.
 */
extern ULONG	Vhsadac16_Sdram_DualPageMode( HANDLE h );

/**
 * Sets the SDRAM read page register to a specific value.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] page SDRAM page number.
 *
 * \return 0 on success.
 */
extern ULONG	Vhsadac16_Sdram_ReadPage( HANDLE h, USHORT page );

/**
 * Sets the SDRAM write page register to a specific value.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] page SDRAM page number.
 *
 * \return 0 on success.
 */
extern ULONG	Vhsadac16_Sdram_WritePage( HANDLE h, USHORT page );

/**
 * Sets both SDRAM read/write page registers to a specific value.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] page SDRAM Page number.
 *
 * \return 0 on success.
 */
extern ULONG	Vhsadac16_Sdram_Page( HANDLE h, USHORT page );

/**
 * Writes data to the SDRAM at a specific address. This function supports automatic page swapping.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] offset Offset in the SDRAM in bytes.
 * \param[in] data Pointer to the output buffer to be written.
 * \param[in] len Transfer length in bytes.
 *
 * \return Number of bytes written.
 */
extern  LONG	Vhsadac16_Sdram_Write( HANDLE h, ULONG offset, UCHAR *data, ULONG len );

/**
 * Reads data from the SDRAM at a specific address. This function supports automatic page swapping.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] offset Offset in the SDRAM in bytes.
 * \param[in] data Pointer to the input buffer.
 * \param[in] len Transfer length in bytes.
 *
 * \return Number of bytes read.
 */
extern  LONG	Vhsadac16_Sdram_Read ( HANDLE h, ULONG offset, UCHAR *data, ULONG len );
//////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Programs a VHS-ADC/DAC Virtex-4 with the contents of its flash memory.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 0 on success.
 */
extern LONG     Vhsadac16_Flash_Boot( HANDLE h );

/**
 * Erases VHS-ADC/DAC Virtex-4 FPGA flash memory.
 *
 * \param[in] h VHS-ADC/DAC handle.
 *
 * \return 0 on success.
 */
extern LONG     Vhsadac16_Flash_Erase( HANDLE h );

/**
 * Reads VHS-ADC/DAC Virtex-4 FPGA flash memory.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] length Number of bytes to read.
 * \param[out] buff Pointer to the output buffer.
 *
 * \return 0 on success.
 */
extern LONG     Vhsadac16_Flash_Read( HANDLE h, ULONG length, PUCHAR buff );

/**
 * Programs VHS-ADC/DAC Virtex-4 FPGA flash memory.
 *
 * \param[in] h VHS-ADC/DAC handle.
 * \param[in] length Number of bytes to read.
 * \param[in] bitstream Pointer to the input buffer that contains data to be written.
 * \param[in] bootFpga If true, programs the FPGA from the flash when flash write is done.
 *
 * \return 0 on success.
 */
extern LONG     Vhsadac16_Flash_Program( HANDLE h, ULONG length, PUCHAR bitstream, BOOLEAN bootFpga );


//////////////////////////////////////////////////////////////////////////////////////////////
extern LONG Vhsadac16_GetTransferStatus(HANDLE h);

extern VHSADAC16_BOARD_ERROR Vhsadac16_getLastError(HANDLE h);
extern DWORD Vhsadac16_getLastErrorEx();

extern LONG  Vhsadac16_RTDExAbortRead(HANDLE h);
extern LONG  Vhsadac16_RTDExAbortWrite(HANDLE h);

extern ULONG Vhsadac16_RTDExPreloadWrite(HANDLE h, PULONG buffer, ULONG length);

extern ULONG Vhsadac16_getRTDExReadStatus(HANDLE h);
extern ULONG Vhsadac16_getRTDExWriteStatus(HANDLE h);

extern LONG  Vhsadac16_RTDExFlowControl(HANDLE h, BOOLEAN rxStart, BOOLEAN txStart);

extern LONG  Vhsadac16_GetProfiling(HANDLE h, PVHSADAC_PROFILING_INFO profilingInfo);
extern LONG  Vhsadac16_ResetProfiling(HANDLE h,BOOLEAN resetRx, BOOLEAN resetTx);

extern VHSADAC16_BOARD_STATUS Vhsadac16_getBoardStatus(HANDLE h);
extern VHSADAC_BOARD_NODE     Vhsadac16_getBoardNode(HANDLE h);

extern ULONG Vhsadac16_LockBoard(HANDLE h);
extern ULONG Vhsadac16_UnlockBoard(HANDLE h);

extern LONG  Vhsadac16_V360Debug(HANDLE h, PVHSADAC_V360_DEBUG_INFO info);
extern LONG  Vhsadac16_V360Reset(HANDLE h);
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* _VHSADC16_API_H_ */
