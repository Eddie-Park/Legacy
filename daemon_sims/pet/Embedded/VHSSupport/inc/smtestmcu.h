/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SignalMaster                                              *
 *  Processor   : SHARC & TMSC32 & C6XX	                                    *
 *  File        : smtestmcu.h                                               *
 *  Description : "SignalMaster Diagnose Program (MCU)"                     *
 *                                                                          *
 ****************************************************************************
 *
 * "$Name: 1.1 $"
 * "$Date: 2007/03/08 21:31:26 $"
 * "$Author: jf.robert $"
 *
 ****************************************************************************/

#ifndef __SMTESTMCU_H__
#define __SMTESTMCU_H__

#define SMTESTMCU_VERSION_MAJOR		1
#define SMTESTMCU_VERSION_MINOR		4

typedef union
{

	struct
	{
		unsigned int smTestRevMajor;/* SignalMaster Diagnostic program revision	*/
		unsigned int smTestRevMinor;/* SignalMaster Diagnostic program revision	*/
		unsigned int dspBoardRev;	/* Board revision (DSP board)				*/
		unsigned int dspBoardID;	/* Board ID (DSP board)						*/
		unsigned int dspCpldRev;	/* CPLD	revision (DSP board)				*/
		unsigned int DspType;		/* DSP Type									*/
		float DspClock;				/* DSP Clock Frequency						*/
		unsigned int DspClockMode;	/* DSP Clock Mode							*/
		unsigned int FpgaModel;		/* FPGA Type								*/
	} smc6701;

	struct
	{
		unsigned int smTestRevMajor;/* SignalMaster Diagnostic program revision	*/
		unsigned int smTestRevMinor;/* SignalMaster Diagnostic program revision	*/
		unsigned int dspBoardRev;	/* Board revision (DSP board)				*/
		unsigned int dspBoardID;	/* Board ID (DSP board)						*/
		unsigned int dspXCV50Rev;	/* XCV50 FPGA revision (DSP board)			*/
		unsigned int DspType;		/* DSP Type									*/
		float DspClock;				/* DSP Clock Frequency						*/
		unsigned int DspClockMode;	/* DSP Clock Mode							*/
		unsigned int FpgaModel;		/* FPGA Type								*/
	} sm21160;

	struct
	{
		unsigned int smTestRevMajor;/* SignalMaster Diagnostic program revision	*/
		unsigned int smTestRevMinor;/* SignalMaster Diagnostic program revision	*/
		unsigned int flashSize;		/* FLASH Size (ELAN board)					*/
		unsigned int mcusdramSize;	/* SDRAM Size (ELAN board)					*/
		unsigned int mcuBoardRev;	/* Board revision (ELAN board)				*/
		unsigned int mcuCpldRev;	/* CPLD	revision (ELAN board)				*/
	} smelan;

	struct
	{
		unsigned int smTestRevMajor;/* SignalMaster Diagnostic program revision	*/
		unsigned int smTestRevMinor;/* SignalMaster Diagnostic program revision	*/
		unsigned int dspBoardRev;	/* Board revision (DSP board)				*/
		unsigned int dspBoardID;	/* Board ID (DSP board)						*/
		unsigned int dspCpldRev;	/* CPLD	revision (DSP board)				*/
		unsigned int DspType;		/* DSP Type									*/
		float DspClock;				/* DSP Clock Frequency						*/
		unsigned int DspClockMode;	/* DSP Clock Mode							*/
		unsigned int FpgaModel;		/* FPGA Type								*/
	} smquadc6713;


	struct
	{
		unsigned int smTestRevMajor;/* SignalMaster Diagnostic program revision	*/
		unsigned int smTestRevMinor;/* SignalMaster Diagnostic program revision	*/
		unsigned int dspBoardRev;	/* Board revision (DSP board)				*/
		unsigned int dspBoardID;	/* Board ID (DSP board)						*/
		unsigned int dspCpldRev;	/* CPLD	revision (DSP board)				*/
		unsigned int DspType;		/* DSP Type									*/
		float DspClock;				/* DSP Clock Frequency						*/
		unsigned int DspClockMode;	/* DSP Clock Mode							*/
		unsigned int FpgaModel;		/* FPGA Type								*/
	} Swavec6713;

	
} smTestMcu;

#endif