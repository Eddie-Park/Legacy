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

//                                     I M P O R T A N T 

// THE PROTOCOL PANEL IN BOTH UI FILES (physics mode and user mode) MUST CONTAIN THE SAME
// ELEMENTS IN THE SAME TAB ORDER


#include "windows.h"
#include "radioGroup.h"
#include "toolbox.h"
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <userint.h>
#include "constants.h"
#include "typedefs.h"

#include "menuManagement.h"
#include "protocolTreeManagement.h"
#include "protocolEdit.h"
#include "globalDecls.h"
#include "genisys4GUI.h"    
#include "utilities.h"
#include "logManagement.h"
#include "frameSeqStateMac.h"

static char tempString [2048];

static void populateNormRing(void);
static void populateAttenuationRing(void);

static void selectStringInRing(int panelHandle, int controlID, char* string);

static int getIndexFromLabel(char *isotopeName);

void CVICALLBACK insertIsotope (int panelHandle, int controlID, int MenuItemID, void *callbackData);
void CVICALLBACK deleteIsotope (int panelHandle, int controlID, int MenuItemID, void *callbackData);


static void getHistOptions(int panel, unsigned char* histOptions, int control, unsigned char bitMask){
	int optValue;	
	
	GetCtrlVal(panel, control, &optValue);
	if (optValue) (*histOptions) |=  bitMask;   // set
	else          (*histOptions) &= ~bitMask;   // reset
}

static inline void setDefaultHistOption(int control, unsigned char bitMask){
	getHistOptions(protocolEditPanel, &defaultHistOptions, control, bitMask);
}

int protocolEditInitialization(void){
	
	if (0>NewCtrlMenuItem(isotopePanel, ISOT_PANEL_TABLE, "Insert ...", VAL_GOTO, insertIsotope, NULL)){
		writeToLog("protocolEditInitialization - Error Creating menu item");
		return 8;
	}
	if (0>NewCtrlMenuItem(isotopePanel, ISOT_PANEL_TABLE, "Delete", VAL_GOTO, deleteIsotope, NULL)){
		writeToLog("protocolEditInitialization - Error Creating menu item");
		return 8;
	}
	if (0>NewCtrlMenuSeparator(isotopePanel, ISOT_PANEL_TABLE, VAL_GOTO)){
		writeToLog("protocolEditInitialization - Error Creating menu item");
		return 8;
	}
	
	// Set default values from the edit panel (except for calibration constant, which is read from registry)
	defaultHistOptions = 0;
	setDefaultHistOption(PROT_EDIT_ATTNCORR_OPT,  BIT_ATTNCORR); 
	setDefaultHistOption(PROT_EDIT_RNDCORR_OPT,   BIT_RNDCORR); 
	setDefaultHistOption(PROT_EDIT_NORM_OPT,      BIT_NORM); 
	setDefaultHistOption(PROT_EDIT_DTIMCORR_OPT,  BIT_DTIMCORR); 
	setDefaultHistOption(PROT_EDIT_DCYCORR_OPT,   BIT_DCYCORR); 
	
	GetCtrlVal(protocolEditPanel, PROT_EDIT_iterations, &defaultIterations);

	GetCtrlVal(protocolEditPanel, PROT_EDIT_breathMon_CHK,     &defaultBreathingMonitoring);
	GetCtrlVal(protocolEditPanel, PROT_EDIT_xray_CHK,          &defaultXray);

	
	return 0;
}

void protocolEditTermination(void){
	// Nothing to do
}


int readProtocolFromTag(char *tag, struct protocol *prot){
	char protPathName[MAX_FILE_NAME_LENGTH];
	makeProtocolPathName(tag, protPathName, ".ptc");
	return readProtocol(protPathName, prot);
}


// ----------------------------------- Read protocol  ---------------------------------
// Read a protocol file (.ptc) into a protocol structure
int readProtocol(char *protocolName, struct protocol* prot){
	
	FILE  *protFile=0;
	size_t objectsRead;
	int retCode = 0;
	protFile = fopen(protocolName,"rb");
	
	if (protFile){
		objectsRead = fread(prot, sizeof(struct protocol),  1, protFile); 
		if (objectsRead != 1 || prot->version != CURRENT_PROTOCOL_VERSION) {
			writeToLog("protocolEdit::readProtocol - Error reading protocol or wrong version");
			retCode=8;
		}
		fclose(protFile);
	}
	else {
		writeToLog("protocolEdit::readProtocol - ERROR, could not open protocol file for input");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		retCode=8;
	}
	
	return retCode;
}

