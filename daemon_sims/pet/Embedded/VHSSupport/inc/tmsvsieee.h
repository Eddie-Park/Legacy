/************************************************************************/
/*                    Technologies Lyre Inc.                            */
/************************************************************************/
/*                                                                      */
/*  Project      : Signal Master TMS.                                   */
/*  Source      : TMSvsIEEE.h                                           */
/************************************************************************/
/*                                                                      */
/*  Revision   : 1.0                                                    */
/*  Date       : 18 march, 1999                                         */
/*  Author     : Martin Turgeon                                         */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#if ! defined ( _TMSvsIEEE_ )
#define _TMSvsIEEE_

#if defined(OS_WIN32) && defined(SMDLL_EXPORTS)
#define FUNC_DECL __declspec(dllexport)
#elif defined(OS_WIN32) && defined(SMDLL_IMPORTS)
#define FUNC_DECL __declspec(dllimport)
#else
#define FUNC_DECL
#endif
FUNC_DECL float convTMS_IEEE (float DATA);
FUNC_DECL float convIEEE_TMS (float DATA);

#endif

