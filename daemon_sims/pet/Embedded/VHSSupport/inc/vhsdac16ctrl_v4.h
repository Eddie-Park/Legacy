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
 *  Project     : VHS-DAC V4 HIGH LEVEL CONTROL API     (HEADER)
 *  File        : vhsadc16ctrl_v4.h
 *  Description : WIN32 CONTROL API for VHS-DAC V4 Boards.
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
 *  09/12/05   JPLA      2.00       V4 HIGH LEVEL API.
 *  20/04/06   JPLA      2.20       Doxygenification (API DOC!)
 ***************************************************************************
 *  "$RCSfile: vhsdac16ctrl_v4.h,v $"
 *  "$Author: simon.boucher $"
 *  "$Date: 2008/01/22 19:26:33 $"
 ***************************************************************************/
/*! \file vhsdac16ctrl_v4.h
  \brief VHS-DAC Virtex-4-specific API.

  This section describes all the functions used by the high-level API.<BR>
  Refer to this API first when developing applications meant to communicate
  with the VHS-DAC Virtex-4, as they contain application-specific routines.
*/
/* The host CPU and DSP applications share the same high-level API. */


#ifndef _VHSDAC16CTRL_V4_H_
#define _VHSDAC16CTRL_V4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vhsadac16_regs.h"


#ifdef WIN32
#include "vhsadac16_host_api.h"
#else
#include "vhsadac16_dsp_api.h"
#endif

#include "vhsadc16ctrl_v4.h"

/**
 * VHS DAC5687 Identificator. Only available on VHS DAC V4.
 */
typedef enum {
    VHS_DAC5687_ID_CARRIER_BOARD_1 = 0, /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_CARRIER_BOARD_2,     /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_CARRIER_BOARD_3,     /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_CARRIER_BOARD_4,     /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_HARDWARE_MODULE_1,   /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_HARDWARE_MODULE_2,   /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_HARDWARE_MODULE_3,   /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_HARDWARE_MODULE_4,   /* !< Identificator of the DAC5687 Chip. */
    VHS_DAC5687_ID_MAX                  /* !< Maximum number of the DAC5687 Chip. */
} VHS_DAC5687_ID;

typedef enum
{
    VHS_DAC5687_PLL_DIV_1 = 0,
    VHS_DAC5687_PLL_DIV_2,
    VHS_DAC5687_PLL_DIV_4,
    VHS_DAC5687_PLL_DIV_8,
    VHS_DAC5687_PLL_DIV_MAX
} VHS_DAC5687_PLL_DIV;

typedef enum {
    VHS_DAC5687_FIR_INTRPOL_FULL_BYPASS = 0,
    VHS_DAC5687_FIR_INTRPOL_X2_MODE,
    VHS_DAC5687_FIR_INTRPOL_X4_MODE,
    VHS_DAC5687_FIR_INTRPOL_X4L_MODE,
    VHS_DAC5687_FIR_INTRPOL_X8_MODE,
    VHS_DAC5687_FIR_INTRPOL_MAX
} VHS_DAC5687_FIR_INTRPOL;

typedef enum {
    NO_MIXING,
    F2_DA_MAPA_DB_MBPB = 4,
    F2_DA_MAPA_DB_PBMB,
    F2_DA_PAMA_DB_MBPB,
    F2_DA_PAMA_DB_PBMB,
    F4_DA_PAMB_DB_PBPA,
    F4_DA_PAMB_DB_MBMA,
    F4_DA_MAPB_DB_PBPA,
    F4_DA_MAPB_DB_MBMA,
    MF4_DA_PAPB_DB_PBMA,
    MF4_DA_PAPB_DB_MBPA,
    MF4_DA_MAMB_DB_PBMA,
    MF4_DA_MAMB_DB_MBPA
} VHS_DAC5687_COAR_MIX_MODE;

typedef enum {
    VHS_DAC5687_FIR1_MODE_LOW_PASS = 0,
    VHS_DAC5687_FIR1_MODE_HIGH_PASS,
    VHS_DAC5687_FIR1_MODE_MAX
} VHS_DAC5687_FIR1_MODE;

