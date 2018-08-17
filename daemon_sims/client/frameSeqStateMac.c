//*******************************************************************************
//*                                                                             *
//*  GENISYS4 -  Graphical User Interface                                       *
//*                                                                             *
//*  Copyright - 2009, 2010, 2011                                               *
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
#include <cvirte.h>		
#include <userint.h>
#include "globalDecls.h"
#include "genisys4GUI.h"
#include "typedefs.h"

#include "utilities.h"
#include "logManagement.h"
#include "frameSeqStateMac.h"

enum states{ STATE_FRAMES, STATE_DURATION_FIRST, STATE_DURATION, STATE_ACCEPT_WHITESPACE, STATE_COMPLETED, STATE_SAME, STATE_UNKNOWN};

#define NUM_STATES 100
static enum states stateStack[NUM_STATES];
static int stackPointer = 0;

static char numFrames[100];
static int  numFramesPointer=0;

void pushChar(char c){
	numFrames[numFramesPointer++]=c;
	numFrames[numFramesPointer]=0;
}

void pushState(enum states s){
	stateStack[stackPointer++] = s;
	if (stackPointer>=NUM_STATES){
		writeToLog("pushState - state stack overflow"); 
		// Get back to a known state
		SetCtrlVal(protocolEditPanel, PROT_EDIT_histFrameSeq, "1x*");  
		playBack("1x*");
	}
}

void popState(void){
	if (stackPointer>0) stackPointer--;
	else {
		writeToLog("pushState - state stack underflow"); 
		// Get back to a known state
		SetCtrlVal(protocolEditPanel, PROT_EDIT_histFrameSeq, "1x*");  
		playBack("1x*");
	}
}

int currentState(void){
	if (stackPointer>0) return stateStack[stackPointer-1];
	else                return -1;
}

void resetStateStack(void){
	numFramesPointer=0;
	stackPointer=0;
	pushState(STATE_FRAMES);  // initialize state machine
}

// Set next state value and perform required action
void stateAction(enum states s, int a, unsigned char c){
	if ( s != STATE_SAME) pushState(s);
	if ( a & ACTION_RESET_NUM_FRAMES) numFramesPointer=0; 
	if ( a & ACTION_PUSH_CHAR)        pushChar(c); 
	if ( a & ACTION_MUST_REPLAY)      mustReplay=1; 
	if ( a & ACTION_BEEP){
		SetCtrlAttribute(protocolEditPanel, PROT_EDIT_histFrameSeq, ATTR_TEXT_COLOR, VAL_RED);
		Beep();
	}
	if      (a & ACTION_REJECT) string_CB_retVal=1;  // don't add key to string (swallow)
	else if (a & ACTION_ACCEPT) string_CB_retVal=0;  // let CVI add key to string
}

int isCompleted(){
	return (STATE_COMPLETED == currentState());
}

/*
	"play back" a stored sequence of characters to bring the state
	machine to the right state.  The string is assumed valid.
*/
void playBack(char *s){
	resetStateStack();
	while( *s ) runStateMachine(*s++,0);
}

/*

	This state machine only allows typing of valid framing sequences.
	
	A framing sequence is an expression containing a pair of numbers 
	(number of frames and duration) separated by the multiplication  
	sign 'x'.  For example, "6x10" means 6 frames of 10 seconds.
	
	Several expressions, separated by commas, can be put together to form
	more complex framing sequences.  For example, "6x10, 8x30" means 6
	ten-second frames followed by 8 thirty-second frames. Spaces are 
	allowed after the comma to improve readability.
	
	An asterisk '*' can be used for duration to mean "until the end".  For
	example, the framing sequence for a static study can be expressed
	as '1x*'.  The asterisk only makes sense when used with '1' as the 
	number of frames and at the end of the whole framing sequence.  An 
	example for a dynamic study could be: '6x10, 8x30, 5x300, 1x*'
	
	--
	
	To allow deletion during typing (through the backspace key and
	text-selection and backspace key), the entire succession of states
	is kept in a stack.  When n characters are deleted, we go back n
	states in the stack.
	
	When a previously stored framing sequence is edited, the sequence
	is first "replayed" through the state machine to bring it into
	the right state.
	
*/


