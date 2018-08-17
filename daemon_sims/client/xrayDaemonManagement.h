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

#ifndef xrayDaemonManagement_H
#define xrayDaemonManagement_H

#define XRAY_DAEMON_PORT_NUMBER 65534

extern int  xrayDaemonInitialization(void);
extern void xrayDaemonTermination(void);
extern int  xrayAcquisition     (struct taskList* tl); 

extern int  xrayDaemonTCPCallbackFunction (unsigned handle, int xType, int errCode, void *callbackData);

#endif
