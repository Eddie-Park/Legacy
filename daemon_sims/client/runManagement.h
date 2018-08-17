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

#ifndef runManagement_H
#define runManagement_H

int runManagementInitialization(void);
void runManagementTermination(void);
int  CVICALLBACK protocolHandlerThread         (void *functionData);
int  CVICALLBACK protocolHandlerThreadNoConfirm(void *functionData);
void  runBatchProtocol (int studyIndex, int protocolIndex);

#endif

