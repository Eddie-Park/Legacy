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

#ifndef protocolEdit_h
#define protocolEdit_h

int  protocolEditInitialization(void);
void protocolEditTermination(void);
void prepareProtocolForDisplay  (void);
int  readProtocolFromTag(char *tag, struct protocol* prot);
int  readProtocol(char *protocolName, struct protocol* prot);

inline int  oldreadProtocol(int index, struct protocol* prot){return 0;}
inline int olddeleteProtocol(int index){return 0;}
inline int  oldrenameProtocol(int index, char *newProtocolName){return 0;} 

int  mergeProtocol(char *tag, struct taskList* tl);
inline int  oldmergeProtocol(int index, struct taskList* tl){return 0;}
int  writeProtocolToDisk(char* pathName, struct protocol* prot);

int  renameProtocol(char *tag, char *newProtocolName);
int  deleteProtocol(char *tag);
void editProtocol  (void);

#endif
