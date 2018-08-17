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
 *  Project     : VHS-DAC HIGH LEVEL CONTROL API    (HEADER)
 *  File        : rffectrl.h
 *  Description : WIN32 CONTROL API for VHS-DAC Boards.
 *
 *  Author(s)   : Louis-Philippe Lessard
 *
 *                      Copyright (c) Lyrtech inc. 2005
 *
 ***************************************************************************
 *
 *                             R e v i s i o n s
 *
 *--------------------------------------------------------------------------
 *  Date       Author    Version    Description
 *--------------------------------------------------------------------------
 *2006-06-26    LPL        1.1       bug fixes
 ***************************************************************************
 *  "$RCSfile: rffectrl.h,v $"
 *  "$Author: michel.mercier $"
 *  "$Date: 2008/01/31 16:21:10 $"
 ***************************************************************************/
/*! \file rffectrl.h
  \brief RFFE control API.

  This section describes all the functions of the RFFE control API.<BR>
  Refer to this API first when developing applications meant to communicate
  with the RFFE, as they contain application-specific routines.
*/
/* The host CPU and DSP applications share the same high-level API. */


#ifndef _RFFEADC16CTRL_H_
#define _RFFEADC16CTRL_H_

#ifdef SWIG //with this, Swig don't choke on far keyword
#define far
#endif

