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
 *  File        : fpgaMgr.h
 *  Description : SignalMaster Quad Project
 *
 *  Author(s)   : Jean-François Robert
 *
 *						Copyright (c) Lyrtech RD inc. 2007
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 * "$Name: 1.2 $"
 * "$Date: 2007/07/18 20:33:08 $"
 * "$Author: jf.robert $"
 ***************************************************************************
 *                          D e s c r i p t i o n
 *
 *
 *
 ***************************************************************************/
/** \file fpgaMgr.h
    \brief FPGA general information

    This section describes.
*/

#ifndef __FPGAMGR_H__
#define __FPGAMGR_H__

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


// Supported FPGA types.
#define XC4000	1
#define VIRTEX	2   /*!< FPGA VIRTEX type. */
#define SPARTAN	3

// Supported FPGA Models
#define FPGA_MODEL_UNKNOWN	0
#define FPGA_MODEL_XCV50	0x50		/* PQ240 */
#define FPGA_MODEL_XCV300	0x300		/* BG432 or PQ240 */
#define FPGA_MODEL_XCV800	0x800		/* BG432 or PQ240 */
#define FPGA_MODEL_XCV1000E	0x1000E		/* FF1156 */
#define FPGA_MODEL_XC2V1000	0x2A1000	/* XC2V1000 */
#define FPGA_MODEL_XC2V3000	0x2A3000	/* XC2V3000 */
#define FPGA_MODEL_XC2V6000	0x2A6000	/* XC2V6000 */
#define FPGA_MODEL_XC2V8000	0x2A8000	/* XC2V8000 */

#define FPGA_MODEL_XC4VLX80  0x016D8093  /* XC4LX80  (JP.5/04/06) */
#define FPGA_MODEL_XC4VLX100 0x01700093  /* XC4LX100 (JP.5/04/06) */
#define FPGA_MODEL_XC4VLX160 0x01718093  /* XC4LS160 (JP.5/04/06) */

#define FPGA_MODEL_XC4VSX35  0x00C40035	 /* XC4VSX35 (FL 20/09/06) */

#define FPGA_MODEL_XC4010E	0x4010E		/* PQ208 */
#define FPGA_MODEL_XC4013E	0x4013E		/* PG240 */
#define FPGA_MODEL_XC4020E	0x4020E		/* PQ240 */
#define FPGA_MODEL_XC4028EX	0x4028E		/* PQ208 or PQ240 */

// Use in GCS-II for LabVolt
#define FPGA_MODEL_XC2S300E	0x25300E	/* FG456 */
#define FPGA_MODEL_XC2S400E	0x25400E	/* FG456 */

#define FPGA_MODEL_XC3S500E		0x35500E	/* FG320 */
#define FPGA_MODEL_XC3S1200E	0x351200E	/* FG320 */
#define FPGA_MODEL_XC3S1600E	0x351600E	/* FG320 */

// Supported FPGA Package
#define FPGA_PACKAGE_UNKNOWN	0x0
#define FPGA_PACKAGE_PQ208		0x208
#define FPGA_PACKAGE_PQ240		0x240
#define FPGA_PACKAGE_BG432		0x432
#define FPGA_PACKAGE_FG456		0x456
#define FPGA_PACKAGE_FG1156		0x1156
#define FPGA_PACKAGE_FF668		0xFF668
#define FPGA_PACKAGE_FF896		0xFF896
#define FPGA_PACKAGE_FF1152		0xFF1152
#define FPGA_PACKAGE_FF1148		0xFF1148


// Supported Configuration Methods.
#define SLAVE_SERIAL_MODE   1

/*! Onboard FPGA supported configuration/programmation mode.
    The FPGA is programmed by his 8 bit parallele interface.  
    Only valid for onboard FPGA not FPGA module.
*/
#define SELECT_MAP_MODE		2
    
/***************************************************************************
 *                          S T R U C T U R E S                            *
 ***************************************************************************/

// SMFPGA Device Definition
typedef struct __SMFPGA_DEVICE
{
	// Magic Number (public)
#define SMFPGA_DEVICE_MAGIC_NUMBER	0x51433658	// ASCII => "FPGA"
	int MagicNumber;

	// Version Control (public)
	// (changing this version will programs to be recompiled and verified)
#define SMFPGA_DEVICE_VERSION_MAJOR	0
#define SMFPGA_DEVICE_VERSION_MINOR	0
	int VersionMajor;
	int VersionMinor;

	// FPGA Specifications (public)
	int	FpgaBoot;		/* unused, for now */
	int	FpgaModel;
	int	FpgaPackage;
	int FpgaConfigMethod;

	// FPGA Programming routine
	int (*FpgaReset)( void *p, int fpga_id, int fpgaType, int configMethod, unsigned char bitstream[], int length );
	void *p;			// Pointer to void *p above
	int FpgaId;
}
SMFPGA_DEVICE, *PSMFPGA_DEVICE;

/***************************************************************************
 *                           V A R I A B L E S                             *
 ***************************************************************************/

/***************************************************************************
 *                       P U B L I C  F U N C T I O N S                    *
 ***************************************************************************/


int fpga_getDescriptor( int fpgaType, unsigned char bitstream[], char *szDescriptor );
int fpga_reset(int fpgaType, int configMethod, unsigned char bitstream[], int length);
int fpga_getBitstreamSize(int fpgaType, int fpgaModel);

void fpga_extractModelfromDescriptor(char *szDescriptor, int *fpgaModel, int *fpgaPackage);
char* fpga_getModelString( int fpgaModel );

#ifdef  __cplusplus
}
#endif

#endif /* __FPGAMGR_H__ */