// ----------------------------------- Merge protocol  ---------------------------------

// Called when running a protocol on a study in which a protocol has already been run
// We have to merge protocol options with the existing tasklist
// New protocol tasks have precedence over existing ones

// retCodes  0: OK
//           4: duplicate acquisition; not allowed
//           8: error reading the protocol 

int mergeProtocol(char *tag, struct taskList *tl){
	
	// Read new protocol in temporary struct
	struct protocol localProtocol;
	int readRetCode = readProtocolFromTag(tag, &localProtocol);
	
	if (readRetCode != 0 ) return 8;
	
	// This is the current task list, old protocol to update
	struct protocol * prot = &tl->prot; // a shorthand
	myStrcpy(prot->name,   localProtocol.name);

	if (localProtocol.acq){
		
		// Do not allow multiple acquisitions in the same study
		if (prot->acq){
			writeToLog("protocolEdit::mergeProtocol - Multiple acquisition detected");
			MessagePopup("ACQUISITION NOT ALLOWED", "There is already data in that study!");
			return 4;
		}
		
		prot->acq                           = localProtocol.acq;
		prot->duration                      = localProtocol.duration;
		myStrcpy(prot->theIsotope.name,       localProtocol.theIsotope.name);
		prot->theIsotope.halfLife           = localProtocol.theIsotope.halfLife;
		prot->theIsotope.branchingFraction  = localProtocol.theIsotope.branchingFraction;
		
		prot->breathingMon         = localProtocol.breathingMon;
		prot->xray                 = localProtocol.xray;
		tl->acqInfo.startTime      =  0;
		tl->acqInfo.endTime        =  0;
		tl->acqInfo.completionCode = -1;
	}
	
	if (localProtocol.hist){
		prot->hist                  = localProtocol.hist;
		prot->histOptions           = localProtocol.histOptions;
		myStrcpy(prot->normFile,      localProtocol.normFile);
		myStrcpy(prot->attnCorrFile,  localProtocol.attnCorrFile);
		myStrcpy(prot->frameSequence, localProtocol.frameSequence);
		tl->histInfo.startTime      =  0;
		tl->histInfo.endTime        =  0;
		tl->histInfo.completionCode = -1;
	}
	
	if (localProtocol.recon){
		prot->recon                  = localProtocol.recon;
		prot->iterations             = localProtocol.iterations;
		prot->calibrationConstant    = localProtocol.calibrationConstant;
		if (prot->calibrationConstant == 0) prot->calibrationConstant = defaultCalibrationConstant; 

		tl->reconInfo.startTime      =  0;
		tl->reconInfo.endTime        =  0;
		tl->reconInfo.completionCode = -1;
	}
	
	return 0;

}

int populateIsotopeRing(void){ 
	struct isotope *isotopeTable=NULL;
	char isotopeData[32];
	DWORD  size;
	int retCode;
	__int64 numEntries;
	int i;
	
	// Query size of isotope table first (to be able to allocate memory)
	size         = 0;
	retCode      = RegQueryValueEx(petBoxRegKey, "IsotopeList", NULL, NULL, NULL, &size);
	if (retCode!=0) {
		writeToLog("populateIsotopeRing - Registry error getting isotope list size");
		return 4;
	}
	if (size==0) return 4;
	
	isotopeTable = malloc(size);
	if (isotopeTable==NULL) {
		writeToLog("populateIsotopeRing - Can't allocate memory for isotope table");
		return 4;
	}
	numEntries   = size/sizeof(struct isotope);
	retCode      = RegQueryValueEx(petBoxRegKey, "IsotopeList", NULL, NULL, (LPBYTE)isotopeTable, &size);
	if (retCode!=0) {
		writeToLog("populateIsotopeRing - Registry error getting isotope list");
		return 4;
	}
	
	ClearListCtrl(protocolEditPanel, PROT_EDIT_isotope_RING);
	for (i=0; i<numEntries; i++) {
		sprintfWithLock(32, isotopeData, "%f %f", isotopeTable[i].halfLife, isotopeTable[i].branchingFraction); 
		InsertListItem(protocolEditPanel, PROT_EDIT_isotope_RING, -1, isotopeTable[i].name,  isotopeData);
	}
	free(isotopeTable);
	return 0;
}

