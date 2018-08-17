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
 *  File        : vhsdac16ctrl.h
 *  Description : WIN32 CONTROL API for VHS-DAC Boards.
 *
 *  Author(s)   : Jérémie Plante
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
 *  20/04/06   JPLA      2.20       Doxygenification (API DOC!)
 ***************************************************************************
 *  "$RCSfile: vhsdac16ctrl.h,v $"
 *  "$Author: caroline.pouliot $"
 *  "$Date: 2008/03/25 18:50:14 $"
 ***************************************************************************/
/*! \file vhsdac16ctrl.h
  \brief VHS-DAC control API.

  This section describes all the functions used by the high-level API.<BR>
  Refer to this API first when developing applications meant to communicate
  with the VHS-DAC, as they contain application-specific routines.
*/
/* The host CPU and DSP applications share the same high-level API. */

#ifndef _VHSDAC16CTRL_H_
#define _VHSDAC16CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vhsadac16_regs.h"

#ifdef WIN32
#include "vhsadac16_host_api.h"
#else
#include "vhsadac16_dsp_api.h"
#endif

#include "vhsadc16ctrl.h"

#define TRANSMISSION_STATE          ACQUISITION_STATE
#define TRANSMISSION_ON             ACQUISITION_ON
#define TRANSMISSION_OFF            ACQUISITION_OFF


/**
 * VHS-ADC/DAC playback modes.
 */
typedef enum{
    PLAYBACK_OFF,                /*!< VHS-ADC/DAC playback is off. */
    PLAYBACK_ON                  /*!< VHS-ADC/DAC playback is on. */
}PLAYBACK_STATE;


#ifdef DOXYGEN_PROTOTYPE_DECLARATION
/**
 * Sets the green and red LED status on/off.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] lednumber LED number.
 * \param[in] userdefined LED type.
 * \param[in] state LED state.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetLed(HANDLE h, LED_NUMBER lednumber, CONFIGURATION_LED_TYPE userdefined, LED_STATE state);

/**
 * Resets FPGA control registers to their default values.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_FPGAInternalReset(HANDLE h);

/**
 * Returns the FPGA Lyrtech core revision number.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return FPGA core revision number.
 */
unsigned char VHSDAC_GetFPGARevision(HANDLE h);

/**
 * Sets the transmission status on/off and activate control logic
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] state Transmission state.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetTransmissionStatus(HANDLE h, TRANSMISSION_STATE state);

/**
 * Enables or disables the digital trigger mechanism.
 * The VHS-ADC/DAC must not be in the RUN state when you use this function.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] triggerEnable True enables the digital trigger mechanism.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetTriggerEnable(HANDLE h, BOOL triggerEnable);

/**
 * Returns the FPGA DCM lock status.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return DCM lock status.
 */
DCMS_STATUS VHSDAC_GetDCMsStatus(HANDLE h);

/**
 * Resets the FPGA DCM.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return DCM lock status.
 */
DCMS_STATUS VHSDAC_DCMsReset(HANDLE h);

/**
 * Acquires the lock bit status.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return VHS-DAC lock status.
 */
VHS_LOCK_STATUS VHSDAC_GetLockStatus(HANDLE h);

/**
 * Sets the lock bit to access VHS-DAC resources from a Lyrtech DSP board.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] status Lock status bits.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetLockStatus(HANDLE h, LOCK_BIT_STATUS status);

/**
 * Sets channel gain bypass analog switches.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] ctrl Gain level control.
 * \param[in] plevel Gain levels.
 *
 * \return Gain status.
 */
PROG_GAINS_STATUS VHSDAC_SetGainLevel(HANDLE h, GAIN_LEVEL_CONTROL ctrl, unsigned short plevel[]);

/**
 * Acquires the gain level from the register.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] gainlevelnumber Gain channel number.
 *
 * \return Gain level for channel.
 *
 * \todo FIX ON ERROR RETURN VALUE!
 */
unsigned int VHSDAC_GetGainLevelReg( HANDLE h, unsigned int gainlevelnumber );

/**
 * Sets the gain control mode (register/System Generator for DSP).
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode Gain mode.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_SetGainMode(HANDLE h, GAIN_MODE mode);

/**
 * Sets the 32-bit user control registers.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] userctrlnumber User control register number.
 * \param[in] data 32-bit data to write in control register.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetUserCtrlReg(HANDLE h, unsigned int userctrlnumber, unsigned int data);

/**
 * Acquires the 32-bit user control registers.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] userctrlnumber User control register number.
 *
 * \return 32-bit data read from the control register.
 */
unsigned int VHSDAC_GetUserCtrlReg(HANDLE h, unsigned int userctrlnumber);

/**
 * Initializes handles and returns handles of VHS-ADC/DAC.
 *
 * \param[in] board_type VHS-DAC handle.
 * \param[in] sm_board_num VHS-ADC/DAC number in the system.
 *
 * \return A valid VHS-ADC/DAC handle if the board is found or INVALID_HANDLE_VALUE.
 */
HANDLE VHSDAC_Open(VHSADAC16_B_T board_type,unsigned int sm_board_num);

/**
 * Closes a handle to release the device.
 *
 * \param[in] handle VHS-DAC handle.
 */
void VHSDAC_Close(HANDLE handle);

