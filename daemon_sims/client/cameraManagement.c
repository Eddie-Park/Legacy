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
#include <userint.h>
#include "genisys4GUI.h"
#include <ansi_c.h>
#include "globalDecls.h"
#include "cameraManagement.h"
#include "preferenceManagement.h"
#include "utilities.h"
#include "logManagement.h"
#include "FlyCapture2_C.h"

// Local variables
static fc2Error   error;
static 	int bitMapID;

// These pointers are for the display loop
static  fc2Context      currentContext;  // context is actually a pointer
static  struct cameraSettings *currentView;

// Local prototypes
static void updatePanelControlsFromCurrentView(void);
static void updateControlRangeFromCamera(fc2Context context); 
static void updateCameraSettingsFromView(fc2Context context, struct cameraSettings *view );
static int  CVICALLBACK  displayLoopThread(void *functionData);
static void stopAcquisitionLoop(void);


#define MICROSECOND      1
#define SECOND           1000000 
#define HUNDRETH_SECOND  (SECOND/100)

int cameraInitialization(){
	int i;
	int width;
	int height;
	
	unsigned int numCams;
	
	fc2CreateContext( &contextSideCam );
	fc2GetNumOfCameras( contextSideCam, &numCams );
	fc2PGRGuid guid;
	if (numCams != 1) {
		writeToLog("cameraManagement::cameraInitialization - No camera detected; respiration monitoring disabled.");
		return 0;
	}
    error = fc2GetCameraFromIndex( contextSideCam, 0, &guid );
	error = fc2Connect( contextSideCam, &guid );  
	error = fc2WriteRegister (contextSideCam, 0x00, 0x80000000); 	

	// Make sure we can get time stamps
	fc2CameraInfo camInfo;
    error = fc2GetCameraInfo( contextSideCam, &camInfo );
    fc2EmbeddedImageInfo embeddedInfo;
    error = fc2GetEmbeddedImageInfo( contextSideCam, &embeddedInfo );
	if (! embeddedInfo.timestamp.available ){
		writeToLog("cameraManagement::cameraInitialization - Camera time stamping not available; respiration monitoring disabled.");
		return 0;
	}
    embeddedInfo.timestamp.onOff = TRUE;
    error = fc2SetEmbeddedImageInfo( contextSideCam, &embeddedInfo );

	// Set grabbing time out as infinite, so a call to retrieve buffer becomes a blocking call 
	fc2Config theConfig;
	error = fc2GetConfiguration(contextSideCam, &theConfig);
	theConfig.grabTimeout = FC2_TIMEOUT_INFINITE;
	error = fc2SetConfiguration(contextSideCam, &theConfig);
	
 	updateControlRangeFromCamera(contextSideCam);
	
	/*

	// ---------------------------------------------------------------------
	// Camera test stuff

	fc2Image  image;
 	fc2TimeStamp currentTs;
 	fc2TimeStamp previousTs;

	currentContext = contextSideCam;
	currentView    = &sideView;
	__int64 delta;
	
	openView(currentContext, currentView);	
    error = fc2CreateImage( &image );
	
	error = fc2RetrieveBuffer (contextSideCam, &image);
	if (error != FC2_ERROR_OK) printf("error\n");
	previousTs = fc2GetImageTimeStamp( &image);
	Delay(1.0);
	
	for (i=0; i<5; i++){
		error = fc2RetrieveBuffer (contextSideCam, &image);
		if (error != FC2_ERROR_OK) printf("error\n");
		currentTs = fc2GetImageTimeStamp( &image);
		delta = (currentTs.seconds - previousTs.seconds)*SECOND + (currentTs.microSeconds - previousTs.microSeconds)*MICROSECOND;
		delta /= HUNDRETH_SECOND;
		printf("delta:%d\n", (int) delta);
		previousTs=currentTs;
		Delay(1.2);
	}
	
	closeView(currentContext);

	i=error;
	// ---------------------------------------------------------------------

	*/
	
	
	// Make B&W color table for display
	for(i=0; i<256; i++) colorTable[i]=MakeColor(i, i, i);
	GetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_HEIGHT, &height);
	GetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_WIDTH,  &width);
	canvasRect = MakeRect(0, 0, height, width);
	
	displayRect = canvasRect;
	
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_RED);
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_MODE, VAL_COPY_MODE);
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_WIDTH,  1);
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_STYLE, VAL_SOLID);
			
	return 0;	
}


