/*
 Module:        os.h
 Creation date: 16-jul-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Debug layer for OS. Init functions.
*/


#ifndef _OS_
#define _OS_

#include "low_os.h"

#include "putstr_.h"
#include "time_.h"
#include "memory_.h"
#include "task_.h"
#include "mut_sem_.h"
#include "socket_.h"

int Os_Init();

#endif // _OS_
