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
 *
 ***************************************************************************
 *
 *  Project     : Error management
 *  Description : Error codes generation and analysis.
 *  File target : DSP, HOST, MCU
 *
 *  Author(s)   : Louis-Philippe Lessard & Pierre-Marc Jobin
 *
 *                Copyright (c) Lyrtech inc. 2008
 *
 ***************************************************************************
 *
 *  "$Revision: 1.3 $"
 *  "$Name: 1.3 $"
 *  "$Date: 2008/09/25 15:44:42 $"
 *  "$Author: simon.boucher $"
 *
 **************************************************************************/

#ifndef __LYR_ERROR_H_
#define __LYR_ERROR_H_

/************************************************************************//**
 * @file lyr_error.h
 ****************************************************************************
 *
 * This module manages the codes returned by functions. This code manager
 * is used by many drivers and software modules to ensure that returned
 * codes are uniform between drivers and modules. When errors occur, this
 * method allows users to determine what drivers or modules failed, and
 * the cause of the problem(s). It thus becomes easier and quicker to troubleshoot
 * applications.
 *
 ****************************************************************************/

/************************************************************************//**
 *
 * Return code management type definitions.
 *
 ****************************************************************************/
typedef int Result;

/************************************************************************//**
 *
 * Generates a function return code.
 *
 * @param [in]  severity
 *      Degree of severity
 *
 * @param [in]  module
 *      Current software module or driver
 *
 * @param [in]  code
 *      Code description number
 *
 * @return
 *      Function return code
 *
 ****************************************************************************/
#define MAKE_RESULT(severity, module, code) \
    ((Result)(((unsigned int)(severity) << 30) | ((unsigned int)(module) << 16) | ((unsigned int)(code))))

/************************************************************************//**
 *
 * Retrieves the severity from the return code.
 *
 * @param [in]  result
 *      Function return code
 *
 * @return
 *      Degree of severity
 *
 ****************************************************************************/
#define RESULT_SEVERITY(result) \
    (((unsigned int)(result) >> 30) & 0x3)

/************************************************************************//**
 *
 * Retrieves the module or driver affected by the return code.
 *
 * @param [in]  result
 *      Function return code
 *
 * @return
 *      Affected software module or driver.
 *
 ****************************************************************************/
