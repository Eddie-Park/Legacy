// PETBoxControlUIDlg.h : header file
//
#include "stdafx.h"

#pragma once

#include <iostream>
#include <fstream>

#using <mscorlib.dll>
#using <System.dll>
//#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
//using namespace std;
using namespace System::IO;
using namespace System::Text;

#ifndef _VHS_ADC_V4
#define _VHS_ADC_V4

#include "vhsadac16_regs.h"
#include "vhsadc16ctrl.h"
#include "vhsadc16ctrl_v4.h"
#include "vhsdac16ctrl.h"
#include "vhsadac16_host_api.h"

#define VHS_BT		BT_VHSADC16
#define VHS_ID		0
#define VHS_FPGA_Run_Reg VHSVIRTEX_USER_CTRL0
#define VHS_FPGA_Acq_Status_Reg VHSVIRTEX_USER_CTRL1
#define VHS_FPGA_Threshold_Reg VHSVIRTEX_USER_CTRL2
#define VHS_FPGA_Timing_Reg VHSVIRTEX_USER_CTRL3
#define VHS_FPGA_CH34_OFFSET_Reg VHSVIRTEX_USER_CTRL4
#define VHS_FPGA_CH12_OFFSET_Reg VHSVIRTEX_USER_CTRL5
#define VHS_FPGA_CH56_OFFSET_Reg VHSVIRTEX_USER_CTRL7
#define VHS_FPGA_CH78_OFFSET_Reg VHSVIRTEX_USER_CTRL6
#define VHS_FPGA_Top_Singles_CntRateReg VHSVIRTEX_USER_CTRL8
#define VHS_FPGA_Bottom_Singles_CntRateReg VHSVIRTEX_USER_CTRL9
#define VHS_FPGA_Coinc_CntRateReg VHSVIRTEX_USER_CTRL10
#define VHS_FPGA_Random_CntRateReg VHSVIRTEX_USER_CTRL11


#endif
#include "afxwin.h"





//define acquisition buffer parameters
#define numAcqBuffer 64			//64*128KB=8MB
#define acqBufferSize 131072   //32*1024*4Byte


#define PET_BOX_ACQ_REG_KEY   "SOFTWARE\\Sofie Inc.\\PETBoxServer\\1.0\\Acquisition"

#define WM_NEWFRAME WM_APP+30
#define WM_REFRESHGUI WM_APP+31
#define WM_DUMPACQDATA WM_APP+40



class CMyButton : public CButton                                              
{                                                                                
// Construction
public:
	CMyButton();	// standard constructor
	virtual ~CMyButton();
                                                                                 
private:                                                                         
    COLORREF m_clrFore;    // foreground color                                  
    COLORREF m_clrBack;   //background color                                    
    CBrush m_brush;         //background brush  
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	DECLARE_MESSAGE_MAP()
};

                                                                     
    //WM_CTLCOLOR(=WM_CTLCOLOR)消息并指定新         


// CPETBoxControlUIDlg dialog
class CPETBoxControlUIDlg : public CDialog
{
// Construction
public:
	CPETBoxControlUIDlg(CWnd* pParent = NULL);	// standard constructor
	~CPETBoxControlUIDlg();

// Dialog Data
	enum { IDD = IDD_PETBOXCONTROLUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAboutbutton();
	LRESULT OnNewFrame(WPARAM param1, LPARAM param2);
	LRESULT OnRefreshGUI(WPARAM param1, LPARAM param2);
	LRESULT OnDumpAcqData(WPARAM param1, LPARAM param2);

// timer

public:
	
	// VHS stuff
	VHSADC16CONTEXT m_VHSContext;
	HANDLE m_hVHSdevice;
	bool m_VHS_Brd_Init_Sucessful;
	PUCHAR inBuffer[2];


	//VHS functions
	void VHS_FPGA_Run(bool bRun);
	bool VHS_Brd_Init();

	// Acq. & Acq.Buffer
	unsigned long m_numCallBacks;
	unsigned long m_numDiskWrite;
	unsigned long m_numBufferRead;