static void saveIsotopeList(void){
	struct isotope *isotopeTable=NULL;
	char isotopeData[32];
	DWORD  size;
	int retCode;
	int numEntries=0;
	int i;
	
	GetNumListItems(protocolEditPanel, PROT_EDIT_isotope_RING, &numEntries);
	if(numEntries==0) return; 
	
	size = numEntries *   (DWORD)sizeof(struct isotope);
	isotopeTable = malloc(size);
	if (isotopeTable ==NULL){
		writeToLog("saveIsotopeList - Can't allocate memory for isotope table");
		return;
	}
	
	for (i=0; i<numEntries; i++){
		GetLabelFromIndex(protocolEditPanel, PROT_EDIT_isotope_RING, i, isotopeTable[i].name);
		GetValueFromIndex(protocolEditPanel, PROT_EDIT_isotope_RING, i, isotopeData);
		
		CmtGetLock(sprintfLock);
		sscanf(isotopeData, "%f %f", &isotopeTable[i].halfLife, &isotopeTable[i].branchingFraction);
		CmtReleaseLock(sprintfLock);
		
	}
	retCode = RegSetValueEx(petBoxRegKey, "IsotopeList", 0, REG_BINARY, (BYTE*)isotopeTable, size);
	free(isotopeTable);
	if (retCode !=0){
		writeToLog("saveIsotopeList - Registry error saving isotope list");
	}
}

/*
	Return index of 'isotopeName' in isotope table or -1 if not found
*/
static int getIndexFromLabel(char *isotopeName){
	int count;
	int i;
	char label[24];
	
	if (0<=GetNumListItems(protocolEditPanel, PROT_EDIT_isotope_RING, &count)){
		for (i=0; i<count; i++){
			if (0<=GetLabelFromIndex(protocolEditPanel, PROT_EDIT_isotope_RING, i,label)){
				if ( 0 == strcmp(label, isotopeName) ) return i;
			}
		}
	}
	return -1;
}

static void setSliderFromDuration(unsigned int duration){
	// Not the ideal way to do that
	if      (duration<=   5*60) SetCtrlVal(protocolEditPanel, PROT_EDIT_ACQTIME_SLIDER, 0);
	else if (duration<=  10*60) SetCtrlVal(protocolEditPanel, PROT_EDIT_ACQTIME_SLIDER, 1); 
	else if (duration<=  30*60) SetCtrlVal(protocolEditPanel, PROT_EDIT_ACQTIME_SLIDER, 2); 
	else if (duration<=  60*60) SetCtrlVal(protocolEditPanel, PROT_EDIT_ACQTIME_SLIDER, 3); 
	else                        SetCtrlVal(protocolEditPanel, PROT_EDIT_ACQTIME_SLIDER, 4); 
}