#ifdef DOXYGEN_PROTOTYPE_DECLARATION
/**
 * Sets under what mode the clock is controlled.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True indicates that the clock is controlled by the HST_CLK_CTRL register,
 *            otherwise by the custom logic.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_HostControlClock(HANDLE h, BOOL mode);

/**
 * Sets the DIF clock divider.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True indicates that the DIF clock is divided by two, otherwise the clock is not divided.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DivideDIFClock(HANDLE h,BOOL mode);

/**
 * Queries whether the DIF clock is divided.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return True if the DIF clock is divided by two.
 */
BOOL VHSDAC_IsDIFClockDivided(HANDLE h);

/**
 * Queries whether the DIF clock is controlled by the custom logic.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return True if the DIF clock is controlled by the custom logic.
 */
BOOL VHSDAC_IsDIFClockSysgenControlled(HANDLE h);

/**
 * Queries the clock source.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return Clock source.
 */
CLOCK_SOURCE VHSDAC_GetClockSource(HANDLE h);

/**
 * Sets the clock source.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] clkSource VHS-DAC clock source.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_SetClockSource(HANDLE h,CLOCK_SOURCE clkSource);

/**
 * Sets the SERDES control mode.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True indicates that the SERDES is controlled by the DIF_HOST_CTRL register,
 *            otherwise by the custom logic.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_HostControlSerdes(HANDLE h, BOOL mode);

/**
 * Sets the SERDES transmitter output enable state.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True enables the SERDES transmitter output.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_EnableSerdesTransmitter(HANDLE h, BOOL mode);

/**
 * Sets the SERDES receiver input enable state.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True enables the SERDES receiver input.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_EnableSerdesReceiver(HANDLE h, BOOL mode);

/**
 * Sets the SERDES transmitter loopback state.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True enables the SERDES transmitter loopback mode.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_EnableSerdesTransmitLoopback(HANDLE h, BOOL mode);

/**
 * Sets the SERDES receiver loopback state.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True enables the SERDES receiver loopback mode.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_EnableSerdesReceiverLoopback(HANDLE h, BOOL mode);

/**
 * Sets the SERDES receiver power state.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] mode True enables the SERDES receiver power.
 *
 * \return RETURN_NORMAL on success, RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_PowerDownSerdesReceiver(HANDLE h, BOOL mode);

/**
 * Returns the SERDES lock status.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return SERDES lock status.
 */
VHS_LOCK_STATUS VHSDAC_GetSerdesLockStatus(HANDLE h);

/**
 * Queries whether the RapidCHANNEL is aligned.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return True if the RapidCHANNEL is aligned.
 */
BOOL VHSDAC_IsRapidChannelAligned(HANDLE h);

/**
 * Queries whether the RapidCHANNEL DCM is locked.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return True if the RapidCHANNEL DCM is locked.
 */
BOOL VHSDAC_IsRapidChannelDCMLocked(HANDLE h);

/**
 * Queries whether the VHS module is AC coupled.
 *
 * \param[in] h VHS-DAC handle.
 *
 * \return True if the VHS module is AC coupled.
 */
BOOL VHSDAC_IsVhsDaughterACCoupled(HANDLE h);

#else

#define VHSDAC_HostControlClock(h,mode)                         VHSADC_HostControlClock(h, mode)

#define VHSDAC_DivideDIFClock(h,mode)                   VHSADC_DivideDIFClock(h,mode)

#define VHSDAC_GetClockSource(h)                        VHSADC_GetClockSource(h)

#define VHSDAC_SetClockSource(h,clkSource)              VHSADC_SetClockSource(h,clkSource)

#define VHSDAC_GetSerdesLockStatus(h)                   VHSADC_GetSerdesLockStatus(h)

#define VHSDAC_HostControlSerdes(h, mode)               VHSADC_HostControlSerdes(h, mode)

#define VHSDAC_EnableSerdesTransmitter(h, mode)         VHSADC_EnableSerdesTransmitter(h, mode)

#define VHSDAC_EnableSerdesReceiver(h, mode)            VHSADC_EnableSerdesReceiver(h, mode)

#define VHSDAC_EnableSerdesTransmitLoopback(h, mode)    VHSADC_EnableSerdesTransmitLoopback(h, mode)

#define VHSDAC_EnableSerdesReceiverLoopback(h, mode)    VHSADC_EnableSerdesTransmitLoopback(h, mode)

#define VHSDAC_PowerDownSerdesReceiver(h, mode)         VHSADC_PowerDownSerdesReceiver(h, mode)

#define VHSDAC_IsVhsDaughterACCoupled(h)            VHSADC_IsVhsDaughterACCoupled(h)

