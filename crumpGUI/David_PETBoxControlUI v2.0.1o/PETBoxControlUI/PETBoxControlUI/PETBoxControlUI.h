// PETBoxControlUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "Mmsystem.h"
#define TARGET_RESOLUTION 1         // 1-millisecond target resolution


// CPETBoxControlUIApp:
// See PETBoxControlUI.cpp for the implementation of this class
//

class CPETBoxControlUIApp : public CWinApp
{
public:
	CPETBoxControlUIApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPETBoxControlUIApp theApp;