// -------------------------------------- Prepare Protocol For Display ------------------------------------------
void prepareProtocolForDisplay(){
	char temp[MAX_PATHNAME_LEN];
	struct protocol prot;
	unsigned short int hour, min, sec;
	
	if (0==readProtocolFromTag(editPanelTag, &prot)){
		
		SetCtrlVal(protocolEditPanel,   PROT_EDIT_protName, prot.name);
		SetCtrlIndex(protocolEditPanel, PROT_EDIT_isotope_RING, getIndexFromLabel(prot.theIsotope.name));
	
		// Locate actual norm file in the list
		SplitPath(prot.normFile, 0, 0, temp);
		removeExtension(temp);
	 	selectStringInRing(protocolEditPanel, PROT_EDIT_normFileRing, temp);
		
		// do the same for attenuation correction
		SplitPath(prot.attnCorrFile, 0, 0, temp);
		removeExtension(temp);
	 	selectStringInRing(protocolEditPanel, PROT_EDIT_attnCorrFileRing, temp);
	
		/* Acquisition related parameters */
		SetCtrlVal(protocolEditPanel, PROT_EDIT_acq_CHK,        prot.acq         );
		SetCtrlVal(protocolEditPanel, PROT_EDIT_breathMon_CHK,  prot.breathingMon);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_xray_CHK,       prot.xray        );
	
		durationToHms(&hour, &min, &sec, prot.duration);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_hour,  hour);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_min,   min);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_sec,   sec);
		setSliderFromDuration(prot.duration);

		/* Histogramming related parameters */
		SetCtrlVal(protocolEditPanel, PROT_EDIT_hist_CHK,             prot.hist);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_histFrameSeq,  (char*)prot.frameSequence);
		playBack(prot.frameSequence);
		
		/* Hist OPTION handling */
		SetCtrlVal(protocolEditPanel, PROT_EDIT_ATTNCORR_OPT,    prot.histOptions & BIT_ATTNCORR);          
		SetCtrlVal(protocolEditPanel, PROT_EDIT_RNDCORR_OPT,     prot.histOptions & BIT_RNDCORR);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_NORM_OPT,        prot.histOptions & BIT_NORM);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_DTIMCORR_OPT,    prot.histOptions & BIT_DTIMCORR);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_DCYCORR_OPT,     prot.histOptions & BIT_DCYCORR);
		SetCtrlVal(protocolEditPanel, PROT_EDIT_iterations,      prot.iterations);


		if (prot.calibrationConstant == 0){
			SetCtrlVal(protocolEditPanel, PROT_EDIT_calibConstant,   defaultCalibrationConstant);
			SetCtrlAttribute(protocolEditPanel, PROT_EDIT_calibConstant, ATTR_TEXT_BGCOLOR, VAL_LT_GRAY);
		}
		else {
			SetCtrlVal(protocolEditPanel, PROT_EDIT_calibConstant,   prot.calibrationConstant);
 			SetCtrlAttribute(protocolEditPanel, PROT_EDIT_calibConstant, ATTR_TEXT_BGCOLOR, VAL_WHITE);
		}

		/* Recon */
		SetCtrlVal(protocolEditPanel, PROT_EDIT_recon_CHK, prot.recon);
	}
							  
}

static void prepareNewProtocolForDisplay(void){
	const unsigned int defaultDuration = 600; // 600s = 10 min
	unsigned short int hour, min, sec;

	durationToHms(&hour, &min, &sec, defaultDuration);
 	setSliderFromDuration(defaultDuration);

	SetCtrlAttribute( protocolEditPanel, PROT_EDIT_protName, ATTR_DIMMED,0);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_protName,             "");

	SetCtrlIndex(     protocolEditPanel, PROT_EDIT_isotope_RING, 0);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_ATTNCORR_OPT,         defaultHistOptions & BIT_ATTNCORR);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_RNDCORR_OPT,          defaultHistOptions & BIT_RNDCORR);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_NORM_OPT,             defaultHistOptions & BIT_NORM);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_DTIMCORR_OPT,         defaultHistOptions & BIT_DTIMCORR);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_DCYCORR_OPT,          defaultHistOptions & BIT_DCYCORR);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_iterations,           defaultIterations);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_breathMon_CHK,        defaultBreathingMonitoring);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_xray_CHK,             defaultXray);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_calibConstant,        defaultCalibrationConstant);
	SetCtrlAttribute( protocolEditPanel, PROT_EDIT_calibConstant,        ATTR_TEXT_BGCOLOR, VAL_LT_GRAY);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_hour,                 hour);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_min,                  min);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_sec,                  sec);
	SetCtrlVal(       protocolEditPanel, PROT_EDIT_histFrameSeq,        "1x*");
	playBack("1x*");
	
}

int writeProtocolToDisk(char* pathName, struct protocol* prot){
	FILE* protFile = fopen(pathName,"wb");
	prot->version = CURRENT_PROTOCOL_VERSION;

	if (protFile){
		fwrite(prot, sizeof(*prot), 1, protFile);
		fclose(protFile);
		return 0;
	}
	else {
		writeToLog("protocolEdit::writeProtocolToDisk - ERROR, could not open protocol file for output");
		MessagePopup("Logic Error", "Please notify Sofie Biosciences of this error");
		return 8;
	}
}