void runStateMachine(int character, int virtualKey){
	char theString[300];
	int selLength;
	int totalLength;
	int start;
	
	if (mustReplay){
		GetCtrlVal(protocolEditPanel, PROT_EDIT_histFrameSeq, theString);  
		mustReplay=0;
		playBack(theString);
	}
		
	if (virtualKey == VAL_BACKSPACE_VKEY  )
	{
		SetCtrlAttribute(protocolEditPanel, PROT_EDIT_SAVE_BUTTON,       ATTR_SHORTCUT_KEY, 0);
		GetCtrlAttribute(protocolEditPanel, PROT_EDIT_histFrameSeq,   ATTR_TEXT_SELECTION_LENGTH, &selLength);
		GetCtrlAttribute(protocolEditPanel, PROT_EDIT_histFrameSeq,   ATTR_TEXT_SELECTION_START,  &start);
		GetCtrlAttribute(protocolEditPanel, PROT_EDIT_histFrameSeq,   ATTR_STRING_TEXT_LENGTH,    &totalLength);
		if ((selLength+start)<totalLength)  stateAction(STATE_SAME,    ACTION_REJECT + ACTION_BEEP, 0);
		else                                stateAction(STATE_UNKNOWN, ACTION_ACCEPT + ACTION_MUST_REPLAY, 0);
	} 
	
	else
	{
		switch(currentState())
		{
			case STATE_FRAMES:
				if      ( isdigit(character)           ) stateAction(STATE_SAME,              ACTION_ACCEPT + ACTION_PUSH_CHAR, character);
				else if ( character  == 'x'            ) stateAction(STATE_DURATION_FIRST,    ACTION_ACCEPT,                            0);
				else if ( virtualKey == VAL_TAB_VKEY   ) stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);   
				else if ( virtualKey == VAL_ENTER_VKEY ) stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);  
				else                                     stateAction(STATE_SAME,              ACTION_REJECT,                            0);  
				break;
				
			case STATE_DURATION_FIRST:
				if      ( isdigit(character)         )   stateAction(STATE_DURATION,          ACTION_ACCEPT,                            0);
				else if ( character == '*'   )  
				{
					if( 1!= atoi(numFrames))             stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);
					else                                 stateAction(STATE_COMPLETED,         ACTION_ACCEPT,                            0);
				}
				else if ( virtualKey == VAL_TAB_VKEY   ) stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);  
				else if ( virtualKey == VAL_ENTER_VKEY ) stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);  
				else                                     stateAction(STATE_SAME,              ACTION_REJECT,                            0);
				break;
				
			case STATE_DURATION:
				if      ( isdigit(character) )           stateAction(STATE_SAME,              ACTION_ACCEPT,                            0);  
				else if ( character == ','   )           stateAction(STATE_ACCEPT_WHITESPACE, ACTION_ACCEPT,                            0);  
				else if ( virtualKey == VAL_TAB_VKEY   ) stateAction(STATE_COMPLETED,         ACTION_ACCEPT,                            0);
				else if ( virtualKey == VAL_ENTER_VKEY ) stateAction(STATE_COMPLETED,         ACTION_ACCEPT,                            0);
				else                                     stateAction(STATE_SAME,              ACTION_REJECT,                            0);
				break;
				
			case STATE_ACCEPT_WHITESPACE:
				if      ( character == ' ' )             stateAction(STATE_SAME,              ACTION_ACCEPT,                            0);
				else if ( isdigit(character)  ) 		 stateAction(STATE_FRAMES,            ACTION_ACCEPT + ACTION_PUSH_CHAR + ACTION_RESET_NUM_FRAMES,  character); 
				else if ( virtualKey == VAL_TAB_VKEY   ) stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);  
				else if ( virtualKey == VAL_ENTER_VKEY ) stateAction(STATE_SAME,              ACTION_REJECT + ACTION_BEEP,              0);  
				else                                     stateAction(STATE_SAME,              ACTION_REJECT,                            0);
				break;
				
			case STATE_COMPLETED:
				if ( virtualKey == VAL_TAB_VKEY   )      stateAction(STATE_SAME,              ACTION_ACCEPT,                            0);
				else                                     stateAction(STATE_SAME,              ACTION_REJECT,                            0);
				break;
		}
	}


}