#define RESULT_MODULE(result) \
    (((unsigned int)(result >> 16) & 0x3FFF)

/************************************************************************//**
 *
 * Retrieves the code description number of the return code.
 *
 * @param [in]  result
 *      Function return code
 *
 * @return
 *      Code description number
 *
 ****************************************************************************/
#define RESULT_CODE(result) \
    ((unsigned int)(result) & 0xFFFF)

/************************************************************************//**
 *
 * Verifies whether the return code signifies success.
 *
 * @param [in]  result
 *      Function return code
 *
 * @return
 *      Success (1) or failure (0)
 *
 ****************************************************************************/
#define SUCCESS(result) \
    ((result) >= 0)

/************************************************************************//**
 *
 * Verifies whether the return code signifies failure.
 *
 * @param [in]  result
 *      Function return code
 *
 * @return
 *      Failure (1) or success (0)
 *
 ****************************************************************************/
#define FAILURE(result) \
    ((result) < 0)

/************************************************************************//**
 *
 * Degrees of severity of a return code.
 *
 ****************************************************************************/
enum Lyr_Severity {
    LYR_SEVERITY_SUCCESS=0,             /**< MAKE_RESULT(LYR_SEVERITY_SUCCESS, MODULE_NULL, 0) = 0x00000000 */
    LYR_SEVERITY_INFORMATIONAL=1,       /**< MAKE_RESULT(LYR_SEVERITY_SUCCESS, MODULE_NULL, 0) = 0x40000000 (use to report calls to legacy functionalities) */
    LYR_SEVERITY_WARNING=2,             /**< MAKE_RESULT(LYR_SEVERITY_SUCCESS, MODULE_NULL, 0) = 0x80000000 */
    LYR_SEVERITY_ERROR=3                /**< MAKE_RESULT(LYR_SEVERITY_SUCCESS, MODULE_NULL, 0) = 0xC0000000 */
};

/************************************************************************//**
 *
 * List of the software modules and drivers using this return code management.
 *
 ****************************************************************************/
enum Module {
    /* broadly applicable module (NULL) */
    MODULE_NULL=0,          /**< Default software module */
    MODULE_I2C=1,           /**< I2C software module */
    MODULE_I2C_EEPROM=2,    /**< I2C EEPROM software module */
    MODULE_AD9511=3,        /**< AD9511 software module */

    /* Host */
    MODULE_SMQUADAPI=0x10,  /**< SignalMaster Quad host API software module */
    MODULE_RFFE     =0x11,  /**< RFFE's control API software module */
    MODULE_U2C      =0x12,  /**< USB to GPIO control API software module (refer to i2cbridge.h) */
    MODULE_ADACSYNC =0x13,  /**< ADACSync's control API software module */
    MODULE_INITLOADER=0x14, /**< Control API software module of the ADACSync's initialization loader */

    /* DSP internal modules */
    MODULE_DMA=0x20,        /**< DSP DMA software module */
    MODULE_IRQ=0x21,        /**< DSP IRQ software module */
    MODULE_SYS=0x22,        /**< DSP general platform configuration software module */

    /* Lyrtech drivers */
    MODULE_XEVT=0x30,       /**< DSP external event software module */
    MODULE_FBUS=0x31,       /**< DSP FastBus driver */
    MODULE_RAVE=0x32,       /**< DSP RAVe driver */
    MODULE_CAMLNK=0x33,     /**< DSP Camera Link driver */
    MODULE_DRC=0x34,        /**< DSP DRC Virtex-4 driver */
    MODULE_TTRFT=0x35,      /**< Twin Tunable RF Transceiver's control API */
    MODULE_TWRFT=0x36,      /**< Twin Wimax RF Transceiver's control API */

    /* Utility modules */
    MODULE_QUEUE=0x40,      /**< DSP queue management software module */
    MODULE_PINGPONG=0x41    /**< DSP ping-pong management software module */
};

/************************************************************************//**
 *
 * This is an example of an enumeration that specifies a module's specific error code.
 *
 * @attention
 *      The only usable return code of this enumeration is OK. Do NOT
 *      use any other. Other return codes are general, so they do not apply
 *      to any module. It is important to use module-specific return codes
 *      because they inform you of what modules failed when errors occur. Also,
 *      specify a code description number to each specific problem. This allows
 *      you to better understand the problem when it occurs.
 *
 ****************************************************************************/
enum Error {
    OK                  = MAKE_RESULT(LYR_SEVERITY_SUCCESS,       MODULE_NULL, 0),          /**< The method succeeded */
    ERR_GENERIC         = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 0xFFFF),     /**< The method failed */
    ERR_CRITICAL        = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 0xDEAD),     /**< Unrecoverable failure */
    ERR_OUTOFMEMORY     = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 1),          /**< The method failed to allocate necessary memory */
    ERR_PENDING         = MAKE_RESULT(LYR_SEVERITY_INFORMATIONAL, MODULE_NULL, 2),          /**< The necessary data is not yet available */
    ERR_ABORT           = MAKE_RESULT(LYR_SEVERITY_WARNING,       MODULE_NULL, 3),          /**< The ongoing operation was aborted */
    ERR_ACCESSDENIED    = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 4),          /**< General access denial */
    ERR_NOTIMPLEMENTED  = MAKE_RESULT(LYR_SEVERITY_INFORMATIONAL, MODULE_NULL, 5),          /**< The method is not implemented */
    ERR_INVARG          = MAKE_RESULT(LYR_SEVERITY_WARNING,       MODULE_NULL, 6),          /**< The provided argument is invalid */
    ERR_INVPTR          = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 7),          /**< The provided pointer is invalid */
    ERR_INVHANDLE       = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 8),          /**< The provided handle is invalid */
    ERR_TIMEOUT         = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 9),          /**< The time has elapsed */
    ERR_ALRDYALLOC      = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 10),         /**< Ressource already allocated */
    ERR_NOTINIT         = MAKE_RESULT(LYR_SEVERITY_ERROR,         MODULE_NULL, 11),         /**< The module was not initialized. Initialize it before calling this function */
    ERR_ALRDYDONE       = MAKE_RESULT(LYR_SEVERITY_WARNING,       MODULE_NULL, 12),         /**< General task that must be performed only once */
    ERR_NOHWMODULE      = MAKE_RESULT(LYR_SEVERITY_INFORMATIONAL, MODULE_NULL, 13)          /**< The targeted hardware module was not detected */
};

#endif /*__LYR_ERROR_H_*/
