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
 *  Project     : VHS ADC/DAC REGISTERS DEFINITION     (HEADER)
 *  File        : vhsadc16_regs.h
 *  Description : VHS ADC/DAC REGISTERS DEFINITION
 *
 *  Author(s)   : Jérémie Plante
 *
 *                        Copyright (c) Lyrtech inc. 2005
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s                           
 *
 *--------------------------------------------------------------------------
 *  Date       Author    Version    Description                    
 *--------------------------------------------------------------------------
 *  13/12/05   JPLA      2.41       V4 RAPID CHANNEL REGISTER ADDR Added.
 ***************************************************************************
 *  "$RCSfile: vhsadac16_regs.h,v $"
 *  "$Author: david.pelletier $"
 *  "$Date: 2008/08/15 17:13:18 $"
 ***************************************************************************/
/*! \file vhsadc16_regs.h
  \brief VHS ADC/DAC REGISTERS DEFINITION
*/

/* CPCI Board mapping */

#ifndef _VHSADAC16_REGS_H_
#define _VHSADAC16_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

// PCI LOCAL BUS OFFSETs
#define CPLD_BASE            0x00000000 // IO SPACE! (use PciBase0 to access)
#define FPGA_BASE            0x00000000 // MEMORY MAPPED! (use PciIoBase to access)
        
#define VIRTEX_CFG_BASE        (1<<3)<<2        /* A3 (in bytes) */

#define VHSADC16_XC2V3000_DEVICE_ID            0x82 
#define VHSADC16_XC2V4000_DEVICE_ID            0x83 
#define VHSADC16_XC2V6000_DEVICE_ID            0x84 
#define VHSADC16_XC2V8000_DEVICE_ID            0x85 
#define VHSDAC16_XC2V3000_DEVICE_ID            0x86
#define VHSDAC16_XC2V4000_DEVICE_ID            0x87
#define VHSDAC16_XC2V6000_DEVICE_ID            0x88
#define VHSDAC16_XC2V8000_DEVICE_ID            0x89
#define VHSDAC16_XC4LX100_DEVICE_ID            0x8A
#define VHSDAC16_XC4LX160_DEVICE_ID            0x8B
#define VHSDAC16_XC4SX55_DEVICE_ID             0x8C
#define VHSADC16_XC4LX100_DEVICE_ID            0x8D
#define VHSADC16_XC4LX160_DEVICE_ID            0x8E
#define VHSADC16_XC4SX55_DEVICE_ID             0x8F


/* CPLD CONFIGURATION REGISTERS */

#define VIRTEX_SEL            ((0x00 << 2))
#define VIRTEX_PGM            ((0x01 << 2))
#define VIRTEX_DAUGHTER_PGM   ((0x02 << 2))
#define ADC_SPEED_STATUS      ((0x03 << 2))
#define CPLD_STATUS           ((0x04 << 2))
#define CPLD_REVISION         ((0x05 << 2))
#define PCB_REVISION          ((0x06 << 2))
#define SM_BOARD_ID           ((0x07 << 2))

/* FPGA SELECTION BIT (VIRTEX_SEL Register) */

#define ONBOARD_FPGA        0x00
#define DAUGHTER_FPGA_1     0x01
#define DAUGHTER_FPGA_2     0x02
#define DAUGHTER_FPGA_3     0x04
#define DAUGHTER_FPGA_4     0x08

/* FPGA CONTROL REG BIT (VIRTEX_PGM Register) */

#define FPGA_PROGRAM        0x01
#define FPGA_BUSY           0x02
#define FPGA_DONE           0x04
#define FPGA_INIT           0x08
#define FPGA_POWER_DOWN     0x40

#ifdef  SMC6XX_TEST
#define FPGA_FLASH_ENABLE   0x80
#endif

/* Misc. */

// Supported FPGA types
#define XC4000    1
#define VIRTEX    2

// Supported FPGA Configuration Methods
#define SLAVE_SERIAL_MODE   1
#define SELECT_MAP_MODE     2

/* PCI MAP 1 - PCI Memory Mapped Virtex registers */
                                            
