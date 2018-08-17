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
 *  Project     : VHS-ADC HIGH LEVEL CONTROL API    (HEADER)
 *  File        : vhsadc16ctrl.h
 *  Description : WIN32 CONTROL API for VHS-ADC Boards.
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
 *  20/04/06   JPLA      2.20       Doxygenification (API DOC!)
 ***************************************************************************
 *  "$RCSfile: vhsadc16ctrl.h,v $"
 *  "$Author: patrick.gilbert $"
 *  "$Date: 2008/05/02 15:24:17 $"
 ***************************************************************************/
/*! \file vhsadc16ctrl.h
  \brief VHS-ADC control API.

  This section describes all the functions used by the high-level API.<BR>
  Refer to this API first when developing applications meant to communicate
  with the VHS-ADC, as they contain application-specific routines.
*/
/* The host CPU and DSP applications share the same high-level API. */

#ifndef _VHSADC16CTRL_H_
#define _VHSADC16CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vhsadac16_regs.h"


#ifdef WIN32
#include "vhsadac16_host_api.h"
#else
#include "dsplib.h"
#include "vhsadac16_dsp_api.h"
#endif

/*
#define GAIN_ENABLE_CH1                 0x00000001
#define GAIN_ENABLE_CH2                 0x00000002
#define GAIN_ENABLE_CH3                 0x00000004
#define GAIN_ENABLE_CH4                 0x00000008
#define GAIN_ENABLE_CH5                 0x00000010
#define GAIN_ENABLE_CH6                 0x00000020
#define GAIN_ENABLE_CH7                 0x00000040
#define GAIN_ENABLE_CH8                 0x00000080
#define GAIN_ENABLE_CH9                 0x00000100
#define GAIN_ENABLE_CH10                0x00000200
#define GAIN_ENABLE_CH11                0x00000400
#define GAIN_ENABLE_CH12                0x00000800
#define GAIN_ENABLE_CH13                0x00001000
#define GAIN_ENABLE_CH14                0x00002000
#define GAIN_ENABLE_CH15                0x00004000
#define GAIN_ENABLE_CH16                0x00008000
#define GAIN_ENABLE_ALL_CHANNELS        0x0000FFFF
#define GAIN_DISABLE_ALL_CHANNELS       0x00000000

#define GAIN_ENABLE_CH(x)                (1 << (x-1))*/

/**
 * All VHS module types.
 */
typedef enum{
    VHS_MODULE_UNKNOWN = 0,        /*!< VHS-ADC/DAC Virtex-II module */
    VHS_MODULE_ADC_VIRTEX_2,       /*!< VHS-ADC Virtex-II module */
    VHS_MODULE_DAC_VIRTEX_2,       /*!< VHS-DAC Virtex-II module */
    VHS_MODULE_ADC_VIRTEX_4,       /*!< VHS-ADC Virtex-4 module */
    VHS_MODULE_DAC_VIRTEX_4,       /*!< VHS-DAC Virtex-4 module */
    VHS_MODULE_MEMORY              /*!< VHS Memory module */
} VHS_MODULE_TYPE;

/**
 * VHS-ADC/DAC LED control type.
 */
typedef enum{
    DEFAULT_CONFIGURABLE_LED,       /*!< VHS-ADC/DAC LED default configuration. */
    USER_CONFIGURABLE_LED           /*!< User-defined VHS-ADC/DAC LED configuration. */
}CONFIGURATION_LED_TYPE;

/**
 * VHS-ADC/DAC LED states.
 */
typedef enum{
    LED_OFF,                        /*!< VHS-ADC/DAC LED are off. */
    LED_ON                          /*!< VHS-ADC/DAC LED are on. */
}LED_STATE;

/**
 * VHS-ADC/DAC LED.
 */
typedef enum{
    LED_0,                          /*!< VHS-ADC/DAC LED 0. */
    LED_1                           /*!< VHS-ADC/DAC LED 1. */
}LED_NUMBER;

/**
 * VHS-ADC/DAC acquisition modes.
 */
typedef enum{
    ACQUISITION_OFF,                /*!< VHS-ADC/DAC acquisition off mode. */
    ACQUISITION_ON                  /*!< VHS-ADC/DAC acquisition on mode. */
}ACQUISITION_STATE;