static int panelToProtocolData(struct protocol *p){
	
	char   temp[MAX_PATHNAME_LEN];
	unsigned short int hour, min, sec; 
	int item;	                              // Index of the selected isotope in the pull-down menu (ring)  
	char isotopeData[32];
	
	GetCtrlVal(protocolEditPanel, PROT_EDIT_protName, p->name);
	
	/* Acquisition related parameters */
	GetCtrlIndex(protocolEditPanel,      PROT_EDIT_isotope_RING, &item);	     
	
	GetLabelFromIndex(protocolEditPanel, PROT_EDIT_isotope_RING, item, p->theIsotope.name);
	GetCtrlVal(protocolEditPanel,        PROT_EDIT_isotope_RING, isotopeData );
	
	CmtGetLock(sprintfLock);
	sscanf(isotopeData, "%f %f", &p->theIsotope.halfLife, &p->theIsotope.branchingFraction);
	CmtReleaseLock(sprintfLock);
	
	GetCtrlVal(protocolEditPanel, PROT_EDIT_acq_CHK,        &(p->acq) );
	GetCtrlVal(protocolEditPanel, PROT_EDIT_breathMon_CHK,  &(p->breathingMon) );
	GetCtrlVal(protocolEditPanel, PROT_EDIT_xray_CHK,       &(p->xray) );
	GetCtrlVal(protocolEditPanel, PROT_EDIT_hour, &hour);
	GetCtrlVal(protocolEditPanel, PROT_EDIT_min,  &min);
	GetCtrlVal(protocolEditPanel, PROT_EDIT_sec,  &sec);
	hmsToDuration(hour, min, sec, &(p->duration) );

	/* Histogramming related parameters */
	GetCtrlVal(protocolEditPanel, PROT_EDIT_hist_CHK,  &(p->hist) );
																	  
	/* Histogramming options here ... */ 
	p->histOptions = 0;
	getHistOptions(protocolEditPanel, &(p->histOptions), PROT_EDIT_ATTNCORR_OPT,   BIT_ATTNCORR); 
	getHistOptions(protocolEditPanel, &(p->histOptions), PROT_EDIT_RNDCORR_OPT,    BIT_RNDCORR); 
	getHistOptions(protocolEditPanel, &(p->histOptions), PROT_EDIT_NORM_OPT,       BIT_NORM); 
	getHistOptions(protocolEditPanel, &(p->histOptions), PROT_EDIT_DTIMCORR_OPT,   BIT_DTIMCORR); 
	getHistOptions(protocolEditPanel, &(p->histOptions), PROT_EDIT_DCYCORR_OPT,    BIT_DCYCORR); 

	GetCtrlVal(protocolEditPanel,     PROT_EDIT_iterations,    &(p->iterations) );

	int bgColor=0;
	GetCtrlAttribute(protocolEditPanel,     PROT_EDIT_calibConstant, ATTR_TEXT_BGCOLOR, &bgColor);
	if (bgColor == VAL_LT_GRAY) p->calibrationConstant = 0;
	else GetCtrlVal(protocolEditPanel,     PROT_EDIT_calibConstant, &(p->calibrationConstant) );
	
	/* Attenuation correction file */
	if (0==GetCtrlVal(protocolEditPanel, PROT_EDIT_attnCorrFileRing,  &item)){
		GetLabelFromIndex(protocolEditPanel, PROT_EDIT_attnCorrFileRing, item, temp);
		sprintfWithLock(MAX_FILE_NAME_LENGTH, p->attnCorrFile,"%s.atn", temp);
	}else{
		myStrcpy(p->attnCorrFile, "");
	}
	
	/* Reconstruction parameters */
	GetCtrlVal(protocolEditPanel, PROT_EDIT_recon_CHK,        &(p->recon) );
	
	/* Normalization file */
	if (0==GetCtrlVal(protocolEditPanel, PROT_EDIT_normFileRing,  &item)){
		GetLabelFromIndex(protocolEditPanel, PROT_EDIT_normFileRing, item, temp);
		sprintfWithLock(MAX_FILE_NAME_LENGTH, p->normFile,"%s.nrm", temp);
	}else{
		myStrcpy(p->normFile,"");
	}

	// Framing sequence
	GetCtrlVal(protocolEditPanel, PROT_EDIT_histFrameSeq, (char*)&(p->frameSequence) );
	

	return 0;
}


// -------------------------------------- RENAME Protocol ------------------------------------------

