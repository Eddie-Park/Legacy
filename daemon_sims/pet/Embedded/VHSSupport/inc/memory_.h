/*
 Module:        memory_.h
 Creation date: 10-jun-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Debug layer of memory managment functions.
*/


#ifndef _MEMORY_
#define _MEMORY_

int Memory_Init( void);

#if DBG
#define MemoryAlloc( size) Debug_MemoryAlloc( size, _HERE_)
#define MemoryFree( p) Debug_MemoryFree( (void **)&(p))
#else
#include <stdlib.h>
#define MemoryAlloc( size) malloc( size)
#define MemoryFree( p) free( p)
#endif


#if DBG
void * Debug_MemoryAlloc( int size, char * who);
void Debug_MemoryFree( void ** p);
int Debug_MemoryGetTotalSpace();
void Debug_MemoryDump();
#endif


#endif // _MEMORY_