/**
 * Acquisition clock sources.
 */
typedef enum{
    PROGRAMMABLE_CLK_SOURCE,        /*!< Programmable clock source. */
    EXTERNAL_CLK_SOURCE,            /*!< External clock source. */
    FPDP_CLK_SOURCE                 /*!< FPDP clock source. Note that this is only available to the VHS-DAC. */
}ACQUISITION_CLK_SOURCE;

/**
* VHS-ADC/DAC trigger sources.
*/
typedef enum{
    TRIGGER_MANUAL,
    TRIGGER_SIGNAL,
    TRIGGER_EXTERNAL
}VHS_TRIGGER_SOURCE;

/**
* VHS-ADC trigger polarity.
*/
typedef enum{
    TRIGGER_FALLING_EDGE,
    TRIGGER_RISING_EDGE
}VHS_TRIGGER_POLARITY;


typedef enum{
    OVR_CHANNEL_1,
    OVR_CHANNEL_2,
    OVR_CHANNEL_3,
    OVR_CHANNEL_4,
    OVR_CHANNEL_5,
    OVR_CHANNEL_6,
    OVR_CHANNEL_7,
    OVR_CHANNEL_8,
    OVR_CHANNEL_9,
    OVR_CHANNEL_10,
    OVR_CHANNEL_11,
    OVR_CHANNEL_12,
    OVR_CHANNEL_13,
    OVR_CHANNEL_14,
    OVR_CHANNEL_15,
    OVR_CHANNEL_16,
    OVR_OR_WITH_ALL_CHANNELS
}CHANNEL_OVR_SELECTOR;


/**
 * VHS-ADC/DAC gain level control configuration.
 */
typedef enum{
    GAIN_LEVEL_INDIVIDUAL,          /*!< Individual (per channel) gain control. */
    GAIN_LEVEL_ALL_LIKE_CHANNEL_1,  /*!< Gain control synchronized on channel 1 configuration. */
    GAIN_LEVEL_MAIN_BOARD_ONLY,     /*!< Configure gain control on main board only. */
    GAIN_LEVEL_HARDWARE_MODULE_ONLY /*!< Configure gain control on hardware module only. */
}GAIN_LEVEL_CONTROL;

/**
 * VHS-ADC/DAC gain modes.
 */
typedef enum{
    GAIN_MODE_REGISTER,             /*!< Gain controlled by the gain control register. */
    GAIN_MODE_SYSGEN                /*!< Gain controlled by System Generator for DSP. */
}GAIN_MODE;

/**
 * Return value.
 */
typedef enum{
    RETURN_ERROR,                   /*!< VHS-ADC/DAC error. */
    RETURN_NORMAL,                  /*!< No error. */
    RETURN_TIMEOUT,                 /*!< Timeout. */
    RETURN_CONTINUOUS,              /*!< Playback is in continuous mode; it will not stop by itself. Use VHSDAC_StopPlayback. */
    RETURN_NOTRUN,                  /*!< VHS-ADC/DAC is not in run state. */
    RETURN_RUNSTATE,                /*!< VHS-ADC/DAC must not be in run state. */
    RETURN_NOTSUPPORTED,            /*!< Unsupported functionality. */
    RETURN_INVALIDPARAM             /*!< The value of a parameter is invalid. */
} RETURN_STATUS;

/**
 * DCM lock status.
 */
typedef enum{
    DCMS_UNLOCKED,                  /*!< DCM is unlocked. */
    DCMS_LOCKED                     /*!< DCM is locked. */
}DCMS_STATUS;

/**
 * Gain programming status.
 */
typedef enum{
    PROG_GAINS_UNSET,               /*!< Gains programming error on timeout. */
    PROG_GAINS_SET,                 /*!< Gains sucessfully programmed. */
    PROG_GAINS_INVALID              /*!< Gains are invalid. */
}PROG_GAINS_STATUS;

/**
 * VHS-ADC/DAC types.
 */
typedef enum{
    VHSADC16_BT,                    /*!< VHS-ADC. */
    VHSDAC16_BT                     /*!< VHS-DAC. */
}VHSADAC16_B_T;

/**
 * VHS-ADC/DAC lock bit status.
 */