/**
 * Enables the spcified VHS-DAC SDRAM signal for playback.
 * You must use this function for every signal, in the same order the signals are found in memory.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] signalNum SDRAM signal selection.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_EnablePlayback (HANDLE h, unsigned int signalNum);

/**
 * Sets the VHS-DAC playback frame size.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] frameSize Playback frame size (number of samples that each enabled channel must play back).
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetPlaybackFramesize(HANDLE h, unsigned long frameSize);

/**
 * Sets the VHS-DAC trigger mode.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] triggerMode VHS-DAC trigger mode.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_SetTriggerMode (HANDLE h, TRIGGER_MODE triggerMode);

/**
* Sets the trigger source.
* To use the external trigger source, you must enable the trigger logic with the VHSADC_SetTriggerEnable function
*
* \param[in] h VHS-DAC handle.
* \param[in] source TRIGGER_MANUAL for a manual trigger, TRIGGER_EXTERNAL for an external trigger
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSDAC_SetTriggerSource(HANDLE h, VHS_TRIGGER_SOURCE source);


/**
* Sets the trigger delay.
* To use the external trigger source, you must enable the trigger logic with the VHSADC_SetTriggerEnable function
*
* \param[in] h VHS-ADC/DAC handle.
* \param[in] delay This 31-bit value indicates how many clock cycles (at 156.25 MHz)
* the record controller waits between the trigger event and the actual start of the recording.
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSDAC_SetTriggerDelay(HANDLE h, unsigned int delay);

/**
 * Starts the VHS-DAC playback on enabled signals.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_StartPlayback(HANDLE h);

/**
 * Stops the VHS-DAC playback and clears the enabled channels list.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return RETURN_NORMAL on success, anything else is an error.
 */
RETURN_STATUS VHSDAC_StopPlayback (HANDLE h);

/**
* Waits for the end of the playback and clears the enabled channels list.
* The VHS-ADC/DAC must be configured in one-shot mode for the function to operate correctly.
*
* \param[in] h VHS-DAC handle
* \param[in] timeout Timeout in milliseconds. When set to zero, timeout is set to a default value which is 500000 milliseconds.
*
* \return RETURN_NORMAL on success, anything else is an error.
*/
RETURN_STATUS VHSDAC_WaitEndofPlayback(HANDLE h, unsigned int timeout);


#else


#define VHSDAC_Open(board_type,sm_board_num)                        VHSADC_Open(board_type,sm_board_num)

#define VHSDAC_Close(handle)                                        VHSADC_Close(handle)

#define VHSDAC_FPGAInternalReset(h)                                 VHSADC_FPGAInternalReset(h)

#define VHSDAC_GetFPGARevision(h)                                   VHSADC_GetFPGARevision(h)

#define VHSDAC_SetLed(h, lednumber, userdefined, state)             VHSADC_SetLed(h, lednumber, userdefined, state)

#define VHSDAC_GetDCMsStatus(h)                                     VHSADC_GetDCMsStatus(h)

#define VHSDAC_DCMsReset(h)                                         VHSADC_DCMsReset(h)

#define VHSDAC_GetFPDPDCMsStatus(h)									VHSADC_GetFPDPDCMsStatus(h)

#define VHSDAC_SetTriggerEnable(h,enable)                           VHSADC_SetTriggerEnable(h,enable)

#define VHSDAC_SetTransmissionStatus(h,state)                       VHSADC_SetAcquisitionStatus(h,state)

#define VHSDAC_SetGainMode(h,mode)                                  VHSADC_SetGainMode(h,mode)

#define VHSDAC_SetUserCtrlReg(h,userctrlnumber,data)                VHSADC_SetUserCtrlReg(h,userctrlnumber,data)

#define VHSDAC_GetUserCtrlReg(h,userctrlnumber)                     VHSADC_GetUserCtrlReg(h,userctrlnumber)

#define VHSDAC_SetGainLevel(h,ctrl,plevel)/* Level = 0 to 255*/     VHSADC_SetGainLevel(h,ctrl,plevel)

#define VHSDAC_GetGainLevelReg(h,gainlevelnumber )                  VHSADC_GetGainLevelReg(h,gainlevelnumber )

#define VHSDAC_SetLockStatus(h,status)                              VHSADC_SetLockStatus(h,status)

#define VHSDAC_GetLockStatus(h)                                     VHSADC_GetLockStatus(h)


far RETURN_STATUS VHSDAC_EnablePlayback (HANDLE h, unsigned int signalNum);

far RETURN_STATUS VHSDAC_WaitEndofPlayback(HANDLE h, unsigned int timeout);

far RETURN_STATUS VHSDAC_SetPlaybackFramesize(HANDLE h, unsigned long frameSize);
far RETURN_STATUS VHSDAC_SetTriggerMode (HANDLE h, VHS_TRIGGER_MODE triggerMode);
far RETURN_STATUS VHSDAC_SetTriggerSource(HANDLE h, VHS_TRIGGER_SOURCE source);

far RETURN_STATUS VHSDAC_StartPlayback(HANDLE h);
far RETURN_STATUS VHSDAC_StopPlayback (HANDLE h);

far ULONG VHSDAC_isPlaybackInterfaceDetected(HANDLE h);


#endif /* DOXYGEN_PROTOTYPE_DECLARATION */

#ifdef __cplusplus
}
#endif

#endif /* _VHSDAC16CTRL_H_ */

