/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2012. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  EXPORT_PNL                       1
#define  EXPORT_PNL_BROWSE                2       /* control type: pictButton, callback function: exportBrowse_CB */
#define  EXPORT_PNL_LMF                   3       /* control type: radioButton, callback function: (none) */
#define  EXPORT_PNL_HIST                  4       /* control type: radioButton, callback function: (none) */
#define  EXPORT_PNL_PHOTOS                5       /* control type: radioButton, callback function: (none) */
#define  EXPORT_PNL_DICOM                 6       /* control type: radioButton, callback function: (none) */
#define  EXPORT_PNL_SAVE_KEY              7       /* control type: pictButton, callback function: exportOK_CB */
#define  EXPORT_PNL_COMMANDBUTTON_5       8       /* control type: pictButton, callback function: exportCancel_CB */
#define  EXPORT_PNL_DEST                  9       /* control type: string, callback function: (none) */
#define  EXPORT_PNL_WAIT_CTRL             10      /* control type: pictRing, callback function: (none) */
#define  EXPORT_PNL_DECORATION            11      /* control type: deco, callback function: (none) */
#define  EXPORT_PNL_TEXTMSG               12      /* control type: textMsg, callback function: (none) */
#define  EXPORT_PNL_PICTURE               13      /* control type: picture, callback function: (none) */

#define  ISOT_PANEL                       2
#define  ISOT_PANEL_button                2       /* control type: pictButton, callback function: isotopeSave_CB */
#define  ISOT_PANEL_TABLE                 3       /* control type: table, callback function: (none) */
#define  ISOT_PANEL_PICTURE               4       /* control type: picture, callback function: (none) */

#define  MAIN_PANEL                       3
#define  MAIN_PANEL_COMMANDBUTTON_2       2       /* control type: command, callback function: test_CB */
#define  MAIN_PANEL_COMMANDBUTTON         3       /* control type: command, callback function: quitButton_CB */
#define  MAIN_PANEL_mainTAB               4       /* control type: tab, callback function: (none) */
#define  MAIN_PANEL_sideTAB               5       /* control type: tab, callback function: (none) */
#define  MAIN_PANEL_PICTURE               6       /* control type: picture, callback function: (none) */
#define  MAIN_PANEL_scriptBanner          7       /* control type: textMsg, callback function: (none) */

#define  PEEK_PANEL                       4
#define  PEEK_PANEL_STUDY_DATA            2       /* control type: textMsg, callback function: (none) */

#define  SPLASH_PNL                       5
#define  SPLASH_PNL_TEXTMSG_2             2       /* control type: textMsg, callback function: (none) */
#define  SPLASH_PNL_PICTURE               3       /* control type: picture, callback function: (none) */
#define  SPLASH_PNL_versionID             4       /* control type: textMsg, callback function: (none) */
#define  SPLASH_PNL_MAIN_BANNER_3         5       /* control type: canvas, callback function: (none) */
#define  SPLASH_PNL_MAIN_BANNER_2         6       /* control type: canvas, callback function: (none) */
#define  SPLASH_PNL_WAIT_CTRL             7       /* control type: pictRing, callback function: (none) */

     /* tab page panel controls */
#define  ABOUT_PNL_TEXTMSG                2       /* control type: textMsg, callback function: closeAbout_CB */
#define  ABOUT_PNL_PICTURE_2              3       /* control type: picture, callback function: sideTAB_CB */

     /* tab page panel controls */
#define  ACQ_PANEL_hour                   2       /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_min                    3       /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_sec                    4       /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_rate_0                 5       /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_1                 6       /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_2                 7       /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_3                 8       /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_4                 9       /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_5                 10      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_6                 11      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_7                 12      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_8                 13      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_9                 14      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_PICTURE                15      /* control type: picture, callback function: (none) */
#define  ACQ_PANEL_rate_10                16      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_11                17      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_12                18      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_13                19      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_14                20      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_15                21      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_16                22      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_17                23      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_18                24      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_19                25      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_20                26      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_21                27      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_22                28      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_rate_23                29      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_WAIT_CTRL              30      /* control type: pictRing, callback function: (none) */
#define  ACQ_PANEL_DUMMY_BUTTON           31      /* control type: pictButton, callback function: (none) */
#define  ACQ_PANEL_STOP_BUTTON            32      /* control type: pictButton, callback function: stopAcqNow_CB */
#define  ACQ_PANEL_breathingHistory       33      /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_sep1                   34      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_CANVAS                 35      /* control type: canvas, callback function: (none) */
#define  ACQ_PANEL_sep2                   36      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_freqTitle_4            37      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_freqTitle_5            38      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_freqTitle_3            39      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_protocolName           40      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG_16             41      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG                42      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_studyName              43      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_timeTitle              44      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_hourLabel              45      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG_2              46      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_minLabel               47      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG_17             48      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG_18             49      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG_19             50      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_secLabel               51      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_TEXTMSG_20             52      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_PROGRESS_NULL          53      /* control type: deco, callback function: (none) */
#define  ACQ_PANEL_PROGRESS_BAR           54      /* control type: deco, callback function: (none) */
#define  ACQ_PANEL_breathFreq             55      /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  CAM_PANEL_DONE_BUTTON            2       /* control type: pictButton, callback function: doneCamSettings_CB */
#define  CAM_PANEL_BRIGHTNESS             3       /* control type: scale, callback function: brightness_CB */
#define  CAM_PANEL_GAIN                   4       /* control type: scale, callback function: gain_CB */
#define  CAM_PANEL_SHUTTER                5       /* control type: scale, callback function: shutter_CB */
#define  CAM_PANEL_AUTO_GAIN              6       /* control type: radioButton, callback function: autoGain_CB */
#define  CAM_PANEL_AUTO_SHUT              7       /* control type: radioButton, callback function: autoShutter_CB */
#define  CAM_PANEL_AUTO_BRIGHT            8       /* control type: radioButton, callback function: autoBright_CB */
#define  CAM_PANEL_CANVAS                 9       /* control type: canvas, callback function: (none) */
#define  CAM_PANEL_TEXTMSG_4              10      /* control type: textMsg, callback function: (none) */
#define  CAM_PANEL_PICTURE                11      /* control type: picture, callback function: (none) */

     /* tab page panel controls */