#define VHSDAC_IsRapidChannelAligned(h)          VHSADC_IsRapidChannelAligned(h)

#define VHSDAC_IsRapidChannelDCMLocked(h)        VHSADC_IsRapidChannelDCMLocked(h)

#define VHSDAC_IsDIFClockDivided(h)             VHSADC_IsDIFClockDivided(h)

#define VHSDAC_IsDIFClockSysgenControlled(h)        VHSADC_IsDIFClockSysgenControlled(h)

#endif /* DOXYGEN_PROTOTYPE_DECLARATION */


/**
 * Set the NCO frequency in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ulNcoFreq NCO frequency value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetNcoFreq(HANDLE           h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       unsigned long    ulNcoFreq);

/**
 * Get the NCO frequency in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pulNcoFreq Pointer that will receive the NCO frequency value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetNcoFreq(HANDLE           h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       unsigned long    *pulNcoFreq);

/**
 * Set the NCO phase in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ulNcoPhase NCO phase value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetNcoPhase(HANDLE          h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       unsigned long    ulNcoPhase);

/**
 * Get the NCO phase in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pulNcoPhase Pointer that will receive the NCO phase value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetNcoPhase(HANDLE          h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       unsigned long    *pulNcoPhase);

/**
 * Set the DAC A offset in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] lDacAOffset DAC A offset value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetOffsetA(HANDLE           h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       long             lDacAOffset);

/**
 * Get the DAC A offset in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] plDacAOffset Pointer that will receive the DAC A value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetOffsetA(HANDLE           h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       long             *plDacAOffset);

/**
 * Set the DAC B offset in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] lDacBOffset DAC B offset value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetOffsetB(HANDLE           h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       long             lDacBOffset);

/**
 * Get the DAC B offset in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] plDacBOffset Pointer that will receive the DAC B value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetOffsetB(HANDLE           h,
                                       VHS_DAC5687_ID   dac5687Id,
                                       long             *plDacBOffset);

/**
 * Set the QMC Gain A in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ulQmcGainA QMC gain A value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetQmcGainA(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned long    ulQmcGainA);

/**
 * Get the QMC Gain A in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pulQmcGainA Pointer that will receive the QMC gain A value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetQmcGainA(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned long    *pulQmcGainA);

/**
 * Set the QMC Gain B in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ulQmcGainB QMC gain B value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetQmcGainB(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned long    ulQmcGainB);

/**
 * Get the QMC Gain B in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pulQmcGainB Pointer that will receive the QMC gain B value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetQmcGainB(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned long    *pulQmcGainB);

/**
 * Set the QMC phase in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ulQmcPhase QMC phase value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetQmcPhase(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned long    ulQmcPhase);

/**
 * Get the QMC phase in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pulQmcPhase Pointer that will receive the QMC phase value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetQmcPhase(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned long    *pulQmcPhase);

/**
 * Set the Gain A in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ucGainACoarse Coarse Gain A value.
 * \param[in] cGainAFine Fine Gain A value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetGainA(HANDLE           h,
                                     VHS_DAC5687_ID   dac5687Id,
                                     unsigned char    ucGainACoarse,
                                     char             cGainAFine);

/**
 * Get the Gain A in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pucGainACoarse Pointer that will receive the coarse gain A value.
 * \param[out] pcGainAFine Pointer that will receive the fine gain A value.

 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetGainA(HANDLE           h,
                                     VHS_DAC5687_ID   dac5687Id,
                                     unsigned char   *pucGainACoarse,
                                     char            *pcGainAFine);

/**
 * Set the Gain B in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ucGainBCoarse Coarse Gain B value.
 * \param[in] cGainBFine Fine Gain B value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetGainB(HANDLE           h,
                                     VHS_DAC5687_ID   dac5687Id,
                                     unsigned char    ucGainBCoarse,
                                     char             cGainBFine);

/**
 * Get the Gain B in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[out] pucGainBCoarse Pointer that will receive the coarse gain B value.
 * \param[out] pcGainBFine Pointer that will receive the fine gain B value.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetGainB(HANDLE           h,
                                     VHS_DAC5687_ID   dac5687Id,
                                     unsigned char   *pucGainBCoarse,
                                     char            *pcGainBFine);

/**
 * Set a register value in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ucRegAddr Address of the register in the DAC5687 chip (see TI Documentation on DAC5687).
 * \param[in] ucRegValue Value to write in the DAC5687 chip (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetRegister(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned char    ucRegAddr,
                                        unsigned char    ucRegValue);

/**
 * Get a register value in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] ucRegAddr Address of the register in the DAC5687 chip (see TI Documentation on DAC5687).
 * \param[out] pucRegValue Pointer to read the DAC5687 chip (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687GetRegister(HANDLE           h,
                                        VHS_DAC5687_ID   dac5687Id,
                                        unsigned char    ucRegAddr,
                                        unsigned char    *pucRegValue);

/**
 * Reset a DAC5687 chip settings.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687Reset(HANDLE           h,
                                  VHS_DAC5687_ID   dac5687Id);



/**
 * Configure the clock in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] pllDiv Configuration of the PLL divider (see TI Documentation on DAC5687).
 * \param[in] intrpMode Configuration of the FIR Interpolation (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetDACClockMode(HANDLE                  h,
                                            VHS_DAC5687_ID          dac5687Id,
                                            VHS_DAC5687_PLL_DIV     pllDiv,
                                            VHS_DAC5687_FIR_INTRPOL intrpMode);


/**
 * Configure FMix Mode in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] bNCOEnable Enable the NCO (see TI Documentation on DAC5687).
 * \param[in] bNCOGainEnable When set, the data output of the NCO is increased by 2x. (see TI Documentation on DAC5687).
 * \param[in] bInvSincEnable Enables the invsinc compensation filter. (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetDACFMixMode(HANDLE           h,
                                           VHS_DAC5687_ID   dac5687Id,
                                           BOOL             bNCOEnable,
                                           BOOL             bNCOGainEnable,
                                           BOOL             bInvSincEnable);

/**
 * Configure QMC in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] bQmcEnable Enable the Quadrature modulator gain and phase correction. (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetDacQMC(HANDLE            h,
                                      VHS_DAC5687_ID    dac5687Id,
                                      BOOL              bQmcEnable);

/**
 * Configure CMIX in a DAC5687 chip.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] eCMIXMode Controls f/2 or f/4 mixer modes for the coarse mixer block. (see TI Documentation on DAC5687).
 * \param[in] eFir1MA First FIR filter in high-pass mode when enable, low-pass mode when disable. (see TI Documentation on DAC5687).
 * \param[in] eFir1MB First FIR filter in high-pass mode when enable, low-pass mode when disable. (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */
RETURN_STATUS VHSDAC_DAC5687SetCMIX(HANDLE                      h,
                                    VHS_DAC5687_ID              dac5687Id,
                                    VHS_DAC5687_COAR_MIX_MODE   eCMIXMode,
                                    VHS_DAC5687_FIR1_MODE       eFir1MA,
                                    VHS_DAC5687_FIR1_MODE       eFir1MB);

/**
 * Initializes the Dual Dac for Single Side Band operation with NCO mode to enable frequency translation.
 * This function is at a higher level than the rest of the API, it processes the input for the user
 * to properly configure the DAC device.
 *
 * \param[in] h VHS-DAC handle.
 * \param[in] dac5687Id Id of the chip.
 * \param[in] udwTranslation Frequency translation in Hz.
 * \param[in] udwFs VHS data Sampling frequency in Hz.
 * \param[in] bNCOGainEnable When set, the data output of the NCO is increased by 2x. (see TI Documentation on DAC5687).
 * \param[in] bInvSincEnable Enables the invsinc compensation filter. (see TI Documentation on DAC5687).
 * \param[in] IntrpMode Configuration of the FIR Interpolation (see TI Documentation on DAC5687).
 *
 * \return RETURN_NORMAL on success, RETURN_INVALIDPARAM if a parameter is
 *         invalid or RETURN_ERROR otherwise.
 */

RETURN_STATUS VHSDAC_DAC5687SetDacSSB(HANDLE                    h,
                                      VHS_DAC5687_ID            dac5687Id,
                                      unsigned long             udwTranslation,
                                      unsigned long             udwFs,
                                      BOOL                      bNCOGainEnable,
                                      BOOL                      bInvSincEnable,
                                      VHS_DAC5687_FIR_INTRPOL   IntrpMode);

#ifdef __cplusplus
}
#endif

#endif /* _VHSDAC16CTRL_V4_H_ */

