/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SHARC-based SignalMaster                                  *
 *  Processor   : Host                                                      *
 *  File        : gatesBIOS.h                                               *
 *  Description : GatesMaster Device Driver                                 *
 *                                                                          *
 ****************************************************************************
 *
 * "$Name: 1.1 $"
 * "$Date: 2007/04/24 21:02:58 $"
 * "$Author: jf.robert $"
 *
 ****************************************************************************/
#ifndef __GATESBIOS_H__
#define __GATESBIOS_H__

#define XCV300	218980L
#define XCV400	318260L
#define XCV600	451000L
#define XCV800	589448L

#if defined(OS_WIN32) && defined(SMDLL_EXPORTS)
#define FUNC_DECL __declspec(dllexport)
#elif defined(OS_WIN32) && defined(SMDLL_IMPORTS)
#define FUNC_DECL __declspec(dllimport)
#else
#define FUNC_DECL
#endif
FUNC_DECL int	GatesMasterInit(smResource *p, DWORD *gmType);
FUNC_DECL int	GatesMasterBoot(smResource *p, DWORD gmType, int bank);	
FUNC_DECL int GatesMasterFlashErase(smResource *p, DWORD gmType, int bank);
FUNC_DECL int	GatesMasterFlashProgram(smResource *p, DWORD gmType, int bank, char *fileName);
FUNC_DECL int GatesMasterFlashVerify(smResource *p, DWORD gmType, int bank, char *fileName);

#endif // __GATESBIOS_H__


