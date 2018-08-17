/*
 Module:        putstr_.h
 Creation date: 15-dec-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Low level input/output print-formatted interface.
*/



#ifndef _PUTSTR__
#define _PUTSTR__

#define PRINTF Stack_printf
#define HPRINTF Stack_hprintf
#define BEGIN_MULTI_PRINTF() Stack_StringLock()
#define END_MULTI_PRINTF() Stack_StringUnlock()

#if DBG
#define DEB_NONE     0
#define DEB_ERROR    1
#define DEB_WARNING  2
#define DEB_INFO     3
#define DEB_TRACE    4

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
#define PRINTF_WRN if(1);else
#define PRINTF_INFO if(1);else
#define PRINTF_TRACE if(1);else
#define DBG_S(n) ""
#define _HERE_ 
#endif	// DBG


#if DBG
extern int debug_lev;
#endif

int Stack_SetPutStringFn( int (*vprintfP)( const char * s, va_list marker), int (*putLockP)(void), int (*putUnlockP)(void));

int Stack_printf( const char * s, ...);
int Stack_hprintf( void * handle, const char * s, ...);
int Stack_vprintf( const char * s, va_list marker);
int Stack_StringLock( void);
int Stack_StringUnlock( void);

#endif // _PUTSTR__