#ifdef __cplusplus
extern "C" {
#endif


#include "vhsadac16_regs.h"

#ifdef WIN32
#include "vhsadac16_host_api.h"
#else
#include "vhsadac16_dsp_api.h"
#endif

#include "vhsdac16ctrl.h"
#include "vhsadc16ctrl.h"


/**
*RFFE transceiver number.
*/
typedef enum{
TRANSCEIVER_1,          /*!< Transceiver 1 */
TRANSCEIVER_2,          /*!< Transceiver 2 */
TRANSCEIVER_3,          /*!< Transceiver 3 */
TRANSCEIVER_4           /*!< Transceiver 4 */
}TRANSCEIVER_NUMBER;

/**
*RFFE transceiver programming modes.
*/
typedef enum{
TRANSCEIVER_PROG_SINGLE=0,  /*!< Programs the transceiver specified by the transceiver argument*/
TRANSCEIVER_PROG_ALL        /*!< Programs all the transceivers */
}TRANSCEIVER_PROG_MODE;

/**
*RFFE transceiver modes.
* \note For details, refer to the Quad Dual Band RF Transceiver guide.
*/
typedef enum{
TRANSCEIVER_SPI_RESET=0x40,         /*!< Resets the SPI */
TRANSCEIVER_SHUTDOWN=0x20,          /*!< Shuts down all the circuit blocks except the SPI */
TRANSCEIVER_STANDBY=0x10,           /*!< Places the transceiver on standby, allowing various blocks of the system to be enabled or disabled,
                                         according to the standby register. */
TRANSCEIVER_RX=0x8,                 /*!< Transceiver reception mode */
TRANSCEIVER_TX=0x4,                 /*!< Transceiver transmission mode */
TRANSCEIVER_TX_CALIBRATION=0x2,     /*!< Transceiver reception mode calibration*/
TRANSCEIVER_RX_CALIBRATION=0x1      /*!< Transceiver transmission mode  calibration*/
}TRANSCEIVER_MODE;

/**
* Selects the power amplifiers enabled in the transmission path.
*/
typedef enum{
PABS_OFF=0x0,   /*!< 2.4-GHz band is used */
PABS_ON=0x4     /*!< 5-GHz band is used */
}PABS_STATE;

/**
* Diversity switch control signal.
*/
typedef enum{
DSW_OFF=0x0,    /*!< Antenna B */
DSW_ON=0x2      /*!< Antenna A */
}DSW_STATE;

/**
* Reference oscillator source selection.
*/
typedef enum{
OSCS_OFF=0x0,   /*!< External oscillator */
OSCS_ON=0x1     /*!< Internal oscillator */
}OSCS_STATE;

/**
* RX baseband AC-coupling high pass corner frequency control digital input selection bit.
*/
typedef enum{
RXHP_ON=0,         /*!< On*/
RXHP_OFF = 0x80      /*!< Off */
}RXHP_STATE;


/**
* Transmission power amplifiers enable/disable.
*/
typedef enum{
PAEN_ON = 0,       /*!< Power amplifiers on */
PAEN_OFF= 0x100    /*!< Power amplifiers off */
}PAEN_STATE;


/*! \def RFFE_BASE_ADDRESS
  RFFE base address. Used as the starting point to access the RFFE. Use the correct 'enum' or 'define' to access registers.
 */
#define RFFE_BASE_ADDRESS           0x1200

/**
* Register addresses in the FPGA
* \note For details, refer to the Quad Dual Band RF Transceiver guide
*/
typedef enum{
RFFE_REGISTER0=(RFFE_BASE_ADDRESS + 0x00),                  /*!< Unused */
RFFE_REGISTER1=(RFFE_BASE_ADDRESS + 0x04),                  /*!< Unused */
RFFE_STANDBY=(RFFE_BASE_ADDRESS + 0x08),                    /*!< Various internal blocks can be turned on or off using the standby register */
RFFE_INTEGER_DIVIDER_RATIO=(RFFE_BASE_ADDRESS + 0x0C),      /*!< Contains the integer portion of the divider ratio of the synthesizer. */
RFFE_FRACTIONAL_DIVIDER_RATIO=(RFFE_BASE_ADDRESS + 0x10),   /*!< Contains the fractionnal portion of the divider ratio of the synthesizer. */
RFFE_BAND_SELECT_PLL=(RFFE_BASE_ADDRESS + 0x14),            /*!< Configures the programmable-reference frequency dividers for the synthesizers, and
                                                                 sets the DC current for the charge pump.*/
RFFE_CALIBRATION=(RFFE_BASE_ADDRESS + 0x18),                /*!< Configures the RX/TX calibration modes */
RFFE_LOWPASS_FILTER=(RFFE_BASE_ADDRESS + 0x1C),             /*!< Allows the adjustment of the RX and TX low-pass filter corner frequencies */
RFFE_RX_CONTROL_RSSI=(RFFE_BASE_ADDRESS + 0x20),            /*!< Allows the adjustment of the RX section and the RSSI output */
RFFE_TX_LINEARITY_BB_GAIN=(RFFE_BASE_ADDRESS + 0x24),       /*!< Allows the adjustment of the TX gain and linearity */
RFFE_PA_BIAS_DAC=(RFFE_BASE_ADDRESS + 0x28),                /*!< Unused */
RFFE_RX_GAIN=(RFFE_BASE_ADDRESS + 0x2C),                    /*!< Sets the RX baseband and RF gain */
RFFE_TX_VGA_GAIN=(RFFE_BASE_ADDRESS + 0x30)                 /*!< Sets the TX VGA gain */
}REGISTER_SPI_NUMBER;

/**
* Sets bit to update registers on the MAX2829 chip.
*/
typedef enum{
RFFE_REGISTER0_UPDATE=0x4,                      /*!< Unused */
RFFE_REGISTER1_UPDATE=0x8,                      /*!< Unused */
RFFE_STANDBY_UPDATE=0x10,                       /*!< Updates the standby register */
RFFE_INTEGER_DIVIDER_RATIO_UPDATE=0x20,         /*!< Updates the integer portion of the divider ratio of the synthesizer */
RFFE_FRACTIONAL_DIVIDER_RATIO_UPDATE=0x40,      /*!< Updates the fractionnal portion of the divider ratio of the synthesizer */
RFFE_BAND_SELECT_PLL_UPDATE=0x80,               /*!< Updates the band select register */
RFFE_CALIBRATION_UPDATE=0x100,                  /*!< Updates the calibration register */
RFFE_LOWPASS_FILTER_UPDATE=0x200,               /*!< Updates the lowpass filter register */
RFFE_RX_CONTROL_RSSI_UPDATE=0x400,              /*!< Updates RSSI control register */
RFFE_TX_LINEARITY_BB_GAIN_UPDATE=0x800,         /*!< Updates the TX gain and linearity */
RFFE_PA_BIAS_DAC_UPDATE=0x1000,                 /*!< Unused */
RFFE_RX_GAIN_UPDATE=0x2000,                     /*!< Updates the RX baseband and RF gain */
RFFE_TX_VGA_GAIN_UPDATE=0x4000                  /*!< Updates the TX VGA gain */
}REGISTER_SPI_UPDATE;


/* Common RFFE register definition */
#define RFFE_ADDRESS                    (RFFE_BASE_ADDRESS + 0x34)
#define RFFE_PARALLEL_GAIN              (RFFE_BASE_ADDRESS + 0x38)
#define RFFE_PABS_DSW_OSCS              (RFFE_BASE_ADDRESS + 0x3C)
#define RFFE_FREQ_SYNTH_LOCK_DETECT     (RFFE_BASE_ADDRESS + 0x40)
#define RFFE_OPERATION_MODE1            (RFFE_BASE_ADDRESS + 0x44)
#define RFFE_OPERATION_MODE2            (RFFE_BASE_ADDRESS + 0x48)
#define RFFE_OPERATION_MODE3            (RFFE_BASE_ADDRESS + 0x4C)
#define RFFE_OPERATION_MODE4            (RFFE_BASE_ADDRESS + 0x50)


/*TX Register Definitions*/

#define RFFE_POWER_ADC_IN               (RFFE_BASE_ADDRESS + 0x5C)
#define RFFE_POWER_ADC_OUT              (RFFE_BASE_ADDRESS + 0x60)

/*! \def RFFE_IOCTLR IO control register */
#define RFFE_IOCTLR                     (RFFE_BASE_ADDRESS + 0x6C)
/*! \def RFFE_LYRTECH RFFE Core version */
#define RFFE_LYRTECH                    (RFFE_BASE_ADDRESS + 0x70)


/*! \def RFFE_EXTCTLR External control register */
#define RFFE_EXTCTLR                    (RFFE_BASE_ADDRESS + 0x74)

/**
* Allows controlling the operation mode from within the FPGA without writing to a register.
*/
typedef enum {
MODE_EXTCTRL_OFF=0,   /*!< A register controls the operation mode */
MODE_EXTCTRL_ON=0x1   /*!< The FPGA controls the operation mode */
} MODE_EXTCTRL;

/**
* Allows selecting the transceiver from within the FPGA without writing to a register.
*/
typedef enum {
ADD_EXTCTRL_OFF=0,    /*!< A register selects the transceiver to be used */
ADD_EXTCTRL_ON=0x2    /*!< The FPGA selects the transceiver to be used */
} ADD_EXTCTRL;

/**
* Allows controlling the gain from within the FPGA without writing to a register.
*/
typedef enum {
GAIN_EXTCTRL_OFF=0,   /*!< A register controls the gain */
GAIN_EXTCTRL_ON=0x4   /*!< The FPGA controls the gain */
} GAIN_EXTCTRL;

/**
* Allows controlling the frequency from within the FPGA without writing to a register.
*/
typedef enum {
FREQ_EXTCTRL_OFF=0,   /*!< A register controls the Frequency */
FREQ_EXTCTRL_ON=0x10   /*!< The FPGA controls the Frequency */
} FREQ_EXTCTRL;


/* Updates bits definitions */
#define GAIN_GEN                0x1

#define SPI_EN                  0x40000000
#define DIN_SPI2_UPDATE         0x1000
#define DOUT_SPI2_UPDATE        0x10000
#define MODE_LATCH              0x200

/**
* Indicates whether the frequency synthesizer is in locked state.
*/
typedef enum{
LD_UNLOCKED=0,      /*!< The frequency synthesizer is not in locked state */
LD_LOCKED,          /*!< The frequency synthesiser is in locked state */
LD_ERROR            /*!< An error occured */
}LOCK_DETECT_STATE;


/*Functions Definitions*/

/**
* Initializes the RFFE.
*
* \param[in] h Board handle.
*/
extern far RETURN_STATUS RFFE_Init(HANDLE h);

/**
* The transceiver's registers through the serial parallel interface (SPI).
*
* \param[in] h Board handle.
* \param[in] transceiver Transceiver selected to be progammed.
* \param[in] spi Register address within the FPGA.
* \param[in] data Data to be programmed.
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetRegisterSPI(HANDLE h, TRANSCEIVER_NUMBER transceiver, REGISTER_SPI_NUMBER spi, unsigned int data);

/**
* Programs the transceiver's gain with the B1 to B7 parallel pins of the GPIO connector.
*
* \param[in] h Board handle.
* \param[in] transceiver Transceiver selected to be progammed.
* \param[in] data Gain to program. Refer to the Quad Dual Band RF Transceiver guide for the correct format to use.
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetParallelGain(HANDLE h, TRANSCEIVER_NUMBER transceiver, unsigned int data);

/**
* Programs the transceiver's operating mode.
*
* \param[in] h Board handle.
* \param[in] transceiver Transceiver selected to be progammed.
* \param[in] data One of the seven allowed operating modes.
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetMode(HANDLE h, TRANSCEIVER_NUMBER transceiver, TRANSCEIVER_MODE data);

/**
* Reads the lock-detect digital output of the frequency synthesizer.
*
* \param[in] h Board handle.
* \param[in] transceiver Specifies from what transceiver to read.
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far LOCK_DETECT_STATE RFFE_GetFreqSynthLockDetect(HANDLE h, TRANSCEIVER_NUMBER transceiver);

/**
* Sends data to the PABS, DSW, and OSCS pins of the RFFE connector.
*
* \param[in] h Board handle.
* \param[in] pabs Selects between two states (on and off).
* \param[in] dsw Selects between two states (on and off).
* \param[in] oscs Selects between two states (on and off).
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetPabsDswOscs(HANDLE h, PABS_STATE pabs, DSW_STATE dsw, OSCS_STATE oscs);

/**
* Enables or disables the high-pass filter of a transceiver
*
* \param[in] h Board handle.
* \param[in] transceiver Selects what transceiver, hence what pin, to which to send data.
* \param[in] state Selects between two states (on and off).
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetRxhp(HANDLE h, TRANSCEIVER_NUMBER transceiver, RXHP_STATE state);

/**
* Enables or disables the power amplifier of a transceiver
*
* \param[in] h Board handle.
* \param[in] transceiver Selects what transceiver (hence what pin) to which to send data.
* \param[in] state Selects between two states (on and off).
*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetPaen(HANDLE h, TRANSCEIVER_NUMBER transceiver, PAEN_STATE state);
/**
* Allows controlling the mode, the gain or the ADD bit from within the FPGA without writing to a register
* \param[in] h Board handle.
* \param[in] mode use MODE_EXTCTRL_ON or MODE_EXTCTRL_OFF.
* \param[in] add use ADD_EXTCTRL_ON or ADD_EXTCTRL_OFF.
* \param[in] gain use GAIN_EXTCTRL_ON or GAIN_EXTCTRL_OFF.
* \param[in] freq use FREQ_EXTCTRL_ON or GAIN_EXTCTRL_OFF.

*
* \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
*/
extern far RETURN_STATUS RFFE_SetExternalControl(HANDLE h, MODE_EXTCTRL mode, ADD_EXTCTRL add, GAIN_EXTCTRL gain, FREQ_EXTCTRL freq);

extern far RETURN_STATUS RFFE_SetSpi2Din(HANDLE h, unsigned int data);

extern far unsigned int RFFE_GetSpi2Dout(HANDLE h);

#ifdef __cplusplus
}
#endif

#endif

