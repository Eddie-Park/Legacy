/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2013. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_QUITBUTTON                 2       /* control type: command, callback function: QuitCallback */
#define  PANEL_det2SinglesRate            3       /* control type: numeric, callback function: (none) */
#define  PANEL_det3SinglesRate            4       /* control type: numeric, callback function: (none) */
#define  PANEL_det1SinglesRate            5       /* control type: numeric, callback function: (none) */
#define  PANEL_det0SinglesRate            6       /* control type: numeric, callback function: (none) */
#define  PANEL_SavedBytes                 7       /* control type: numeric, callback function: (none) */
#define  PANEL_promptsRate                8       /* control type: numeric, callback function: (none) */
#define  PANEL_TIME_LEFT                  9       /* control type: numeric, callback function: (none) */
#define  PANEL_LOG_LED                    10      /* control type: LED, callback function: (none) */
#define  PANEL_CONNECTED_LED              11      /* control type: LED, callback function: (none) */
#define  PANEL_ACQ_LED                    12      /* control type: LED, callback function: (none) */
#define  PANEL_STUDY_ID                   13      /* control type: string, callback function: (none) */
#define  PANEL_PICTURE                    14      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_2                  15      /* control type: picture, callback function: (none) */
#define  PANEL_randomsRate                16      /* control type: numeric, callback function: (none) */
#define  PANEL_petBoxID                   17      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  18      /* control type: textMsg, callback function: (none) */
#define  PANEL_CMD_ACQ_HW_NOT_READY       19      /* control type: command, callback function: CB_SET_ACQ_HW_NOT_READY */
#define  PANEL_CMD_ACQ_HW_READY           20      /* control type: command, callback function: CB_SET_ACQ_HW_READY */
#define  PANEL_buildID                    21      /* control type: textMsg, callback function: (none) */
#define  PANEL_ACQ_HW_READY               22      /* control type: LED, callback function: (none) */
#define  PANEL_ACQ_TOGGLE_LABEL           23      /* control type: textMsg, callback function: (none) */

#define  PANEL_SER                        2
#define  PANEL_SER_COMMANDBUTTON          2       /* control type: command, callback function: OK_CB */
#define  PANEL_SER_serNum                 3       /* control type: numeric, callback function: (none) */
#define  PANEL_SER_PICTURE                4       /* control type: picture, callback function: (none) */
#define  PANEL_SER_PICTURE_2              5       /* control type: picture, callback function: (none) */
#define  PANEL_SER_TEXTMSG                6       /* control type: textMsg, callback function: (none) */
#define  PANEL_SER_PICTURE_4              7       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CB_SET_ACQ_HW_NOT_READY(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_SET_ACQ_HW_READY(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OK_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
