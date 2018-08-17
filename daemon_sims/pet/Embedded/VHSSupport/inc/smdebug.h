/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SignalMaster                                              *
 *  File        : smdebug.h                                                 *
 *  Description : Debug macros.                                             *
 *                                                                          *
 ****************************************************************************
 *
 * "$Revision: 1.1 $"
 * "$Name: 1.1 $"
 * "$Date: 2007/04/24 21:02:58 $"
 * "$Author: jf.robert $"
 *
 ****************************************************************************/

#ifndef _SMDEBUG_H_
#define _SMDEBUG_H_

#include <stdio.h>

//#define DBG 1		// with DEBUG code
//#define DBG 0		// without DEBUG code
#ifndef DBG
#define DBG 0
#endif

#define DEB_NONE     0
#define DEB_ERROR    1
#define DEB_WARNING  2
#define DEB_INFO     3
#define DEB_TRACE    4

//#define PRINTF printf
//#define PRINTF mexPrintf

extern int (*sm_printf)( const char *, ... );
#define PRINTF (*sm_printf)

#if DBG == 1
#define PRINTF_ERR if( debug_lev < DEB_ERROR) ; else PRINTF
#define PRINTF_WRN if( debug_lev < DEB_WARNING) ; else PRINTF
#define PRINTF_INFO if( debug_lev < DEB_INFO) ; else PRINTF
#define PRINTF_TRACE if( debug_lev < DEB_TRACE) ; else PRINTF
#define DBG_S(n) n
#define _STR(n) #n
#define __STR(n) _STR(n)
#define _HERE_ "[" __FILE__ ":" __STR(__LINE__) "] "
#else
#define PRINTF_ERR
#define PRINTF_WRN
#define PRINTF_INFO
#define PRINTF_TRACE
#define DBG_S(n) ""
#define _HERE_ ""
#endif

extern int debug_lev; // = DEB_INFO;		// all!!

#endif /* _SMDEBUG_H_ */

