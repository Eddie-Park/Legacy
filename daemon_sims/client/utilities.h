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

#ifndef utilities_H
#define utilities_H

#include "typedefs.h"

inline void myStrcpy(char* dest, char* source){
	while( (*dest++)=(*source++) );		// Expect warning for this (assignment in conditional expression)
}

inline void myMemcpy(char* target, char* source, int count){
	while (count--) (* (target)++) = (* (source)++) ;
}

void sprintfWithLock(unsigned __int64 bufLen, char * string, char *fmt, ...);

void shutdown(void);

char ** createDirList(char *startPath, int dir, int *size);
void freeDirList(char **nameArray, int size);
char * removeExtension(char *string);
void hmsToDuration(unsigned short int hour, unsigned short int min, unsigned short int sec, unsigned int *duration);
void durationToHms(unsigned short int* hour, unsigned short int* min, unsigned short int* sec, unsigned int duration);
int  isDirectoryEmpty(char *startPath);
int  emptyDirectory(char *startPath);
int  forceDeleteDir(char *dirName);

char* chomp(char* str);
int   makeProtocolPathName(char* tag, char *protPathName, char *extension);
int   makeStudyPathName(char * tag, char *studyPathName);
int   myGetTreeItemPathFromTag  (int panel, int control, char *tag, char *protPathName, int keepLowest);
int   myGetTreeItemPathFromIndex(int panel, int control, int index, char *protPathName, int keepLowest);

void fadeInPanel (struct fadingPanel *pan);
void fadeOutPanel(struct fadingPanel *pan);
void fadingPanelTermination(struct fadingPanel *pan);
int  fadingPanelInitialization(struct fadingPanel *pan, int numSteps, int panelID);
void putImageInTransparentButton(int panel, int control);

int  writeTaskList(struct taskList *tl);
int  readTaskList(char *taskListFileName, struct taskList *tl);
int  createTaskList(struct taskList** tl);
void printTaskList(struct taskList *tl);

int  readStudyData(char *fileName, struct studyData *sd);
int  writeStudyData(char *fileName, struct studyData *sd);

int determineTaskStatus(struct taskInfo *ti);

void updateTaskStatus(struct taskInfo *ti, int row, int col);


void setVector(struct vector *v, double x, double y);
struct vector subVector (struct vector a, struct vector b);
struct vector addVector (struct vector a, struct vector b);
double modulus(struct vector v);

void putToHex(char* out,void* in);

void LEDControlInitialization(struct LEDControl *meter, int panelID);
void LEDControlTermination(struct LEDControl *meter);
void resetLEDControl(struct LEDControl *meter);
void setLEDControlValue(struct LEDControl *meter, double value);

int isDirectory(char *pathName);
int isStudy(char *pathName);
int isYoungEnough(const char* fileName, int maxDays);

int fileExists(char *fileName);
void removeTrailingBlanks(char* s);

int copyFileWithProgress(char* inFilePath, char* outFilePath, struct waitPanelControlPair *wpcp);
void makeUniqueID(char* theID);
void localTimeWithLock( struct tm *tm, const time_t *dateTime);

#endif