typedef enum{
    LOCK_BIT_OFF,                   /*!< Lock bit is off. */
    LOCK_BIT_ON                     /*!< Lock bit is on. */
}LOCK_BIT_STATUS;

/**
 * VHS-ADC/DAC lock status.
 */
typedef enum{
    VHS_UNLOCKED,                   /*!< VHS-ADC/DAC is unlocked. */
    VHS_LOCKED                      /*!< VHS-ADC/DAC is locked. */
}VHS_LOCK_STATUS;

/**
 * VHS-ADC/DAC trigger modes.
 */
typedef enum{
    SINGLE_SHOT,                    /*!< Single shot. */
    CONTINUOUS                      /*!< Continuous. */
} VHS_TRIGGER_MODE;


/**
 * VHS-ADC record input source modes.
 */
typedef enum
{
  ADC_NORMAL_ACQUISITION, /*!< Normal acquisition. */
  ADC_RAMP_TEST           /*!< Internally generated ramp test. */
}VHS_ADC_REC_SRC;


/**
 * VHS-ADC state of the record controller.
 */
typedef enum
{
  RECORD_IDLE = 0x0,                   /*!< The controller is idle. */
  RECORD_ARMED = 0x1,                  /*!< The controller is waiting for a trigger event. */
  RECORD_WAITING = 0x2,                /*!< A trigger event occurred, the controller is waiting for delay. */
  RECORD_STORING = 0x3,                /*!< A trigger event occurred, the controller is recording data. */
  RECORD_READY = 0x4,                  /*!< Frame count = Frame size, finished recording. */
  RECORD_PRE_READY = 0x5,              /*!< The controller is waiting for pending recording transfers. */
  RECORD_DATA_SELECT_ERROR = 0x8,
  RECORD_STORAGE_OVERRUN_ERROR = 0xA,  /*!< Recording storage sub-module not ready while already recording. */
  RECORD_PACKING_OVERRUN_ERROR = 0xB,  /*!< PACKING OVERRUN ERROR (overrun in recording packing) */
  RECORD_DATA_OVERRUN_ERROR = 0xC      /*!< Overrun in recording receiver. */
}VHS_RECORD_STATE;


/**
 * VHS-DAC state of the playback controller.
 */
typedef enum
{
  PLAYBACK_IDLE = 0x0,                     /*!< The controller is idle. */
  PLAYBACK_ARMED = 0x1,                    /*!< The controller is waiting for a trigger event. */
  PLAYBACK_WAITING = 0x2,                  /*!< A trigger event occurred, the controller is waiting for delay. */
  PLAYBACK_PLAYING = 0x3,                  /*!< A trigger event occurred, the controller is playing back data. */
  PLAYBACK_READY = 0x4,                    /*!< Frame count = Frame size, finished playback. */
  PLAYBACK_PRELOAD = 0x7,                  /*!< The controller is waiting for playback data to be ready. */
  PLAYBACK_DATA_SELECT_ERROR = 0x8,
  PLAYBACK_STORAGE_UNDERRUN_ERROR = 0xA,   /*!< Underrun in playback storage module. */
  PLAYBACK_PACKING_UNDERRUN_ERROR = 0xB,   /*!< Underrun in playback unpacking module */
  PLAYBACK_DATA_UNDERRUN_ERROR = 0xC       /*!< Underrun in playback transmitter module. */
}VHS_PLAYBACK_STATE;


/**
 * \fn HANDLE VHSADC_Open(VHSADAC16_B_T board_type,unsigned int sm_board_num)
 *
 * Initializes handles and returns handles on VHS-ADC/DAC.
 *
 * \param[in] board_type VHS-ADC/DAC Type.
 * \param[in] sm_board_num VHS-ADC number in the system.
 *
 * \return Valid VHS-ADC/DAC handle if the board is found, otherwise INVALID_HANDLE-VALUE.
 */
HANDLE VHSADC_Open(VHSADAC16_B_T board_type,unsigned int sm_board_num);

/**
 * Closes a handle to release the device.
 *
 * \param[in] handle VHS-ADC handle.
 */
void VHSADC_Close(HANDLE handle);