// This is invoked by selecting the Camera Settings (popup) menu option
void CVICALLBACK camSettings_CB (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	if (0==readCameraSettings()){
		croppedRect.left   = canvasRect.width  * (float)sideView.displayArea.left   / (float) sideView.camMode.maxWidth;
		croppedRect.width  = canvasRect.width  * (float)sideView.displayArea.width  / (float) sideView.camMode.maxWidth;
		croppedRect.top    = canvasRect.height * (float)sideView.displayArea.top    / (float) sideView.camMode.maxHeight;
		croppedRect.height = canvasRect.height * (float)sideView.displayArea.height / (float) sideView.camMode.maxHeight;

		displayRect        = canvasRect;
		currentContext     = contextSideCam;
		currentView        = &sideView;
		updatePanelControlsFromCurrentView();
		
		fadeInPanel(&camSetPanelFading);
		
		CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, displayLoopThread, NULL, &displayLoopThreadFunctionID);

	}
}

// Invoked by the done button
int CVICALLBACK doneCamSettings_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			stopAcquisitionLoop();
			saveCameraSettings();
			fadeOutPanel(&camSetPanelFading); 
			break;
	}
	return 0;
}

// Scheduled in its own thread to display the camera view
static int  CVICALLBACK  displayLoopThread(void *functionData){
	
	fc2Error error;
	currentContext = contextSideCam;
	currentView    = &sideView;


	// Open the view
	if (openView(currentContext, currentView)) {
		SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_RED);
		SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_STYLE, VAL_SOLID);
		SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_WIDTH, 2);
		SetCtrlAttribute(camSettingsPanel, CAM_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_TRANSPARENT);
		CanvasDrawText(camSettingsPanel, CAM_PANEL_CANVAS, "Camera not connected", 
			VAL_APP_META_FONT, MakeRect(80, 0, 40, 300), VAL_CENTER);
		return 8;
	}
	
	fc2Image image;
    error = fc2CreateImage( &image );
	if (error != FC2_ERROR_OK){
		writeToLog("cameraManagement::displayLoopThread - Error creating image");
		return 8;
	}
	error = fc2RetrieveBuffer (contextSideCam, &image);
	if (error != FC2_ERROR_OK){
		writeToLog("cameraManagement::displayLoopThread - Error retrieving first buffer");
		return 8;
	}
	
	NewBitmap(currentView->displayArea.width, 8, currentView->displayArea.width, currentView->displayArea.height, colorTable, image.pData, NULL, &bitMapID);
	CanvasDrawBitmap(camSettingsPanel, CAM_PANEL_CANVAS, bitMapID, VAL_ENTIRE_OBJECT, displayRect);
	
	// Enter display loop until save, discard or a camera button is pushed
	camSettingsAcquire = 1;
	while (camSettingsAcquire) {//displayImage(width);
		error = fc2RetrieveBuffer(currentContext, &image);
		if (error != FC2_ERROR_OK){
			writeToLog("cameraManagement::displayLoopThread - Error retrieving subsequent buffer");
			return 8;
		}
		SetBitmapData(bitMapID, currentView->displayArea.width, 8, colorTable, image.pData, NULL);
		CanvasDrawBitmap(camSettingsPanel, CAM_PANEL_CANVAS, bitMapID, VAL_ENTIRE_OBJECT, displayRect);
		ProcessSystemEvents();   // This is required so that controls can be operated
	}

	DiscardBitmap(bitMapID);
    error = fc2DestroyImage( &image );

	return 0;
}


int cameraTermination(){
	error = fc2DestroyContext(contextSideCam);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::cameraTermination - Error destroting context");  
	return 0;
}

int readCameraSettings(){
	int retCode=0;
	DWORD size = (DWORD) sizeof(struct cameraSettings);
	retCode    = RegQueryValueEx(petBoxRegKey, "SideView", NULL, NULL, (LPBYTE)&sideView, &size);
	if (retCode) writeToLog("cameraManagement::readCameraSettings - Error from RegQueryValueEx");
	
	char temp[256];
	sprintfWithLock(256, temp, "cameraManagement::readCameraSettings - rate:%d, H:%d, W:%d, top:%d, left:%d", sideView.frameRate, 
		sideView.displayArea.height, sideView.displayArea.width, sideView.displayArea.top, sideView.displayArea.left);
	writeToLog(temp);
	return retCode;
}

