/*
 Module:        task_.h
 Creation date: 10-jun-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Debug layer for OS task functions.
*/

#ifndef _TASK_
#define _TASK_

int Task_Init();
#define OS_TASK LOWOS_TASK
#define OS_TASK_ID LOWOS_TASK_ID
#define OS_TASK_DECL( name) LOWOS_TASK_DECL( name)	// forward declaration
#define OS_TASK_DEF( name) LOWOS_TASK_DEF( name)
#define OS_GETTASKPARAMS() Task_GetParams()
#define OS_GET_TASK_ID() LOWOS_GET_TASK_ID()
#define OS_TASK2ID( task) LOWOS_TASK2ID( task)
//LSP #if DBG
#if 0
#define OS_NEWTASK( taskFunctionP, params, taskP) Debug_Task_NewTask( taskFunctionP, params, taskP, __STR(taskFunctionP))
#define OS_ENDOFTASK( value) { Debug_Task_EndOfTask(); LOWOS_ENDOFTASK( value); }
#else
#define OS_NEWTASK( taskFunctionP, params, taskP) Task_NewTask( taskFunctionP, params, taskP)
#define OS_ENDOFTASK( value) LOWOS_ENDOFTASK( value)
#endif



//LSP #if DBG
#if 0
char * Debug_GetTaskName( OS_TASK_ID taskId, char * buff, int maxLen);
int Debug_Task_DisplayList();
int Debug_Task_SetMaxCount( int count);
int Debug_Task_GetCount();
int Debug_Task_Kill( OS_TASK_ID taskId);
int Debug_Task_NewTask( OS_TASK_DECL( task), void * params, OS_TASK * taskP, char * fnName);
int Debug_Task_EndOfTask();
#else
int Task_NewTask( LOWOS_TASK_DECL( task), void * params, LOWOS_TASK * taskP);
#endif
void * Task_GetParams( void);


#endif // _TASK_

