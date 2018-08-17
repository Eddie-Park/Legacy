/*
 Module:        mut_sem.h
 Creation date: 16-jul-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Debug layer for OS Mutex/Semaphore.
*/


#ifndef _MUT_SEM_
#define _MUT_SEM_

int MutSem_Init();

// semaphore functions:
#define OS_SEMAPHORE LOWOS_SEMAPHORE
#define OS_WAIT_SEMAPHORE( sem, delay) LOWOS_WAIT_SEMAPHORE( sem, delay)
#define OS_RELEASE_SEMAPHORE( handle, inc) LOWOS_RELEASE_SEMAPHORE( handle, inc)
#if DBG
#define OS_CREATE_SEMAPHORE( initCount, maxCount, semP) Debug_Semaphore_Create( initCount, maxCount, semP)
#define OS_DELETE_SEMAPHORE( sem) Debug_Semaphore_Delete( sem)
#else
#define OS_CREATE_SEMAPHORE( initCount, maxCount, semP) LOWOS_CREATE_SEMAPHORE( initCount, maxCount, semP)
#define OS_DELETE_SEMAPHORE( sem) LOWOS_DELETE_SEMAPHORE( sem)
#endif

// mutex functions:
#define OS_MUTEX LOWOS_MUTEX
#define OS_WAIT_MUTEX( mutex, delay) LOWOS_WAIT_MUTEX( mutex, delay)
#define OS_RELEASE_MUTEX( mutex) LOWOS_RELEASE_MUTEX( mutex)
#if DBG
#define OS_CREATE_MUTEX( owned, mutexP) Debug_Mutex_Create( owned, mutexP)
#define OS_DELETE_MUTEX( mutex) Debug_Mutex_Delete( mutex)
#else
#define OS_CREATE_MUTEX( owned, mutexP) LOWOS_CREATE_MUTEX( owned, mutexP)
#define OS_DELETE_MUTEX( mutex) LOWOS_DELETE_MUTEX( mutex)
#endif


#if DBG
int Debug_Semaphore_SetMaxCount( int count);
int Debug_Mutex_SetMaxCount( int count);
int Debug_Semaphore_Create( int initCount, int maxCount, OS_SEMAPHORE * semP);
int Debug_Semaphore_Delete( OS_SEMAPHORE sem);
int Debug_Mutex_Create( int owned, OS_MUTEX * mutexP);
int Debug_Mutex_Delete( OS_MUTEX mutex);
int Debug_Semaphore_GetCount( void);
int Debug_Mutex_GetCount( void);
#endif


#endif // _MUT_SEM_