/**
 * Resets FPGA control registers to default values.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSADC_FPGAInternalReset(HANDLE h);

/**
 * Returns the FPGA Lyrtech core revision number.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return FPGA core revision number.
 */
unsigned char VHSADC_GetFPGARevision(HANDLE h);

/**
 * Sets the green and red LED status on/off.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] lednumber LED number.
 * \param[in] userdefined LED type.
 * \param[in] state LED state.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSADC_SetLed(HANDLE h, LED_NUMBER lednumber, CONFIGURATION_LED_TYPE userdefined, LED_STATE state);


/**
 * Returns the FPGA DCM lock status.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return DCM lock status.
 */
DCMS_STATUS VHSADC_GetDCMsStatus(HANDLE h);

/**
 * Resets the FPGA DCM.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return DCM lock status.
 */
DCMS_STATUS VHSADC_DCMsReset(HANDLE h);

/**
 * Returns the FPDP DCM lock status.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return DCM lock status.
 */
DCMS_STATUS VHSADC_GetFPDPDCMsStatus(HANDLE h);

/**
 * Sets the acquisition status on/off and activates control logic.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] state Acquisition state.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetAcquisitionStatus(HANDLE h, ACQUISITION_STATE state);

/**
 * Enables or disables the digital trigger mechanism. The VHS-ADC must not be in the run state when you use this function.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] triggerEnable True enables the digital trigger mechanism.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetTriggerEnable(HANDLE h, BOOL triggerEnable);

/**
* Sets the trigger source. To use the external trigger source, you must enable the trigger logic with the VHSADC_SetTriggerEnable function
*
* \param[in] h VHS-ADC handle.
* \param[in] source TRIGGER_MANUAL for manual trigger, TRIGGER_EXTERNAL for external trigger
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSADC_SetTriggerSource(HANDLE h, VHS_TRIGGER_SOURCE source);

/**
 * Returns VHS-ADC OVR flags.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] channel Channel selection.
 *
 * \return OVR flag status for the channel.
 */
unsigned char VHSADC_GetADCsOVR(HANDLE h, CHANNEL_OVR_SELECTOR channel);

/**
 * Sets VHS-ADC channel gain levels.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] ctrl Gain level control.
 * \param[in] plevel Gain levels.
 *
 * \return Gain status.
 */
PROG_GAINS_STATUS VHSADC_SetGainLevel(HANDLE h, GAIN_LEVEL_CONTROL ctrl, unsigned short *plevel);/* Level = 0 to 4095*/

/**
 * Sets the gain control mode (register or System Generator for DSP).
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] mode Gain mode.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetGainMode(HANDLE h, GAIN_MODE mode);

/**
 * Gets the gain level from the register.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] gainlevelnumber Gain channel number.
 *
 * \return Gain level for channel.
 *
 * \todo FIX ON ERROR RETURN VALUE!
 */
unsigned int VHSADC_GetGainLevelReg( HANDLE h, unsigned int gainlevelnumber );

/**
 * Sets the 32-bit user control registers.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] userctrlnumber User control register number.
 * \param[in] data 32-bit data to write in control register.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetUserCtrlReg(HANDLE h, unsigned int userctrlnumber, unsigned int data);

/**
 * Gets the 32-bit user control registers.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] userctrlnumber User control register number.
 *
 * \return 32-bit data to read from the control register.
 */
unsigned int VHSADC_GetUserCtrlReg(HANDLE h, unsigned int userctrlnumber);

/**
 * Sets the lock bit to access VHS-ADC resources from a Lyrtech DSP board.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] status Lock status bits.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetLockStatus(HANDLE h, LOCK_BIT_STATUS status);

/**
 * Gets the lock bit status.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return VHS-ADC lock status.
 */
VHS_LOCK_STATUS VHSADC_GetLockStatus(HANDLE h);