/*
    Rename protocol on disk, tag points to protocol(with new name);

	(in) tag : identifies which protocol
	(in) oldProtocolName: is the old protocolName
	
*/
int renameProtocol(char *tag, char *oldProtocolName){
	
	char  fullPath[MAX_FILE_NAME_LENGTH]; 
	char  newProtocolName[MAX_FILE_NAME_LENGTH];
 	char  oldPathName[MAX_FILE_NAME_LENGTH]; 
 	char  newPathName[MAX_FILE_NAME_LENGTH];

	int   index;

	myGetTreeItemPathFromTag(protocolTabHandle, TAB_PROT_TREE, tag, fullPath, 0);
	GetTreeItemFromTag(protocolTabHandle, TAB_PROT_TREE, tag, &index);
	GetLabelFromIndex(protocolTabHandle, TAB_PROT_TREE, index, newProtocolName);
	
	sprintfWithLock(MAX_FILE_NAME_LENGTH, oldPathName,"%s\\%s\\%s%s", protFolder, fullPath, oldProtocolName, ".ptc");
	sprintfWithLock(MAX_FILE_NAME_LENGTH, newPathName,"%s\\%s\\%s%s", protFolder, fullPath, newProtocolName, ".ptc");
	
	struct protocol prot;
	
	// Read in and update name in protocol
	readProtocol(oldPathName, &prot);
	myStrcpy(prot.name, newProtocolName);

	// Write new protocol, delete old one
	if ( 0 != writeProtocolToDisk(newPathName, &prot) ) return 8;
	if ( 0 != DeleteFile(oldPathName)                 ) return 8;
	
	updateProtocolSubMenu();
	return 0;
}


// -------------------------------------- DELETE Protocol ------------------------------------------
int deleteProtocol(char *tag){
	char path[MAX_FILE_NAME_LENGTH];
	
	if(0==makeProtocolPathName(tag,path, ".ptc")){
		return DeleteFile(path);
	}
	else return 8;
}
// -------------------------------------- SAVE Protocol ------------------------------------------
int CVICALLBACK protSave_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	char pathName[MAX_FILE_NAME_LENGTH]; 
	struct protocol p;
											
	switch (event) {
		
		// Validate before saving: make sure name is unique if new
		case EVENT_COMMIT:
			panelToProtocolData(&p);
			switch (editProtocolPanelReason){
				case NEW:
					makeUniqueID(p.tag);	
 					insertChildInProtocolTree(p.name, p.tag, lastIndex, theColor, theBGColor, PROT_IMAGE_INDEX);		
					break;
					
				case EDIT:
					myStrcpy(p.tag, editPanelTag);
					break;
			
			}
			
			if (0!=makeProtocolPathName(p.tag, pathName, ".ptc")){
				writeToLog("Internal error in writeProtocol - Invalid protocol index");
				return 8;
			}
			writeProtocolToDisk(pathName, &p);
	
			/* Update the protocol subMenu */
			updateProtocolSubMenu(); 			
			fadeOutPanel(&editPanelFading);
			break;
	}
	
	return 0;
}

// -------------------------------------- DISCARD Protocol ------------------------------------------
int CVICALLBACK protDiscard_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			fadeOutPanel(&editPanelFading);
			break;
	}
	return 0;
}

// -------------------------------------- EDIT Protocol ------------------------------------------
void editProtocol(void){

	populateNormRing();
	populateIsotopeRing();
	populateAttenuationRing();
	
	if (editProtocolPanelReason == EDIT){
		prepareProtocolForDisplay();
		SetCtrlAttribute(protocolEditPanel, PROT_EDIT_protName, ATTR_DIMMED,1);

	} else {
		// This is a new protocol; set default values
		prepareNewProtocolForDisplay();
	}
	
	fadeInPanel(&editPanelFading);
}

// Populate attenuation correction files ring
static void populateAttenuationRing(void){
	int size;
	char **nameArray;
	int i;
		
	// Populate  file pull-down menu
	ClearListCtrl(protocolEditPanel, PROT_EDIT_attnCorrFileRing);
	sprintfWithLock(2048 ,tempString,"%s\\*.atn",normFolder);
	nameArray = createDirList(tempString,0,&size);
	InsertListItem(protocolEditPanel, PROT_EDIT_attnCorrFileRing, 0, "*default*", 0);

		if (nameArray != NULL){
		for (i=0; i<size; i++){
			InsertListItem(protocolEditPanel, PROT_EDIT_attnCorrFileRing, i+1, removeExtension(nameArray[i]), i+1);
		}
		freeDirList(nameArray, size);
	}

}