	PUCHAR m_acqBuffer;
	PUCHAR m_acqWritePointer;
	PUCHAR m_acqReadPointer;
	unsigned int m_curBuffer;
	unsigned long m_numRxBytes;
	unsigned long m_numSavedBytes;
	unsigned long m_elapsedTime;
	CString m_saveFileName;
	CString m_saveFileNameDest;

	FILE* m_hSaveFile;
	FILE* m_hSaveFileDest;
	bool  m_bSave2File;
	unsigned int m_dumpIndex;
	
	//acq mode
	bool m_bPETMode_Coinc;
	int  m_AcqMode;  /*0: free running; 1: acq. by time; 2: acq. by counts*/
	unsigned int m_preset_Acq_Time;    /*in seconds*/
	bool m_bPreset_time_reached;
	unsigned long m_preset_Acq_Counts; /**/

	//timer
	unsigned int	msInterval;    // timer period

	MMRESULT  m_timerID;
	UINT     m_wTimerRes;
	DWORD	  dwUser;
	unsigned long m_acqStTime,m_acqCurTime,m_acqEndTime;
	int refreshFreq;  

	
	/*FPGA configurations*/
	int FPGA_ch1_offset; 
	int FPGA_ch2_offset; 
	int FPGA_ch3_offset; 
	int FPGA_ch4_offset; 
	int FPGA_ch5_offset; 
	int FPGA_ch6_offset; 
	int FPGA_ch7_offset; 
	int FPGA_ch8_offset; 

	int FPGA_Noise_Threshold; 
	int FPGA_Event_Threshold; 
	int FPGA_Blocking_Time; 
	int FPGA_Integration_Time;
	int FPGA_Coinc_Window;  //in Q6.4 format

private:
	CFont  m_select_font;
	CFont  m_time_display_font;
	CFont  m_time_display_font2;
	CFont  m_unselect_font;
	CFont  m_pushButton_font;

public:
	CStatic m_Static_System_Info;
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButtonFpgaReset();
public:
	afx_msg void OnBnClickedButtonStartAcq();
public:
	afx_msg void OnBnClickedButtonTestAcq();
public:
	afx_msg void OnBnClickedButtonStopAcq();
public:
	afx_msg void OnEnChangeEditFilePath();
public:
	afx_msg void OnEnSetfocusEditFilePath();
public:
	afx_msg void OnBnClickedButtonChoosePath();
public:
	CButton m_Button_Start_Acq;
public:
	CButton m_Button_Stop_Acq;
public:
	CButton m_Button_Test_Acq;
public:
	CButton m_Button_FPGA_Apply_Setting;
public:
	CButton m_Button_FPGA_Reset;
public:
	afx_msg void OnBnClickedCheckSavetofile();
public:
	CButton m_Button_Exit;
public:
	CButton m_Radio_Coinc;
public:
	CButton m_Radio_Singles;
public:
	CButton m_Radio_Acq_Free_Running;
public:
	CButton m_Radio_Acq_by_Time;
public:
	CButton m_Radio_Acq_by_Counts;


public:
	afx_msg void OnBnClickedRadioCoinc();
public:
	afx_msg void OnBnClickedRadioSingles();
public:
	afx_msg void OnBnClickedRadioAcqFreeRunning();
public:
	afx_msg void OnBnClickedRadioAcqbyTime();
public:
	afx_msg void OnBnClickedRadioAcqbyCounts();
public:
	CEdit m_Edit_Acq_Preset_Time_in_Minutes;
public:
	CEdit m_Edit_Acq_Preset_Counts;
public:
	afx_msg void OnEnKillfocusEditAcqPresetTime();
public:
	afx_msg void OnEnKillfocusEditAcqPresetCounts();
public:
	CButton m_Button_About;
public:
	afx_msg void OnBnClickedButtonProgFpga();
public:
	afx_msg void OnBnClickedButtonChangeFpga();
public:
	CComboBox m_combo_Acqtime_Scale;
public:
	CStatic m_static_elapsed_time;
public:
	CStatic m_static_elapsed_time2;
public:
	CComboBox m_Combo_Timer_Interval;
};


