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
 *  Project     : Lyrtech
 *  File        : LyrRc.h
 *  Description : 
 *
 *  Author(s)   : Jean-François Robert
 *
 *						Copyright (c) Lyrtech RD inc. 2007
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 * 	$Name: 1.1 $													
 *	$Revision: 1.1 $
 *	$Date: 2007/10/15 19:07:15 $
 *	$Author: jf.robert $
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file LyrRc.h
    \brief Standard return code.

    This describe all the return code from the Lyrtech libraries.
*/

#ifndef __LYRRC_INC
#define __LYRRC_INC


#ifdef  __cplusplus
extern "C" {
#endif

/***************************************************************************
 *                           I N C L U D E S                               *
 ***************************************************************************/

#include <windows.h>

/***************************************************************************
 *                             M A C R OS                                  *
 ***************************************************************************/

/***************************************************************************
 *                          C O N S T A N T E S                            *
 ***************************************************************************/
/*! \enum  LYR_enumReturnCode
\brief Return code of the API.

All the API return one of these enumeration. The user should validate all
the return value for each API call to be sure that no error happen.
*/
typedef enum
{
    LYR_RC_NO_ERROR = 0,             /*!< The function return without any error. */ 
    LYR_RC_UNDEFINE_ERROR,           /*!< The function return with an undefined error. */
    LYR_RC_NOT_INIT,                 /*!< The library has not been initialize before calling the function. */
    LYR_RC_INVALID_PARAMETER,        /*!< One of the parameter passed to the function is invalid. */
    LYR_RC_CANCELED,                 /*!< The call have been canceled. */
    LYR_RC_UNSUPPORTED_FUNCTION,     /*!< The current driver doesn't support this function. */
    LYR_RC_CPLD_ERROR,               /*!< An error was detected while accessing the CPLD. */
    LYR_RC_UNKOWN_BOARD,             /*!< The board is undefined. */
    LYR_RC_DSP_HPI_ERROR,            /*!< An error was detected while accessing the HPI. */
    LYR_RC_DSP_EMIF_ERROR,           /*!< An error was detected while configuring the EMIF. */
    LYR_RC_DSP_PROGRAM_ERROR,        /*!< An error was detected while programming the DSP. */
    LYR_RC_FPGA_ERROR,               /*!< An error was detected while accessing the FPGA. */
    LYR_RC_FPGA_PROGRAM_ERROR,       /*!< An error was detected while programming the FPGA. */
    LYR_RC_FPGA_PROGRAM_IN_PROGRESS, /*!< The current FPGA is in programming mode. */
    LYR_RC_I2C_ERROR,                /*!< An error was detected while accessing the I2C. */
    LYR_RC_RTDEX_NOT_DETECTED,       /*!< The RTDEx was not detected on the FPGA. */
    LYR_RC_RTDEX_INIT_ERROR,         /*!< An error was detected while initialize RTDEx. */
    LYR_RC_RTDEX_TRANSFER_ERROR,     /*!< An error was detected while transferring with RTDEx. */
    LYR_RC_RTDEX_IN_PROGRESS,        /*!< RTDEx is currently not available. A transfer is currently pending. */
    LYR_RC_RTDEX_TIMEOUT,            /*!< A timeout occur while transferring with RTDEx. */
    LYR_RC_HM_NOT_DETECTED,          /*!< Hardware module is not detected. */
    LYR_RC_HOST_IRQ_THREAD_ERROR     /*!< An error occur while configuring the Host IRQ Thread. */
} LYR_RETURN_CODE;

/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

typedef struct 
{
    ULONG   ulErrCode;
    char    *szRc;
} LYR_TYPE_RC_STRING;

/***************************************************************************
 *                           V A R I A B L E S                             *
 ***************************************************************************/

static const   LYR_TYPE_RC_STRING    g_lyrError[] = {
    {LYR_RC_NO_ERROR,                "LYR_RC_NO_ERROR"},
    {LYR_RC_UNDEFINE_ERROR,          "LYR_RC_UNDEFINE_ERROR"},
    {LYR_RC_NOT_INIT,                "LYR_RC_NOT_INIT"},
    {LYR_RC_INVALID_PARAMETER,       "LYR_RC_INVALID_PARAMETER"},
    {LYR_RC_CANCELED,                "LYR_RC_CANCELED"},
    {LYR_RC_UNSUPPORTED_FUNCTION,    "LYR_RC_UNSUPPORTED_FUNCTION"},
    {LYR_RC_CPLD_ERROR,              "LYR_RC_CPLD_ERROR"},
    {LYR_RC_UNKOWN_BOARD,            "LYR_RC_UNKOWN_BOARD"},
    {LYR_RC_DSP_HPI_ERROR,           "LYR_RC_DSP_HPI_ERROR"},
    {LYR_RC_DSP_EMIF_ERROR,          "LYR_RC_DSP_EMIF_ERROR"},
    {LYR_RC_DSP_PROGRAM_ERROR,       "LYR_RC_DSP_PROGRAM_ERROR"},
    {LYR_RC_FPGA_ERROR,              "LYR_RC_FPGA_ERROR"},
    {LYR_RC_FPGA_PROGRAM_ERROR,      "LYR_RC_FPGA_PROGRAM_ERROR"},
    {LYR_RC_FPGA_PROGRAM_IN_PROGRESS,"LYR_RC_FPGA_PROGRAM_IN_PROGRESS"},
    {LYR_RC_I2C_ERROR,               "LYR_RC_I2C_ERROR"},
    {LYR_RC_RTDEX_NOT_DETECTED,      "LYR_RC_RTDEX_NOT_DETECTED"},
    {LYR_RC_RTDEX_INIT_ERROR,        "LYR_RC_RTDEX_INIT_ERROR"},
    {LYR_RC_RTDEX_TRANSFER_ERROR,    "LYR_RC_RTDEX_TRANSFER_ERROR"},
    {LYR_RC_RTDEX_IN_PROGRESS,       "LYR_RC_RTDEX_IN_PROGRESS"},
    {LYR_RC_RTDEX_TIMEOUT,           "LYR_RC_RTDEX_TIMEOUT"},
    {LYR_RC_HM_NOT_DETECTED,         "LYR_RC_HM_NOT_DETECTED"},
    {LYR_RC_HOST_IRQ_THREAD_ERROR,   "LYR_RC_HOST_IRQ_THREAD_ERROR"}
};

/***************************************************************************
 *                       P U B L I C  F U N C T I O N s                    *
 ***************************************************************************/
__inline
const char *
LyrRc_GetRcString (
    LYR_RETURN_CODE  smQuadRc
)
{
    ULONG     ulLength;

    ulLength = (sizeof(g_lyrError) / sizeof(LYR_TYPE_RC_STRING));

    if ((ULONG)smQuadRc < ulLength)
    {
        return g_lyrError[smQuadRc].szRc;
    }
    else
    {
        return g_lyrError[LYR_RC_UNDEFINE_ERROR].szRc;
    }
}


#ifdef  __cplusplus
}
#endif

#endif /* End of LYRRC.h */

