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

#ifndef protocolTreeManagement_H
#define protocolTreeManagement_H 

int protocolTreeManagementInitialization(void);
void protocolTreeManagementTermination(void);
int insertChildInProtocolTree(char *name, char* tag, int where, int theColor, int theBGColor, int imageIndex);
inline int oldinsertChildInProtocolTree(char *name, int where, int theColor, int theBGColor, int imageIndex){return 0;}

#endif