#define  FADE_PANEL_PICTURE_2             2       /* control type: picture, callback function: (none) */
#define  FADE_PANEL_PICTURE               3       /* control type: picture, callback function: (none) */

     /* tab page panel controls */
#define  IDLE_PANEL_PETBox_LOGO           2       /* control type: picture, callback function: about_CB */
#define  IDLE_PANEL_PICTURE_2             3       /* control type: picture, callback function: sideTAB_CB */
#define  IDLE_PANEL_READY                 4       /* control type: command, callback function: (none) */
#define  IDLE_PANEL_NOT_CONNECTED         5       /* control type: command, callback function: reconnectClick_CB */
#define  IDLE_PANEL_reconTimer            6       /* control type: timer, callback function: reconTimer_CB */
#define  IDLE_PANEL_build                 7       /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  PROT_EDIT_protName               2       /* control type: string, callback function: protName_CB */
#define  PROT_EDIT_acq_CHK                3       /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_hour                   4       /* control type: numeric, callback function: (none) */
#define  PROT_EDIT_min                    5       /* control type: numeric, callback function: (none) */
#define  PROT_EDIT_sec                    6       /* control type: numeric, callback function: (none) */
#define  PROT_EDIT_isotope_RING           7       /* control type: ring, callback function: isotope_CB */
#define  PROT_EDIT_breathMon_CHK          8       /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_xray_CHK               9       /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_hist_CHK               10      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_histFrameSeq           11      /* control type: string, callback function: frameSeq_CB */
#define  PROT_EDIT_ATTNCORR_OPT           12      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_NORM_OPT               13      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_DTIMCORR_OPT           14      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_RNDCORR_OPT            15      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_DCYCORR_OPT            16      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_attnCorrFileRing       17      /* control type: ring, callback function: (none) */
#define  PROT_EDIT_normFileRing           18      /* control type: ring, callback function: (none) */
#define  PROT_EDIT_recon_CHK              19      /* control type: radioButton, callback function: (none) */
#define  PROT_EDIT_calibConstant          20      /* control type: numeric, callback function: (none) */
#define  PROT_EDIT_iterations             21      /* control type: numeric, callback function: (none) */
#define  PROT_EDIT_SAVE_BUTTON            22      /* control type: pictButton, callback function: protSave_CB */
#define  PROT_EDIT_MORELESS               23      /* control type: pictButton, callback function: (none) */
#define  PROT_EDIT_CANCEL_BUTTON          24      /* control type: pictButton, callback function: protDiscard_CB */
#define  PROT_EDIT_ACQTIME_SLIDER         25      /* control type: scale, callback function: slider_CB */
#define  PROT_EDIT_PICTURE                26      /* control type: picture, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_5              27      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_4              28      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG                29      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_2              30      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_6              31      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_7              32      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_8              33      /* control type: textMsg, callback function: (none) */
#define  PROT_EDIT_TEXTMSG_9              34      /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  STUDY_EDIT_NAME                  2       /* control type: string, callback function: validWindowsFileName_CB */
#define  STUDY_EDIT_INVESTIGATOR          3       /* control type: string, callback function: checkCompleteness_CB */
#define  STUDY_EDIT_SUBJECT_ID            4       /* control type: string, callback function: checkCompleteness_CB */
#define  STUDY_EDIT_COMPOUND              5       /* control type: string, callback function: checkCompleteness_CB */
#define  STUDY_EDIT_ASSAY_ACTIVITY        6       /* control type: numeric, callback function: (none) */
#define  STUDY_EDIT_adminTimeValue        7       /* control type: numeric, callback function: (none) */
#define  STUDY_EDIT_adminTimeMonth        8       /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_adminTimeDay          9       /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_adminTimeHour         10      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_adminTimeMin          11      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_DECORATION_10         12      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_DECORATION_11         13      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_DECORATION_12         14      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_DECORATION_13         15      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_DECORATION_14         16      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_assayTimeValue        17      /* control type: numeric, callback function: (none) */
#define  STUDY_EDIT_adminTimeYear         18      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_TEXTMSG_3             19      /* control type: textMsg, callback function: (none) */
#define  STUDY_EDIT_TEXTMSG               20      /* control type: textMsg, callback function: (none) */
#define  STUDY_EDIT_ACT_UNITS             21      /* control type: textMsg, callback function: (none) */
#define  STUDY_EDIT_DECORATION_9          22      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_DECORATION_8          23      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_monthsRing            24      /* control type: pictRing, callback function: ring_CB */
#define  STUDY_EDIT_yearRing              25      /* control type: pictRing, callback function: ring_CB */
#define  STUDY_EDIT_dayRing               26      /* control type: pictRing, callback function: ring_CB */
#define  STUDY_EDIT_DECORATION_7          27      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_hourRing              28      /* control type: pictRing, callback function: ring_CB */
#define  STUDY_EDIT_minRing               29      /* control type: pictRing, callback function: ring_CB */
#define  STUDY_EDIT_DECORATION_6          30      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_DECORATION_5          31      /* control type: deco, callback function: (none) */
#define  STUDY_EDIT_SAVE_BUTTON           32      /* control type: pictButton, callback function: studyDataSave_CB */
#define  STUDY_EDIT_TAG                   33      /* control type: string, callback function: (none) */
#define  STUDY_EDIT_assayTimeMonth        34      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_assayTimeDay          35      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_assayTimeHour         36      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_assayTimeYear         37      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_assayTimeMin          38      /* control type: textMsg, callback function: timeField_CB */
#define  STUDY_EDIT_CANVAS_2              39      /* control type: canvas, callback function: (none) */
#define  STUDY_EDIT_CANVAS                40      /* control type: canvas, callback function: (none) */
#define  STUDY_EDIT_CANCEL_BUTTON         41      /* control type: pictButton, callback function: studyDataCancel_CB */
#define  STUDY_EDIT_PICTURE               42      /* control type: picture, callback function: (none) */
#define  STUDY_EDIT_saveMask              43      /* control type: picture, callback function: (none) */

     /* tab page panel controls */
