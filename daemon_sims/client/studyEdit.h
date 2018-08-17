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

#ifndef studyEdit_H
#define studyEdit_H

void editStudy(void);
void  showStudy(void);

void studyDataToPanel(struct studyData *s);
void panelToStudyData(struct studyData *s);

int   studyEditInitialization(void);
void  studyEditTermination(void);

int  setSaveButtonStatus(void);

#endif
