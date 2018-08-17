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

#ifndef preferenceManagement_H
#define preferenceManagement_H

int preferenceManagementInitialization(void);
void savePreferences(void);
void setDefaultPreferences(void);
void CVICALLBACK displayPref_CB (int menuBar, int menuItem, void *callbackData, int panel);

bool checkForConversionKey(char *convKey);

#endif