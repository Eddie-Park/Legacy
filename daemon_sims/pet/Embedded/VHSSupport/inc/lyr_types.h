/*============================================================================
==
==          **  **     **  ******  ********  ********  ********  **    **
==         **    **   **  **   ** ********  ********  ********  **    **
==        **     *****   **   **    **     **        **        **    **
==       **       **    ******     **     ****      **        ********
==      **       **    **  **     **     **        **        **    **
==     *******  **    **   **    **     ********  ********  **    **
==    *******  **    **    **   **     ********  ********  **    **
==
==             L Y R T E C H   S I G N A L   P R O C E S S I N G
==
==   Project     : ADACMaster III
==   Author(s)   : Christine Lapointe
==
==                      Copyright (c) Lyrtech inc. 2005
==
==============================================================================
==
==   MODULE NAME         : types.H
==   PUBLIC PREFIX       : NONE
==
==   COMPILER VERSION OS :
==   PROCESSOR           :
==
============================================================================*/
/*==========================================================================
==                          D E S C R I P T I O N
============================================================================
==
==  This modul contains typedef of word definition.
==
============================================================================*/
#ifndef __TYPES_H__
#define __TYPES_H__

/** \file types.h
    This file contains global definitions of the data type.
*/

/*==========================================================================
==  INCLUDES
==========================================================================*/
//#include "constant.h"
//#include "cfghdwr.h"

/*==========================================================================
==  GLOBAL CONSTANTES
==========================================================================*/

typedef char                  int8_t;       /* 8 bits signed      */
typedef unsigned char         uint8_t;      /* 8 bits not signed  */
typedef short                 int16_t;      /* 16 bits signed     */
typedef unsigned short        uint16_t;     /* 16 bits not signed */
typedef long                  int32_t;      /* 32 bits signed     */
typedef unsigned long         uint32_t;     /* 32 bits not signed */
typedef long long             int64_t;      /* 64 bits signed     */
typedef unsigned long long    uint64_t;     /* 64 bits unsigned   */

//Must be delete to support new standard type
#ifdef CFG_MICROBLAZE_COMPILATOR
typedef char                CHAR;    /* 8 bits signed      */
typedef unsigned char       BOOL;    /* 8 bits not signed  */
typedef unsigned char       BYTE;    /* 8 bits not signed  */
typedef short               WORD;    /* 16 bits signed     */
typedef unsigned short      UWORD;   /* 16 bits not signed */
typedef int                 DWORD;   /* 32 bits signed     */
typedef unsigned int        UDWORD;  /* 32 bits not signed */
typedef float               FLOAT;   /* 32 bits float      */
typedef double              DDWORD;  /* 64 bits signed     */
typedef unsigned long long  UDDWORD; /* 64 bits unsigned   */
#endif

//Add for new compatibility with new standard
#ifdef CFG_MICROBLAZE_COMPILATOR
typedef char                  Int8;     /* 8 bits signed      */
typedef unsigned char         Uint8;    /* 8 bits not signed  */
typedef short                 Int16;    /* 16 bits signed     */
typedef unsigned short        Uint16;   /* 16 bits not signed */
typedef int                   Int32;    /* 32 bits signed     */
typedef unsigned int          Uint32;   /* 32 bits not signed */
typedef long long             Int64;    /* 64 bits signed     */
typedef unsigned long long    Uint64;   /* 64 bits unsigned   */
#endif
//Add for new compatibility with new standard
#ifdef WIN32
#include <windows.h>

typedef char                  Int8;     /* 8 bits signed      */
typedef unsigned char         Uint8;    /* 8 bits not signed  */
typedef short                 Int16;    /* 16 bits signed     */
typedef unsigned short        Uint16;   /* 16 bits not signed */
typedef int                   Int32;    /* 32 bits signed     */
typedef unsigned int          Uint32;   /* 32 bits not signed */
typedef long long             Int64;    /* 64 bits signed     */
typedef unsigned long long    Uint64;   /* 64 bits unsigned   */
typedef unsigned char         Bool;     /* 8 bits not signed  */
typedef HANDLE                LYR_Handle;   /* Generic handle */
typedef unsigned int          size_t;       /* standard type of size, length and dimension */
#endif

/* TI Compilator :
    char    8 bits
    short   16 bits
    int     32 bits
    long    40 bits
    float   32 bits
    double  64 bits

    char            Bits 0-7 of register 8 bits aligned to 8-bit boundary
    unsigned char   Bits 0-7 of register 8 bits aligned to 8-bit boundary
    short           Bits 0-15 of register 16 bits aligned to 16-bit boundary
    unsigned short  Bits 0-15 of register 16 bits aligned to 16-bit boundary
    int             Entire register 32 bits aligned to 32-bit boundary
    unsigned int    Entire register 32 bits aligned to 32-bit boundary
    enum            Entire register 32 bits aligned to 32-bit boundary
    float           Entire register 32 bits aligned to 32-bit boundary
    long            Bits 0-39 of even/odd register pair 64 bits aligned to 64-bit boundary
    unsigned long   Bits 0-39 of even/odd register pair 64 bits aligned to 64-bit boundary
    long long           Even/odd register pair 64 bits aligned to 64-bit boundary
    unsigned long long  Even/odd register pair 64 bits aligned to 64-bit boundary
    double              Even/odd register pair 64 bits aligned to 64-bit boundary
    long double         Even/odd register pair 64 bits aligned to 64-bit boundary
*/
#ifdef CFG_TI_COMPILATOR
#include <tistdtypes.h>
typedef char                CHAR;    /* 8 bits signed      */
typedef unsigned char       BOOL;    /* 8 bits not signed  */
typedef unsigned char       BYTE;    /* 8 bits not signed  */
typedef short               WORD;    /* 16 bits signed     */
typedef unsigned short      UWORD;   /* 16 bits not signed */
typedef int                 DWORD;   /* 32 bits signed     */
typedef unsigned int        UDWORD;  /* 32 bits not signed */
typedef float               FLOAT;   /* 32 bits float      */
typedef double              DDWORD;  /* 64 bits signed     */
typedef unsigned long long  UDDWORD; /* 64 bits unsigned   */
#endif

#ifdef WIN32
#ifndef _WINDEF_
typedef char                CHAR;    /* 8 bits signed      */
typedef unsigned char       BOOL;    /* 8 bits not signed  */
typedef unsigned char       BYTE;    /* 8 bits not signed  */
typedef short               WORD;    /* 16 bits signed     */
typedef unsigned short      UWORD;   /* 16 bits not signed */
typedef int                 DWORD;   /* 32 bits signed     */
typedef unsigned int        UDWORD;  /* 32 bits not signed */
typedef float               FLOAT;   /* 32 bits float      */
typedef double              DDWORD;  /* 64 bits signed     */
typedef unsigned long long  UDDWORD; /* 64 bits unsigned   */
#endif
#endif
/*==========================================================================
==  GLOBAL STRUCTURES
==========================================================================*/

/*==========================================================================
==  GLOBAL VARIABLES
==========================================================================*/

/*==========================================================================
==  GLOBAL MACROS
==========================================================================*/

/*==========================================================================
==  PROTOTYPES OF GLOBAL FUNCTIONS
==========================================================================*/

#endif

/*:::::::::::::::::::::::::::: END OF FILE ::::::::::::::::::::::::::::*/