// -------------------- Populate Normalization pull-down menu with norm file names ---------------------
static void populateNormRing(void){
	int size;
	char **nameArray;
	int i;
		
	// Populate normalization file pull-down menu
	ClearListCtrl(protocolEditPanel, PROT_EDIT_normFileRing);
	sprintfWithLock(2048, tempString,"%s\\*.nrm",normFolder);
	nameArray = createDirList(tempString,0,&size);
	InsertListItem(protocolEditPanel, PROT_EDIT_normFileRing, 0, "*default*", 0);

	if (nameArray != NULL){
		for (i=0; i<size; i++){
			InsertListItem(protocolEditPanel, PROT_EDIT_normFileRing, i+1, removeExtension(nameArray[i]), i+1);
		}
		freeDirList(nameArray, size);
	}
}


static int GetIndexFromLabel(int panelHandle, int controlID, char* soughtLabel){
	int i;
	int size;
	char label[MAX_FILE_NAME_LENGTH];
	
	GetNumListItems(panelHandle, controlID, &size);
	for (i=0; i<size; i++){
		GetLabelFromIndex(panelHandle, controlID, i,label);	
		if (0==strcmp(label, soughtLabel)) return i;
	}
	
	return -1;
}



static void selectStringInRing(int panelHandle, int controlID, char* string){
	int idx;
	
	idx = GetIndexFromLabel(panelHandle, controlID, string);
	if (idx >= 0) SetCtrlVal (panelHandle, controlID,  idx);
	
}

void isotopeRingToTable(void){
	int   i;
	int   count;
	char  isotopeName[8];
	char  isotopeData[32];
	
	float halfLife;
	float branchingFraction;
	
	GetNumListItems(protocolEditPanel, PROT_EDIT_isotope_RING, &count);
	DeleteTableRows(isotopePanel, ISOT_PANEL_TABLE, 1, -1); // Clear Table
	
	for (i=0; i<count; i++){
		GetLabelFromIndex(protocolEditPanel, PROT_EDIT_isotope_RING, i, isotopeName);
		GetValueFromIndex(protocolEditPanel, PROT_EDIT_isotope_RING, i, isotopeData);
		
		CmtGetLock(sprintfLock);
		sscanf(isotopeData, "%f %f", &halfLife, &branchingFraction);
		CmtReleaseLock(sprintfLock);
		
		InsertTableRows( isotopePanel, ISOT_PANEL_TABLE, -1, 1, VAL_USE_MASTER_CELL_TYPE);
		SetTableCellVal (isotopePanel, ISOT_PANEL_TABLE, MakePoint (1, i+1), isotopeName);
		SetTableCellVal (isotopePanel, ISOT_PANEL_TABLE, MakePoint (2, i+1), halfLife);
		SetTableCellVal (isotopePanel, ISOT_PANEL_TABLE, MakePoint (3, i+1), branchingFraction);
		
	}
}


void isotopeTableToRing(void){
	int   i;
	int   count;
	char  isotopeName[8];
	float halfLife;
	float branchingFraction;
	char  isotopeData[32];
	
	GetNumTableRows(isotopePanel, ISOT_PANEL_TABLE, &count);
	ClearListCtrl(protocolEditPanel, PROT_EDIT_isotope_RING);
	for (i=0; i<count; i++){
		GetTableCellVal(isotopePanel, ISOT_PANEL_TABLE, MakePoint (1, i+1), isotopeName);   
		GetTableCellVal(isotopePanel, ISOT_PANEL_TABLE, MakePoint (2, i+1), &halfLife);   
		GetTableCellVal(isotopePanel, ISOT_PANEL_TABLE, MakePoint (3, i+1), &branchingFraction);
		sprintfWithLock(32, isotopeData, "%f %f", halfLife, branchingFraction);
		InsertListItem(protocolEditPanel, PROT_EDIT_isotope_RING, -1, isotopeName,  isotopeData);
	}
}