#define VHSVIRTEX_STATUS_CTRL               (0x00 << 2)
#define VHSVIRTEX_FPDP_PI012_CTRL           (0x01 << 2)
#define VHSVIRTEX_CLKS_CTRL                 (0x02 << 2)
#define VHSVIRTEX_PROG_CLK_1_4              (0x03 << 2)
#define VHSVIRTEX_PROG_CLK_5_8              (0x04 << 2)
#define VHSVIRTEX_PROG_CLK_9_12             (0x05 << 2)
#define VHSVIRTEX_PROG_CLK_13_16            (0x06 << 2)
#define VHSVIRTEX_ACQ_TRIG_CTRL             (0x07 << 2)
#define VHSVIRTEX_ADCS_OVR                  (0x08 << 2)
#define VHSVIRTEX_PROG_GAIN_CTRL            (0x09 << 2)
#define VHSVIRTEX_PROG_GAIN_1_2             (0x0A << 2)
#define VHSVIRTEX_PROG_GAIN_3_4             (0x0B << 2)
#define VHSVIRTEX_PROG_GAIN_5_6             (0x0C << 2)
#define VHSVIRTEX_PROG_GAIN_7_8             (0x0D << 2)
#define VHSVIRTEX_PROG_GAIN_9_10            (0x0E << 2)
#define VHSVIRTEX_PROG_GAIN_11_12           (0x0F << 2)
#define VHSVIRTEX_PROG_GAIN_13_14           (0x10 << 2)
#define VHSVIRTEX_PROG_GAIN_15_16           (0x11 << 2)
#define VHSVIRTEX_GPIO_CTRL                 (0x12 << 2)
#define VHSVIRTEX_GPIO_DIR                  (0x13 << 2)
#define VHSVIRTEX_GPIO_STATE                (0x14 << 2)
//#define VHSVIRTEX_FIFO_SIZE                 (0x15 << 2)
#define VHSVIRTEX_FPDP_DCM                 (0x15 << 2)
                                            
#define VHSVIRTEX_DIF_SERDES                (0x16 << 2)    /* (V4) DIF SERDES Status & Control */
                                            
#define VHSVIRTEX_RAPIDCHANNEL              (0x1080) /* (V4) RAPID CHANNEL Status & Control */
                                            
#define VHSVIRTEX_SDRAM_CONFIG              (0x20 << 2)
#define VHSVIRTEX_SDRAM_PAGE                (0x21 << 2)
#define VHSVIRTEX_SDRAM_FREQ                (0x22 << 2)
#define VHSVIRTEX_SDRAM_DCM                 (0x23 << 2)
                                            
                                            
#define VHSVIRTEX_USER_CTRL0                (0x1C << 2)
#define VHSVIRTEX_USER_CTRL1                (0x1D << 2)
#define VHSVIRTEX_USER_CTRL2                (0x1E << 2)
#define VHSVIRTEX_USER_CTRL3                (0x1F << 2)
#define VHSVIRTEX_USER_CTRL4                (0x100)
#define VHSVIRTEX_USER_CTRL5                (0x104)
#define VHSVIRTEX_USER_CTRL6                (0x108)
#define VHSVIRTEX_USER_CTRL7                (0x10C)
#define VHSVIRTEX_USER_CTRL8                (0x110)
#define VHSVIRTEX_USER_CTRL9                (0x114)
#define VHSVIRTEX_USER_CTRL10               (0x118)
#define VHSVIRTEX_USER_CTRL11               (0x11C)



#define VHSVIRTEX_FRAMES                    (0x20 << 2)
                                           
#define VHSVIRTEX_DAC5687_CTRL              (0x140)
#define VHSVIRTEX_DAC5687_INTADDR           (0x144)
#define VHSVIRTEX_DAC5687_INTVALUE          (0x148)
#define VHSVIRTEX_DAC5687_NCO_FREQ          (0x14C)
#define VHSVIRTEX_DAC5687_NCO_PHASE         (0x150)
#define VHSVIRTEX_DAC5687_DACA_OFFSET       (0x154)
#define VHSVIRTEX_DAC5687_DACB_OFFSET       (0x158)
#define VHSVIRTEX_DAC5687_QMC_GAINA         (0x15C)
#define VHSVIRTEX_DAC5687_QMC_GAINB         (0x160)
#define VHSVIRTEX_DAC5687_QMC_PHASE         (0x164)
#define VHSVIRTEX_DAC5687_DACA_GAIN         (0x168)
#define VHSVIRTEX_DAC5687_DACB_GAIN         (0x16C)
#define VHSVIRTEX_DAC5687_ERROR             (0x170)
#define VHSVIRTEX_DAC5687_INIT_DONE         (0x174)
                                           
/* Bits definition for registers */        
                                           
/* Register VHSVIRTEX_STATUS_CTRL */       
#define FPGA_REV(x)                         ( (0x00FF0000&(unsigned)x) >> 16)
#define DAUGTHER_GAIN_OPTION                0x4000  /*1: With programmable gain*/ 
#define DAUGTHER_GAIN_CONFIG                0x0800    /*1: Module DC Config*/
#define LOCK_BIT                            0x0400    /*1: Internal reset state*/
#define FPGA_DPRESENCE                      0x0200    /*1: FPGA Daugther card present*/
#define INT_RESET                           0x0100    /*1: Internal reset state*/
#define LED1_OUT                            0x0080    /*1: On*/
#define LED1_USER_CTRL                      0x0040    /*1: User CTRL*/
#define LED0_OUT                            0x0020    /*1: On*/
#define LED0_USER_CTRL                      0x0010    /*1: User CTRL*/
#define PDOWN_ANALOG                        0x0008    /*1: Power Down Analog Supply*/
#define DAUGHTER_ID                         0x0004    /*1: ADC Board*/
#define DAUGHTER_PRESENT                    0x0002    /*1: Daughter Board Present*/
#define MAIN_BOARD_ID                       0x0001    /*1: ADC Board*/
                                           
                                           
/* Register VHSVIRTEX_FPDP_PI012_CTRL */
#define FPDP_TRIGGER_CTRL                   0x40    /*1: TRIGGER MODE ON */
#define FPDP_SYNC_CTRL                      0x20    /*1: SYNC IS ALWAYS LOW (Information of frame in the bits 15:16)*/
#define PIO_INT_CTRL                        0x10    /*1: Internal Design State (SysGen)*/
#define PIO2_STATE                          0x08    /*1: Output High*/
#define PIO2_DIR                            0x04    /*1: Output*/
#define PIO1_STATE                          0x02    /*1: Output High*/
#define PIO1_DIR                            0x01    /*1: Output*/
                                           
