/*
 Module:        low_os.h
 Creation date: 16-jul-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   (WIN32) Operating System Wraper init functions.
*/

#ifndef _LOW_OS_
#define _LOW_OS_

// for windows-95/win-NT:

#ifndef _WINDOWS_
#include <windows.h>
#endif
#ifndef _WINSOCKAPI_
#include <winsock.h>
#endif
#include <stdarg.h>
#include <process.h>	// _beginthread, _endthread


// initialisation:
#define LOWOS_INIT() LowOs_Init()
extern int LowOs_Init();



// semaphore functions:
#define LOWOS_SEMAPHORE HANDLE	// semaphore descriptor
// int LOWOS_CREATE_SEMAPHORE(
//		int initCount,			// initial count, 0 = non-signaled
//		int maxCount,			// maximum count
//		LOWOS_SEMAPHORE * semP)	// ptr to received semaphore descriptor
// return 0 if OK.
#define LOWOS_CREATE_SEMAPHORE( initCount, maxCount, semP) \
	(NULL == (*semP = CreateSemaphore( NULL, initCount, maxCount, NULL)))
// LOWOS_DELETE_SEMAPHORE(
//		LOWOS_SEMAPHORE sem)		// semaphore descriptor
// return 0 f OK.
#define LOWOS_DELETE_SEMAPHORE( sem) (!CloseHandle( sem))
// int LOWOS_WAIT_SEMAPHORE(
//		LOWOS_SEMAPHORE sem,		// semaphore descriptor
//		LOWOS_TIMEDELAY delay)	// max delay to wait, 0 == return immediately,
									//   LOWOS_INFINITE == wait forever
// return 0 if OK (signaled) else error code.
#define LOWOS_WAIT_SEMAPHORE( sem, delay) (WAIT_OBJECT_0 != WaitForSingleObject( sem, delay))
// LOWOS_RELEASE_SEMAPHORE( 
//		LOWOS_SEMAPHORE sem		// semaphore descriptor
//		int inc)					// increment count
// return 0 if OK.
#define LOWOS_RELEASE_SEMAPHORE( handle, inc) (0 == ReleaseSemaphore( handle, inc, NULL))



// mutex functions:
#define LOWOS_MUTEX HANDLE			// mutex descriptor
// int LOWOS_CREATE_MUTES(
//		int owned				// 1 if mutex is initialy owned by creator
//		LOWOS_MUTEX mutex)		// mutex descriptor
// return 0 if OK.
#define LOWOS_CREATE_MUTEX( owned, mutexP) \
	(NULL == (*mutexP = CreateMutex(NULL, owned, NULL)))
// LOWOS_DELETE_MUTEX(
//		LOWOS_MUTEX mutex)		// mutex descriptor
// return 0 f OK.
#define LOWOS_DELETE_MUTEX( mutex) (!CloseHandle( mutex))
// int LOWOS_WAIT_MUTEX(
//		LOWOS_MUTEX mutex,		// mutex descriptor
//		LOWOS_TIMEDELAY delay)	// max delay to wait for ownership of mutex,
									//  0 == return immediately, LOWOS_INFINITE == wait forever
// return 0 if OK.
#define LOWOS_WAIT_MUTEX( mutex, delay) \
	(WAIT_OBJECT_0 != WaitForSingleObject( mutex, delay))
// LOWOS_RELEASE_MUTEX(
//		LOWOS_MUTEX mutex)		// mutex descriptor
// return 0 if OK.
#define LOWOS_RELEASE_MUTEX( mutex) (0 == ReleaseMutex( mutex))


// timing functions:
#define LOWOS_TIME unsigned long
#define LOWOS_TIMEDELAY unsigned long				// ms
#define LOWOS_SEC2TDELAY( sec) ((sec) * 1000L)	// convert seconds to LOWOS_TIMEDELAY
#define LOWOS_MS2TDELAY( ms) (ms)					// convert mili-seconds to LOWOS_TIMEDELAY
#define LOWOS_ACTUAL_TIME() GetTickCount()		// time, LOWOS_TIME unit
#define LOWOS_MAX_TIME() 0xffffffff					// max value returned by GetTickCount()
#define LOWOS_TIME2SEC( os_time) (os_time / 1000L)	// convert LOWOS_TIME to seconds
#define LOWOS_TIME2MS( os_time) (os_time)			// convert LOWOS_TIME to ms
#define LOWOS_GET_NTP( integP, fracP) LowOs_GetNtp( integP, fracP)

#define LOWOS_SLEEP( timeDelay) Sleep( timeDelay)	// sleep for a while, ms
#define LOWOS_INFINITE INFINITE	// infinit delta time value for OS_WAIT_.. functions
extern int LowOs_GetNtp( unsigned long * integP, unsigned long * fracP);

#define USE_BEGIN_THREAD 1	// set to use stdlib task wrapper
// tasks:
typedef unsigned long LOWOS_TASK_ID;
typedef struct {
#if USE_BEGIN_THREAD
	unsigned long hndl;	// for_beginthread (process.h)
#else
	HANDLE hndl;
#endif
	LOWOS_TASK_ID taskId;
	} LOWOS_TASK;


#define LOWOS_TASK_DECL( name) LOWOS_TASK_DEF( name)	// forward declaration
#if USE_BEGIN_THREAD
//	#define LOWOS_TASK_DEF( name) void __cdecl name( void * _paramsP)	// for _beginthread
	#define LOWOS_TASK_DEF( name) unsigned __stdcall name( void * _paramsP)
	#define _RET_VALUE 0
#else
	#define LOWOS_TASK_DEF( name) DWORD WINAPI name( LPVOID _paramsP)
	#define _RET_VALUE 0
#endif
// LOWOS_NEWTASK( TASK_DEF taskName, LOWOS_TASK * taskHandle)
#define LOWOS_NEWTASK( taskName, taskP) LowOs_NewTask( taskName, taskP)
#define LOWOS_ENDOFTASK( value) { LowOs_EndOfTask( value, /* next parameter is taked from lowos_task_def():*/_paramsP); return _RET_VALUE;} 	// { ExitThread( (DWORD)value); return 0;}
// LOWOS_KILLTASK( OS_TASK task):
#define LOWOS_KILLTASK( task) (0 == TerminateThread( (HANDLE)task.hndl, 0)) // == 0 if ok!
#define LOWOS_GET_TASK_ID() GetCurrentThreadId()
#define LOWOS_TASK2ID( task) task.taskId
extern int LowOs_NewTask( LOWOS_TASK_DECL( name), LOWOS_TASK * taskP);
extern int LowOs_EndOfTask( DWORD value, HANDLE * taskDataP);


// misc:
#define LOWOS_GETLASTERRORSTRING( buff, maxLen)	\
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, \
							NULL, \
							GetLastError(), \
							MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), \
							(LPTSTR) buff, \
							maxLen, \
							NULL)


#endif // _LOW_OS_
