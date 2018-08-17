//*******************************************************************************
//*                                                                             *
//*  PETBox Acquisition and Management Graphical User Interface                 *
//*                                                                             *
//*  Copyright - 2009                                                           *
//*                                                                             *
//*  This code is the property of the Regents of UCLA.                          *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Richard Taschereau                                              *
//*                                                                             *
//*  Created: September 2009                                                    *
//*                                                                             *
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************

#ifndef cameraManagement_H
#define cameraManagement_H

int  cameraInitialization(void);
int  cameraTermination(void);
int  readCameraSettings(void);
int  saveCameraSettings(void);
int  openView (fc2Context context, struct cameraSettings *view );
fc2Error  closeView(fc2Context context);
void CVICALLBACK camSettings_CB (int menuBar, int menuItem, void *callbackData, int panel);

#endif