/* Register VHSVIRTEX_CLKS_CTRL */         
                                           
#define MUX_DIF_CLK_DIV2                    0x8000    /* (V4) 1: SERDES Clock is divided by two. */
#define SYSGEN_DIF_CLK_CONTROL              0x4000    /* (V4) 1: SERDES Clock is controlled by Systen. */
#define HST_CLK_CTRL                        0x2000    /* (V4) 1: CLKs Are Controlled by THIS Register. */
#define DIF_CLK_DIVIDE                      0x1000    /* (V4) 0: Clock is not divided 1: Clock divided by 2 */
#define RAPIDCHANNEL_DCM_LOCKED             0x0800    /* (V4) 1: RapidCHANNEL Reference clock DCM is locked */
                                            
#define DCM_LOCKED                          0x100
#define RESET_DCM                           0x80    /*1: DCMs Are In Reset*/
#define PCLK_SET                            0x40    /*0->1: Set Programmable Clock*/
#define PCLK_SEL_CD                         0x20    /*1: Sel CD PLL*/    
#define PCLK_OE                             0x10    /*1: OE Programmable Clock*/
#define PCLK_PDOWN                          0x08    /*1: PDown Programmable Clock*/
//#define QUADRATURE                          0x04    /*1: Quadrature On*/
#define FPDP_SEL                            0x02    /*1: FPDP SELECTION CLOCK SOURCE FOR VHS-DAC ONLY*/
#define CLK_SOURCE                          0x01    /*1: Acquisition CLK Is External*/

#define FPDP_DCM_LOCKED                     0x1
                                           
#define DCMS_LOCKED_R(x)                    ( (DCM_LOCKED&(unsigned)x) >> 8)
#define FPDP_DCMS_LOCKED_R(x)               (FPDP_DCM_LOCKED&(unsigned)x)
                                           
/* Register VHSVIRTEX_ACQ_TRIG_CTRL */     
#define TRIGGER_ENABLE                      0x08    /*1: Enable trigger mechanism */
#define ACQ_ON                              0x01    /*1: Acquisition Machine Is On*/
                                            
                                           
/* Register VHSVIRTEX_PROG_GAIN_CTRL */    
#define GAIN_SET_ALL_CH1                    0x10000    /*1: Set Gains All Like CH1 Level*/
#define GAIN_SET                            0x20000    /*0->1: Set Gains Levels*/
#define GAIN_CTRL_MODE                      0x40000    /*1: Internal CTRL : SysGenerator*/
                                           
/* Register VHSVIRTEX_GPIO_CTRL */         
#define    GPIO_CTRL_MODE                   0x0001    /*1: Internal CTRL : SysGenerator*/



/* (V4) Register VHSVIRTEX_DIF_SERDES */
#define PLL_UNLOCK                          0x01
#define RCV_PDOWN                           0x02
#define LINE_LPBK                           0x04
#define LOCAL_LPBK                          0x08
#define SERDES_REN                          0x10
#define SERDES_DEN                          0x20
#define DIF_HOST_CTRL                       0x80

/* DAC V4 Function Control */
#define VHSVIRTEX_DAC5687_CTRL_ISR          0x0001
#define VHSVIRTEX_DAC5687_CTRL_WRITE        0x0002
#define VHSVIRTEX_DAC5687_CTRL_RESET        0x0020
#define VHSVIRTEX_DAC5687_CTRL_INTREG       0x0040
#define VHSVIRTEX_DAC5687_CTRL_NCO_FREQ     0x0080
#define VHSVIRTEX_DAC5687_CTRL_NCO_PHASE    0x0100
#define VHSVIRTEX_DAC5687_CTRL_DACA_OFFSET  0x0200
#define VHSVIRTEX_DAC5687_CTRL_DACB_OFFSET  0x0400
#define VHSVIRTEX_DAC5687_CTRL_QMC_GAINA    0x0800
#define VHSVIRTEX_DAC5687_CTRL_QMC_GAINB    0x1000
#define VHSVIRTEX_DAC5687_CTRL_QMC_PHASE    0x2000
#define VHSVIRTEX_DAC5687_CTRL_DACA_GAIN    0x4000
#define VHSVIRTEX_DAC5687_CTRL_DACB_GAIN    0x8000

#ifdef __cplusplus
}
#endif
#endif //_VHSADAC16_REGS_H_