int CVICALLBACK isotope_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_RIGHT_CLICK:
			isotopeRingToTable();
			SetPanelPos(isotopePanel, eventData1, eventData2);
			DisplayPanel(isotopePanel);
			break;
	}
	return 0;
}

int CVICALLBACK isotopeSave_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			isotopeTableToRing();
			saveIsotopeList();
			HidePanel(isotopePanel);
			break;
	}
	return 0;
}

int CVICALLBACK frameSeq_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int character;
	int virtualKey;
	
	switch (event)
	{
		case EVENT_COMMIT:
			break;
			
		case EVENT_GOT_FOCUS:
			// Temporarily disable enter key as meaning "save"
			SetCtrlAttribute(protocolEditPanel, PROT_EDIT_SAVE_BUTTON, ATTR_SHORTCUT_KEY, 0);
			break;
			
		case EVENT_LOST_FOCUS:
			if (isCompleted()) string_CB_retVal = 0;
			else {
				SetActiveCtrl(protocolEditPanel, PROT_EDIT_histFrameSeq);
				mustReplay=1;
			}
			break;
			
		case EVENT_KEYPRESS:
			virtualKey = GetKeyPressEventVirtualKey (eventData2);
			character  = GetKeyPressEventCharacter (eventData2);
			SetCtrlAttribute(protocolEditPanel, PROT_EDIT_histFrameSeq, ATTR_TEXT_COLOR, VAL_BLACK); 
			
			/* ignore leading bytes - process only one-byte character or trail byte of two-byte character */
			if (!KeyPressEventIsLeadByte (eventData2))
				runStateMachine(character, virtualKey);
			
			// Restore save key if done
			if (isCompleted())
				SetCtrlAttribute(protocolEditPanel, PROT_EDIT_SAVE_BUTTON, ATTR_SHORTCUT_KEY, VAL_ENTER_VKEY);

			return string_CB_retVal;
			//break;
	}
	return 0;
}

void CVICALLBACK insertIsotope (int panelHandle, int controlID, int MenuItemID, void *callbackData){
	int count;
	InsertTableRows(isotopePanel, ISOT_PANEL_TABLE, -1, 1, VAL_USE_MASTER_CELL_TYPE);
	GetNumTableRows(isotopePanel, ISOT_PANEL_TABLE, &count);
	SetActiveTableCell(isotopePanel, ISOT_PANEL_TABLE, MakePoint(1, count));
}


void CVICALLBACK deleteIsotope (int panelHandle, int controlID, int MenuItemID, void *callbackData){
	Point cell;
	GetActiveTableCell(isotopePanel, ISOT_PANEL_TABLE, &cell);
	DeleteTableRows(isotopePanel, ISOT_PANEL_TABLE, cell.y, 1);
}


int CVICALLBACK calibConstant_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		// Activate control to allow edition
		case EVENT_LEFT_CLICK:
			SetCtrlAttribute(panel, control, ATTR_TEXT_BGCOLOR, VAL_WHITE);
			break;
	}
	return 0;
}

int CVICALLBACK protName_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char c; 
	int retCode =0;
	
	switch (event){
		case EVENT_COMMIT:

			break;
			
		case EVENT_KEYPRESS:

				c = GetKeyPressEventCharacter(eventData2);
				switch (c){
					// Virtual key
					case 0:
						break;
					// Character case	
					default:
						if ( strchr("/\\:*?\"<>|.", c) ) retCode=1;  // Swallow event
						break;
				}
			
			break;
			
			
		case EVENT_RIGHT_CLICK:

			break;
	}
	
	return retCode;
}

int CVICALLBACK slider_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned short hour, min, sec;
	int valueIndex;
	unsigned short values[]={5, 10, 30, 60, 120};  // in min
	
	switch (event)
	{
		case EVENT_VAL_CHANGED:
			GetCtrlVal(panel, control, &valueIndex);
			durationToHms(&hour, &min, &sec, 60*values[valueIndex]);
			SetCtrlVal(protocolEditPanel, PROT_EDIT_hour,  hour);
			SetCtrlVal(protocolEditPanel, PROT_EDIT_min,   min);
			SetCtrlVal(protocolEditPanel, PROT_EDIT_sec,   sec);
			break;
	}
	return 0;
}

