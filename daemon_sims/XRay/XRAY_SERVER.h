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

#define  SERVEPANEL                       1       /* callback function: ServerPanelCallback */
#define  SERVEPANEL_SERVERLED             2       /* control type: LED, callback function: (none) */
#define  SERVEPANEL_PICTURE_3             3       /* control type: picture, callback function: (none) */
#define  SERVEPANEL_XRAY_ON               4       /* control type: LED, callback function: (none) */
#define  SERVEPANEL_PICTURE_2             5       /* control type: picture, callback function: (none) */
#define  SERVEPANEL_XRAY_CURRENT          6       /* control type: numeric, callback function: (none) */
#define  SERVEPANEL_XRAY_HV               7       /* control type: numeric, callback function: (none) */
#define  SERVEPANEL_buildID               8       /* control type: textMsg, callback function: (none) */
#define  SERVEPANEL_COMMANDBUTTON         9       /* control type: command, callback function: loopTest */
#define  SERVEPANEL_loopOpen              10      /* control type: LED, callback function: (none) */
#define  SERVEPANEL_loopClosed            11      /* control type: LED, callback function: (none) */
#define  SERVEPANEL_XRAY_SIDECAM_RDY_LED  12      /* control type: LED, callback function: (none) */
#define  SERVEPANEL_XRAY_DETECT_RDY_LED   13      /* control type: LED, callback function: (none) */
#define  SERVEPANEL_XRAY_SRC_RDY_LED      14      /* control type: LED, callback function: (none) */
#define  SERVEPANEL_SIDECAM_TOGGLE        15      /* control type: textButton, callback function: CB_toggleSideCamReady */
#define  SERVEPANEL_DETECTOR_TOGGLE       16      /* control type: textButton, callback function: CB_toggleDetectorReady */
#define  SERVEPANEL_SOURCE_TOGGLE         17      /* control type: textButton, callback function: CB_toggleSourceReady */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CB_toggleDetectorReady(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_toggleSideCamReady(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CB_toggleSourceReady(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK loopTest(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ServerPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