int saveCameraSettings(){
	int retCode=0;
	retCode  = RegSetValueEx(petBoxRegKey, "SideView",       0, REG_BINARY, (BYTE*)&sideView,    (DWORD) sizeof(struct cameraSettings)) ;
	if (retCode) writeToLog("cameraManagement::saveCameraSettings - Error from RegSetValueEx");
	return retCode;
}

static void stopAcquisitionLoop(void)
{

	if(camSettingsAcquire){
		camSettingsAcquire = 0;
		CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, displayLoopThreadFunctionID, OPT_TP_PROCESS_EVENTS_WHILE_WAITING );
		CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,displayLoopThreadFunctionID); 
		free(viewArea);
		error = fc2StopCapture(currentContext);
	}
	CanvasClear(camSettingsPanel, CAM_PANEL_CANVAS, VAL_ENTIRE_OBJECT);

}

int openView(fc2Context context, struct cameraSettings *view ){

	// Issue a reset for good measure
	error = fc2WriteRegister (contextSideCam, 0x00, 0x80000000); 
	if (error != FC2_ERROR_OK){
		writeToLog("cameraManagement::openView - Error resetting camera");
		return 8;
	}

	// Format 7: custom view (region-of-interest)
	fc2Format7ImageSettings  settings;
	settings.mode        = view->camMode.mode;
	settings.width       = view->displayArea.width;
	settings.height      = view->displayArea.height;
	settings.pixelFormat = view->camMode.pixFormat ;
	settings.offsetX     = view->displayArea.left;
	settings.offsetY     = view->displayArea.top ;
	
	error = fc2SetFormat7Configuration(context, &settings, 100.0); 
	if (error != FC2_ERROR_OK){
		char temp[512];
		sprintfWithLock(512,temp,"cameraManagement::openView - Error %d in SetFormat7, settings(%d, %d, %d, %d, %d, %d)" , error,
			settings.mode, settings.width, settings.height, settings.pixelFormat, settings.offsetX, settings.offsetY );
		writeToLog(temp);
		return 8;
	}
	error = fc2StartCapture(context);
	if (error != FC2_ERROR_OK){
		writeToLog("cameraManagement::openView - Error in start capture");
		return 8;
	}
	updateCameraSettingsFromView(context, view);
	
	return 0;
}

// Set values in the camera
static void updateCameraSettingsFromView(fc2Context context, struct cameraSettings *view ){ 
 	fc2Error error;
	fc2Property theProp;
	
	// Be sure to set the frame rate first; because it affects the shutter speed
	theProp.type           =  FC2_FRAME_RATE;
	error = fc2GetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error getting frame rate");
	theProp.valueA         =  view->frameRate ;
	error = fc2SetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error setting frame rate");
	
	theProp.type           =  FC2_SHUTTER;
	error = fc2GetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error getting shutter");
	theProp.valueA         =  view->shutter ;
	theProp.autoManualMode =  view->autoShutter;
	error = fc2SetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error setting shutter");
	
	theProp.type           =  FC2_BRIGHTNESS;
	error = fc2GetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error getting brightness");
	theProp.valueA         =  view->brightness ;
	theProp.autoManualMode =  view->autoBrightness;
	error = fc2SetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error setting brightness");
	
	theProp.type           =  FC2_GAIN;
	error = fc2GetProperty( contextSideCam, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error getting gain");
	theProp.valueA         =  view->gain ;
	theProp.autoManualMode =  view->autoGain;
	error = fc2SetProperty( contextSideCam, &theProp);
 	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateCameraSettingsFromView - Error setting gain");

}

fc2Error closeView(fc2Context context){
	fc2Error error;
	error = fc2StopCapture(context);
 	return error;
}


static void updateControlRangeFromCamera(fc2Context context){

	fc2PropertyInfo theProp;

	// Adjust parameter range
	theProp.type           =  FC2_BRIGHTNESS;
	error = fc2GetPropertyInfo(context, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateControlRangeFromCamera - Error getting property FC2_BRIGHTNESS");
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_BRIGHTNESS, ATTR_MIN_VALUE, theProp.min); 
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_BRIGHTNESS, ATTR_MAX_VALUE, theProp.max); 
	
	theProp.type           =  FC2_SHUTTER;
	error = fc2GetPropertyInfo(context, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateControlRangeFromCamera - Error getting property FC2_SHUTTER");
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_SHUTTER, ATTR_MIN_VALUE, theProp.min); 
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_SHUTTER, ATTR_MAX_VALUE, theProp.max); 

	theProp.type           =  FC2_GAIN;
	error = fc2GetPropertyInfo(context, &theProp);
	if (error != FC2_ERROR_OK) writeToLog("cameraManagement::updateControlRangeFromCamera - Error getting property FC2_GAIN");
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_GAIN, ATTR_MIN_VALUE, theProp.min); 
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_GAIN, ATTR_MAX_VALUE, theProp.max); 
	
}