/**
 * Enables the specified VHS-ADC SDRAM signal for recording. Also defines signal priority in the enabled channel list. The first enabled channel is also the first in SDRAM and so on.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] signalNum SDRAM signal selection.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_EnableRecord (HANDLE h, unsigned int signalNum);


/**
 * Sets the VHS-ADC record frame size.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] frameSize Record frame size (number of samples that each enabled channel must record).
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetRecordFramesize(HANDLE h, unsigned long frameSize);


/**
 * Sets the VHS-ADC record data source.
 *
 * \param[in] h VHS-ADC handle.
 * \param[in] source ADC_NORMAL_ACQUISITION for normal acquisition, ADC_RAMP_TEST for recording an internal saw tooth.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_SetRecordInputSource(HANDLE h, VHS_ADC_REC_SRC source);

/**
 * Starts the VHS-ADC recording on enabled signals.
 *
 * \param[in] h VHS-ADC handle.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSADC_StartRecord(HANDLE h);

/**
* Waits for the end of the recording and clears the enabled channel list.
*
* \param[in] h VHS-ADC handle.
* \param[in] timeout Timeout in milliseconds. When set to zero, timeout is set to a default value which is 500000 milliseconds.
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSADC_WaitEndofRecord(HANDLE h, unsigned int timeout);


/**
* Get the current state of the record controller.
*
* \param[in] h VHS-ADC handle.
* \param[in] state Current state of the record controller (value returned by reference).
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSADC_GetRecordState( HANDLE h, VHS_RECORD_STATE* state );

/**
* Sets the trigger level.
* Use this function only whith the trigger source set to signal.
*
* \param[in] h VHS-ADC handle.
* \param[in] level This 16-bit value indicates the trigger level used by the trigger control logic in the record controller.
* The trigger level is compared with the signal's data value to determine the trigger event when the signal trigger source is selected.
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS vhsadc_settriggerlevel(HANDLE h, unsigned short level);

/**
* Sets the trigger delay.
* This is the delay between the trigger event and the start of recording.
*
* \param[in] h VHS-ADC handle.
* \param[in] delay This 31-bit value indicates how many clock cycles (at 156.25 MHz)
* the record controller waits between the trigger event and the actual start of the recording.
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSADC_SetTriggerDelay(HANDLE h, unsigned int delay);

/**
* Sets the trigger polarity.
* Use this function only whith the trigger source set to signal.
*
* \param[in] h VHS-ADC handle.
* \param[in] pol TRIGGER_FALLING_EDGE for a falling edge trigger, TRIGGER_RISING_EDGE for a rising edge trigger
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSADC_SetTriggerPolarity(HANDLE h, VHS_TRIGGER_POLARITY pol);

/**
* Selects a single channel source to feed the trigger logic
* (for recording only when the signal trigger source is selected).
*
* \param[in] h VHS-ADC handle.
* \param[in] signalNum SDRAM signal selection.
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSADC_SetTriggerSignal(HANDLE h, unsigned int signalNum);


/**
* Verifies whether the VHS-ADC is DC coupled.
*
* \param[in] h VHS-ADC handle.
*
* \return True (DC coupled) or false (not DC coupled).
*/
RETURN_STATUS VHSADC_IsDCCoupled(HANDLE h);


/**
* Verifies the presence of hardware modules.
*
* \param[in] h VHS-ADC/DAC handle.
*
* \return 1 if a hardware module is present, 0 if not
*/
unsigned int VHSADC_IsModulePresent(HANDLE h);

/**
* Returns the ID of the specified type of module detected.
*
* \param[in] h VHS-ADC/DAC handle.
*
* \return VHS-ADC/DAC module Identifier.
*/
VHS_MODULE_TYPE VHSADC_GetModuleType(HANDLE h);

/**
* Verifies whether the programmable gain is available for the module
*
* \param[in] h VHS-ADC/DAC handle.
*
* \return 1 if programmable gain is available, 0 if it is not
*/
unsigned int VHSADC_IsModuleHasProgrammableGain(HANDLE h);

/**
* Verifies whether the VHS module is DC coupled.
*
* \param[in] h VHS-ADC/DAC handle.
*
* \return 1 if DC coupled or 0 if it is not.
*/
unsigned int VHSADC_IsModuleDCCoupled(HANDLE h);

/**
* Verifies whether that record interface is available.
*
* \param[in] h VHS-ADC/DAC handle.
*
* \return 1 if record interface is available or 0 if it is not.
*/
ULONG VHSADC_isRecordInterfaceDetected(HANDLE h);



#ifdef __cplusplus
}
#endif

#endif /* _VHSADC16CTRL_H_ */

