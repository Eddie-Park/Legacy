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

#ifndef studyTreeManagement_H
#define studyTreeManagement_H 

int studyTreeManagementInitialization(void);
int  createStudyTree(void);
void studyTreeManagementTermination(void);
void setItemAttributes(int newItemIndex, int theColor, int theBGColor, int imageIndex);

int CVICALLBACK histBrowseNorm_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);

#endif
