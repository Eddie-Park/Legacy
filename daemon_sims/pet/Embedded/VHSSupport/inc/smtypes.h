/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SignalMaster                                              *
 *  Processor   : Host (DOS, LINUX)                                         *
 *  File        : smtypes.h                                                 *
 *  Description : Data type definitions.                                    *
 *                                                                          *
 ****************************************************************************
 *
 * "$Revision: 1.1 $"
 * "$Name: 1.1 $"
 * "$Date: 2007/04/24 21:02:58 $"
 * "$Author: jf.robert $"
 *
 ****************************************************************************/

#ifndef _SMTYPES_H_
#define _SMTYPES_H_

#define TRUE    1
#define FALSE   0

#define far
#define _far

#ifndef XDEF
#define XDEF
typedef double QWORD;
typedef unsigned long	DWORD;
typedef unsigned short	WORD;
typedef unsigned char	BYTE;
#ifndef NULL
#define NULL	((void*) 0)
#endif
#endif

#endif /* _SMTYPES_H_ */