#define  TAB_PET_PROCEED_BUTTON           2       /* control type: pictButton, callback function: confirm_ACQ_CB */
#define  TAB_PET_CANCEL_BUTTON            3       /* control type: pictButton, callback function: cancel_ACQ_CB */
#define  TAB_PET_freqTitle_5              4       /* control type: textMsg, callback function: (none) */
#define  TAB_PET_protocolName             5       /* control type: textMsg, callback function: (none) */
#define  TAB_PET_studyName                6       /* control type: textMsg, callback function: (none) */
#define  TAB_PET_PICTURE                  7       /* control type: picture, callback function: (none) */

     /* tab page panel controls */
#define  TAB_PROT_TREE                    2       /* control type: tree, callback function: protTree_CB */
#define  TAB_PROT_CANVAS                  3       /* control type: canvas, callback function: (none) */

     /* tab page panel controls */
#define  TAB_STUDY_TREE                   2       /* control type: tree, callback function: studyTree_CB */
#define  TAB_STUDY_CANVAS                 3       /* control type: canvas, callback function: (none) */
#define  TAB_STUDY_TEXTMSG                4       /* control type: textMsg, callback function: (none) */
#define  TAB_STUDY_TEXTMSG_4              5       /* control type: textMsg, callback function: (none) */
#define  TAB_STUDY_TEXTMSG_2              6       /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  TAB_XRAY_OK_BUTTON               2       /* control type: command, callback function: xrayGO_CB */
#define  TAB_XRAY_CANCEL_BUTTON           3       /* control type: pictButton, callback function: cancel_XRAY_CB */
#define  TAB_XRAY_TIMER                   4       /* control type: timer, callback function: xrayRingTimer_CB */
#define  TAB_XRAY_protocolName            5       /* control type: textMsg, callback function: (none) */
#define  TAB_XRAY_studyName               6       /* control type: textMsg, callback function: (none) */
#define  TAB_XRAY_PICTURERING             7       /* control type: pictRing, callback function: (none) */
#define  TAB_XRAY_TEXTMSG                 8       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK about_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK autoBright_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK autoGain_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK autoShutter_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK brightness_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cancel_ACQ_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cancel_XRAY_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK checkCompleteness_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK closeAbout_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK confirm_ACQ_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK doneCamSettings_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exportBrowse_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exportCancel_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exportOK_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK frameSeq_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK gain_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK isotope_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK isotopeSave_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK protDiscard_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK protName_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK protSave_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK protTree_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quitButton_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK reconnectClick_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK reconTimer_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ring_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK shutter_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK sideTAB_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK slider_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopAcqNow_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK studyDataCancel_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK studyDataSave_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK studyTree_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK test_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timeField_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK validWindowsFileName_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK xrayGO_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK xrayRingTimer_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
