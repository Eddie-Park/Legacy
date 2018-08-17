/*
 Module:        time.h
 Creation date: 16-jul-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Wraper layer for OS timing functions.
*/

#ifndef _TIME_
#define _TIME_


// timing functions:
#define OS_TIME LOWOS_TIME
#define OS_TIMEDELAY LOWOS_TIMEDELAY				// ms
#define OS_SEC2TDELAY( sec) LOWOS_SEC2TDELAY( sec)	// convert seconds to OS_TIMEDELAY
#define OS_MS2TDELAY( ms) LOWOS_MS2TDELAY(ms) // convert mili-seconds to OS_TIMEDELAY
#define OS_ACTUAL_TIME() LOWOS_ACTUAL_TIME()		// time, OS_TIME unit
#define OS_MAX_TIME() LOWOS_MAX_TIME()
#define OS_TIME2SEC( os_time) LOWOS_TIME2SEC( os_time)	// convert OS_TIME to seconds
#define OS_TIME2MS( os_time) LOWOS_TIME2MS( os_time)		// convert OS_TIME to ms
// convert OS_TIME to second fraction (1/2**32 sec. unit):
#define OS_GET_NTP( integP, fracP) LOWOS_GET_NTP( integP, fracP)

#define OS_SLEEP( timeDelay) LOWOS_SLEEP( timeDelay)	// sleep for a while, ms
#define OS_INFINITE LOWOS_INFINITE	// infinit delta time value for OS_WAIT_.. functions

#endif // _TIME_