// Update panel controls to reflect actual values
static void updatePanelControlsFromCurrentView(){
	// Display current values
	SetCtrlVal(camSettingsPanel, CAM_PANEL_BRIGHTNESS,  currentView->brightness);
	SetCtrlVal(camSettingsPanel, CAM_PANEL_AUTO_BRIGHT, currentView->autoBrightness);
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_BRIGHTNESS, ATTR_DIMMED, currentView->autoBrightness);
		
	SetCtrlVal(camSettingsPanel, CAM_PANEL_SHUTTER,  currentView->shutter);
	SetCtrlVal(camSettingsPanel, CAM_PANEL_AUTO_SHUT, currentView->autoShutter);
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_SHUTTER, ATTR_DIMMED, currentView->autoShutter);
	
	SetCtrlVal(camSettingsPanel, CAM_PANEL_GAIN,  currentView->gain);
	SetCtrlVal(camSettingsPanel, CAM_PANEL_AUTO_GAIN, currentView->autoGain);
	SetCtrlAttribute(camSettingsPanel, CAM_PANEL_GAIN, ATTR_DIMMED, currentView->autoGain);
}

int CVICALLBACK brightness_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_VAL_CHANGED:
			GetCtrlVal(camSettingsPanel, CAM_PANEL_BRIGHTNESS,  &currentView->brightness);
			updateCameraSettingsFromView(currentContext, currentView);
			break;
	}
	return 0;
}
int CVICALLBACK shutter_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_VAL_CHANGED:
			GetCtrlVal(camSettingsPanel, CAM_PANEL_SHUTTER,  &currentView->shutter);
			updateCameraSettingsFromView(currentContext, currentView);
			break;
	}
	return 0;
}
int CVICALLBACK gain_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_VAL_CHANGED:
			GetCtrlVal(camSettingsPanel, CAM_PANEL_GAIN,  &currentView->gain);
			updateCameraSettingsFromView(currentContext, currentView);
			break;
	}
	return 0;
}

int CVICALLBACK autoBright_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(camSettingsPanel, CAM_PANEL_AUTO_BRIGHT, &currentView->autoBrightness);
			SetCtrlAttribute(camSettingsPanel, CAM_PANEL_BRIGHTNESS, ATTR_DIMMED, currentView->autoBrightness);
			updateCameraSettingsFromView(currentContext, currentView);
			break;
	}
	return 0;
}

int CVICALLBACK autoShutter_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(camSettingsPanel, CAM_PANEL_AUTO_SHUT, &currentView->autoShutter);
			SetCtrlAttribute(camSettingsPanel, CAM_PANEL_SHUTTER, ATTR_DIMMED, currentView->autoShutter);
			updateCameraSettingsFromView(currentContext, currentView);
			break;
	}
	return 0;
}

int CVICALLBACK autoGain_CB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(camSettingsPanel, CAM_PANEL_AUTO_GAIN,  &currentView->autoGain);
			SetCtrlAttribute(camSettingsPanel, CAM_PANEL_GAIN, ATTR_DIMMED, currentView->autoGain);
			updateCameraSettingsFromView(currentContext, currentView);
			break;
	}
	return 0;
}


