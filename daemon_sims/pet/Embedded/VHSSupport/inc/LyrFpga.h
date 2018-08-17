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
 *  Project     : FPGA
 *  File        : LyrFpga.h
 *  Description : 
 *
 *  Author(s)   : Jean-François Robert
 *
 *						Copyright (c) Lyrtech RD inc. 2007
 *
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file LyrFpga.h
    \brief General information for FPGA.

    This section describes all common define for FPGA.
*/

#ifndef __LYRFPGA_INC
#define __LYRFPGA_INC


#ifdef  __cplusplus
extern "C" {
#endif

/***************************************************************************
 *                           I N C L U D E S                               *
 ***************************************************************************/

/***************************************************************************
 *                             M A C R OS                                  *
 ***************************************************************************/

/***************************************************************************
 *                          C O N S T A N T E S                            *
 ***************************************************************************/


// Supported FPGA types
/*! \enum  LYR_FPGA_TYPES
\brief Supported FPGA types.

This enumeration list all the FPGA types.
*/
typedef enum
{
    LYR_FGPA_TYPES_UNKOWN	= 0,    /*!< FPGA type unknown. */
    LYR_FGPA_TYPES_XC4000,          /*!< FPGA XC4000. */
    LYR_FGPA_TYPES_VIRTEX,          /*!< FPGA VIRTEX. */
    LYR_FGPA_TYPES_SPARTAN          /*!< FPGA SPARTAN. */
} LYR_FPGA_TYPES;


/*! \enum  LYR_FPGA_PACKAGE
\brief Supported FPGA Package.

This enumeration list all the FPGA package.
*/
typedef enum
{
    LYR_FPGA_PACKAGE_UNKNOWN	= 0x0,         /*!< FPGA package unknown. */
    LYR_FPGA_PACKAGE_PQ208		= 0x208,       /*!< FPGA package PQ208. */
    LYR_FPGA_PACKAGE_PQ240		= 0x240,       /*!< FPGA package PQ240. */
    LYR_FPGA_PACKAGE_BG432		= 0x432,       /*!< FPGA package BG432. */
    LYR_FPGA_PACKAGE_FG456		= 0x456,       /*!< FPGA package FG456. */
    LYR_FPGA_PACKAGE_FG1156		= 0x1156,      /*!< FPGA package FG1156. */
    LYR_FPGA_PACKAGE_FF668		= 0xFF668,     /*!< FPGA package FF668. */
    LYR_FPGA_PACKAGE_FF896		= 0xFF896,     /*!< FPGA package FF896. */
    LYR_FPGA_PACKAGE_FF1152		= 0xFF1152,    /*!< FPGA package FF1152. */
    LYR_FPGA_PACKAGE_FF1148		= 0xFF1148,    /*!< FPGA package FF1148. */
} LYR_FPGA_PACKAGE;

/*! \enum  LYR_FPGA_MODEL
\brief Supported FPGA Models.

This enumeration list all the FPGA models.
*/
typedef enum
{
    LYR_FPGA_MODEL_UNKNOWN      = 0,            /*!< FPGA model unknown. */
    LYR_FPGA_MODEL_XCV50	    = 0x50,		    /*!< FPGA model XCV50 */
    LYR_FPGA_MODEL_XCV300	    = 0x300,	    /*!< FPGA model XCV300 */
    LYR_FPGA_MODEL_XCV800	    = 0x800,		/*!< FPGA model XCV800 */
    LYR_FPGA_MODEL_XCV1000E	    = 0x1000E,		/*!< FPGA model XCV1000E */
    LYR_FPGA_MODEL_XC2V1000	    = 0x2A1000,	    /*!< FPGA model XC2V1000 */
    LYR_FPGA_MODEL_XC2V3000	    = 0x2A3000,	    /*!< FPGA model XC2V3000 */
    LYR_FPGA_MODEL_XC2V6000	    = 0x2A6000,	    /*!< FPGA model XC2V6000 */
    LYR_FPGA_MODEL_XC2V8000	    = 0x2A8000,	    /*!< FPGA model XC2V8000 */
    LYR_FPGA_MODEL_XC4VLX80     = 0x016D8093,   /*!< FPGA model XC4VLX80  */
    LYR_FPGA_MODEL_XC4VLX100    = 0x01700093,   /*!< FPGA model XC4VLX100 */
    LYR_FPGA_MODEL_XC4VLX160    = 0x01718093,   /*!< FPGA model XC4VLX160 */
    LYR_FPGA_MODEL_XC4VSX35     = 0x00C40035,	/*!< FPGA model XC4VSX35 */
    LYR_FPGA_MODEL_XC4010E	    = 0x4010E,		/*!< FPGA model XC4010E */
    LYR_FPGA_MODEL_XC4013E	    = 0x4013E,		/*!< FPGA model XC4013E */
    LYR_FPGA_MODEL_XC4020E	    = 0x4020E,		/*!< FPGA model XC4020E */
    LYR_FPGA_MODEL_XC4028EX	    = 0x4028E,		/*!< FPGA model XC4028EX */
} LYR_FPGA_MODEL;

/*! \enum  LYR_FPGA_ID
\brief Identification of the FPGA on the board.

This enumeration list all the FPGA available on a card.
*/
typedef enum
{
    LYR_FPGA_ID_1 = 0,   /*!< FPGA id 1 */
    LYR_FPGA_ID_2        /*!< FPGA id 2 */
} LYR_FPGA_ID;

/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

/***************************************************************************
 *                           V A R I A B L E S                             *
 ***************************************************************************/

/***************************************************************************
 *                       P U B L I C  F U N C T I O N s                    *
 ***************************************************************************/



#ifdef  __cplusplus
}
#endif

#endif /* End of SMQUAD.h */

