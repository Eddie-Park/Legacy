//*******************************************************************************
//*                                                                             *
//*  PETBox Server Software                                                     *
//*                                                                             *
//*  Copyright - 2009                                                           *
//*                                                                             *
//*  This code is the property of the Regents of UCLA.                          *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Richard Taschereau and Hui Zhang                                *
//*                                                                             *
//*  Created: September 2009                                                    *
//*                                                                             *
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************

#ifndef server_H
#define server_H

int  serverInitialization(void);
void serverTermination(void);

int  acqHardwareInitialization(void);
void acqHardwareTermination(void);


#ifndef _VHS_ADC_V4
#define _VHS_ADC_V4

#define numAcqBuffer 64			//64*128KB=8MB
#define acqBufferSize 131072   //32*1024*4Byte

#define MHZ                      1000000
#define VHS_CLOCK_FREQUENCY      (104*MHZ)


#define TARGET_RESOLUTION 1         // 1-millisecond timer resolution


/*
	Reg4: changed from Channel34 Offset(old) to status Test for RTDex(new)
	Reg5: changed from Channel12 Offset(old) to Input energy window(new)
	Reg6: changed from Channel78 Offset(old) to head2 singles show(new)
	Reg7: changed from Channel56 Offset(old) to head3 singles show(new)
*/
#define VHS_BT								BT_VHSADC16
#define VHS_ID								0
#define VHS_FPGA_Run_Reg 					VHSVIRTEX_USER_CTRL0
#define VHS_FPGA_Acq_Status_Reg 			VHSVIRTEX_USER_CTRL1
#define VHS_FPGA_Threshold_Reg 				VHSVIRTEX_USER_CTRL2
#define VHS_FPGA_Timing_Reg 				VHSVIRTEX_USER_CTRL3
#define VHS_FPGA_Acq_Status_Reg2            VHSVIRTEX_USER_CTRL4   
#define VHS_FPGA_ENERGY_WINDOW_Reg 			VHSVIRTEX_USER_CTRL5    
#define VHS_FPGA_DET2_SINGLES_RATE_Reg 		VHSVIRTEX_USER_CTRL6
#define VHS_FPGA_DET3_SINGLES_RATE_Reg 	    VHSVIRTEX_USER_CTRL7
#define VHS_FPGA_DET0_SINGLES_RATE_Reg 	    VHSVIRTEX_USER_CTRL8
#define VHS_FPGA_DET1_SINGLES_RATE_Reg 	    VHSVIRTEX_USER_CTRL9
#define VHS_FPGA_Coinc_CntRateReg 			VHSVIRTEX_USER_CTRL10
#define VHS_FPGA_Random_CntRateReg 			VHSVIRTEX_USER_CTRL11


#endif

#endif
