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
 *  Project     : DSP Type
 *  File        : LyrDsp.h
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
/** \file LyrDsp.h
    \brief General information for DSP.

    This section describes all common defines for DSP.
*/

#ifndef __LYRDSP_INC
#define __LYRDSP_INC


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

/*! \enum  LYR_DSP_TYPES
\brief Supported DSP types.

This enumeration list all the DSP types.
*/
typedef enum
{
    LYR_DSP_TYPE_UNKNOWN		= 0,       /*!< DSP type unknown. */
    LYR_DSP_TYPE_TMS320C6713	= 6713,    /*!< DSP type TMS320C6713. */
    LYR_DSP_TYPE_TMS320C6416T	= 6416     /*!< DSP type TMS320C6416T. */
} LYR_DSP_TYPES;

/*! \enum  LYR_DSP_CLOCK_MODE
\brief Supported DSP clock mode.

This enumeration list all the DSP clock mode.
*/
typedef enum
{
    LYR_DSP_CLOCK_MODE_UNKOWN   = 0,    /*!< DSP clock mode unknown. */
    LYR_DSP_CLOCK_MODE_X1	    = 1,    /*!< DSP clock mode X1. */
    LYR_DSP_CLOCK_MODE_X4	    = 4     /*!< DSP clock mode X4. */
} LYR_DSP_CLOCK_MODE;

/*! \enum  LYR_DSP_ID
\brief Identification of the DSP on the board.

This enumeration list all the DSP available on a board.
*/
typedef enum
{
    LYR_DSP_ID_1 = 0,    /*!< DSP id 1 */
    LYR_DSP_ID_2,        /*!< DSP id 2 */
    LYR_DSP_ID_3,        /*!< DSP id 3 */
    LYR_DSP_ID_4         /*!< DSP id 4 */
} LYR_DSP_ID;

#define LYR_DSP_CLOCK_0MHZ_VALUE	((float)(0.0)       /*!< DSP clock speed at 0MHz. */
#define LYR_DSP_CLOCK_150MHZ_VALUE	((float)(150.0/4))  /*!< DSP clock speed at 150MHz. */
#define LYR_DSP_CLOCK_167MHZ_VALUE	((float)(167.0/4))  /*!< DSP clock speed at 167MHz. */
#define LYR_DSP_CLOCK_200MHZ_VALUE	((float)(200.0/4))  /*!< DSP clock speed at 200MHz. */
#define LYR_DSP_CLOCK_225MHZ_VALUE	((float)(225.0/4))  /*!< DSP clock speed at 225MHz. */
#define LYR_DSP_CLOCK_720MHZ_VALUE	((float)(720))      /*!< DSP clock speed at 720MHz. */
#define LYR_DSP_CLOCK_1000MHZ_VALUE	((float)(1000))     /*!< DSP clock speed at 1000MHz. */

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

