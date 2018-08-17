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


#include "windows.h"
#include <utility.h>
#include <ansi_c.h>
#include <userint.h>


#include "globalDecls.h"
#include "genisys4GUI.h"
#include "acquisitionManagement.h"
#include "utilities.h"
#include "logManagement.h"




int acquisitionManagementInitialization(void){
	
	const int darkRed              = MakeColor(192,   0,   0);  // Too hot color
	const int darkYellow           = MakeColor(192, 192,   0);  // High rev color     
	const int darkGreen            = MakeColor( 51, 153,   0);  // ON color
	
	const int numLED               =  24;
	const int minValue             =   5;
	const int maxValue             = 120;

	// LED control (prompts) 
	promptsRateMeter.bgColor       = MakeColor( 77,  77,  77);  // OFF color   
	promptsRateMeter.overflowColor = MakeColor(192, 192, 192);  // Overflow (i.e. greater than max display)      
	promptsRateMeter.numLED        = numLED;
	promptsRateMeter.vmin          = minValue;
	promptsRateMeter.vmax          = maxValue;
	
	promptsRateMeter.LEDIDarray    = malloc(sizeof(int)* numLED);
	promptsRateMeter.LEDColorArray = malloc(sizeof(int)* numLED);
	
	if (promptsRateMeter.LEDIDarray == NULL || promptsRateMeter.LEDColorArray == NULL){
		writeToLog("acquisitionManagement::acquisitionManagementInitialization - ERROR - can't allocate memory for LEDIDarray or color array");
		return 8;
	}

	// Annoying to code, but easier to maintain 
	promptsRateMeter.LEDIDarray[ 0] = ACQ_PANEL_rate_0;
	promptsRateMeter.LEDIDarray[ 1] = ACQ_PANEL_rate_1;
	promptsRateMeter.LEDIDarray[ 2] = ACQ_PANEL_rate_2;
	promptsRateMeter.LEDIDarray[ 3] = ACQ_PANEL_rate_3;
	promptsRateMeter.LEDIDarray[ 4] = ACQ_PANEL_rate_4;
	promptsRateMeter.LEDIDarray[ 5] = ACQ_PANEL_rate_5;
	promptsRateMeter.LEDIDarray[ 6] = ACQ_PANEL_rate_6;
	promptsRateMeter.LEDIDarray[ 7] = ACQ_PANEL_rate_7;
	promptsRateMeter.LEDIDarray[ 8] = ACQ_PANEL_rate_8;
	promptsRateMeter.LEDIDarray[ 9] = ACQ_PANEL_rate_9;
	promptsRateMeter.LEDIDarray[10] = ACQ_PANEL_rate_10;
	promptsRateMeter.LEDIDarray[11] = ACQ_PANEL_rate_11;
	promptsRateMeter.LEDIDarray[12] = ACQ_PANEL_rate_12;
	promptsRateMeter.LEDIDarray[13] = ACQ_PANEL_rate_13;
	promptsRateMeter.LEDIDarray[14] = ACQ_PANEL_rate_14;
	promptsRateMeter.LEDIDarray[15] = ACQ_PANEL_rate_15;
	promptsRateMeter.LEDIDarray[16] = ACQ_PANEL_rate_16;
	promptsRateMeter.LEDIDarray[17] = ACQ_PANEL_rate_17;
	promptsRateMeter.LEDIDarray[18] = ACQ_PANEL_rate_18;
	promptsRateMeter.LEDIDarray[19] = ACQ_PANEL_rate_19;
	promptsRateMeter.LEDIDarray[20] = ACQ_PANEL_rate_20;
	promptsRateMeter.LEDIDarray[21] = ACQ_PANEL_rate_21;
	promptsRateMeter.LEDIDarray[22] = ACQ_PANEL_rate_22;
	promptsRateMeter.LEDIDarray[23] = ACQ_PANEL_rate_23;
	
	/*
	promptsRateMeter.LEDIDarray[24] = ACQ_PANEL_rate_24;
	promptsRateMeter.LEDIDarray[25] = ACQ_PANEL_rate_25;
	promptsRateMeter.LEDIDarray[26] = ACQ_PANEL_rate_26;
	promptsRateMeter.LEDIDarray[27] = ACQ_PANEL_rate_27;
	promptsRateMeter.LEDIDarray[28] = ACQ_PANEL_rate_28;
	promptsRateMeter.LEDIDarray[29] = ACQ_PANEL_rate_29;
	promptsRateMeter.LEDIDarray[30] = ACQ_PANEL_rate_30;
	promptsRateMeter.LEDIDarray[31] = ACQ_PANEL_rate_31;
	promptsRateMeter.LEDIDarray[32] = ACQ_PANEL_rate_32;
	promptsRateMeter.LEDIDarray[33] = ACQ_PANEL_rate_33;
	promptsRateMeter.LEDIDarray[34] = ACQ_PANEL_rate_34;
	promptsRateMeter.LEDIDarray[35] = ACQ_PANEL_rate_35;
	promptsRateMeter.LEDIDarray[36] = ACQ_PANEL_rate_36;
	promptsRateMeter.LEDIDarray[37] = ACQ_PANEL_rate_37;
	promptsRateMeter.LEDIDarray[38] = ACQ_PANEL_rate_38;
	promptsRateMeter.LEDIDarray[39] = ACQ_PANEL_rate_39;
	*/
	
	// Color asignment
	for (int k= 0; k<15;     k++) promptsRateMeter.LEDColorArray[k] = darkGreen ;
	for (int k=15; k<19;     k++) promptsRateMeter.LEDColorArray[k] = darkYellow;
	for (int k=19; k<numLED; k++) promptsRateMeter.LEDColorArray[k] = darkRed;

	
	LEDControlInitialization(&promptsRateMeter, acquisitionPanel );
	
	return 0;
}

void acquisitionManagementTermination(void){
	free(promptsRateMeter.LEDIDarray);
	free(promptsRateMeter.LEDColorArray);
	LEDControlTermination(&promptsRateMeter);
}









