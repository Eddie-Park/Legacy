// PETBoxControlUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PETBoxControlUI.h"
#include "PETBoxControlUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 
void CALLBACK AcqTimeCallBackProc( UINT timerid, UINT msg, DWORD user, DWORD dw1, DWORD dw2 )
{
	PostMessage( ::AfxGetApp()->m_pMainWnd->m_hWnd, WM_NEWFRAME, NULL, NULL);
	return;
}

DWORD CALLBACK CopyProgressRoutine(
                          LARGE_INTEGER TotalFileSize,
                          LARGE_INTEGER TotalBytesTransferred,
                          LARGE_INTEGER StreamSize,
                          LARGE_INTEGER StreamBytesTransferred,
                          DWORD dwStreamNumber,
                          DWORD dwCallbackReason,
                          HANDLE hSourceFile,
                          HANDLE hDestinationFile,
                          LPVOID lpData )
{
    // Calculate the percentage here.
    unsigned int Percentage = unsigned int(( double(TotalBytesTransferred.QuadPart)/double(TotalFileSize.QuadPart) ) * 100);
	char string[500]= "Preparing raw data for post-processing...";
	char string_t[100];
	_itoa_s(Percentage,string_t,100,10);
	strcat(string, string_t);
	strcat(string, "%");
	::AfxGetApp()->m_pMainWnd->SetDlgItemText(IDC_STATIC_System_Information,(LPCTSTR)string);

    // Continue the file copy. It can also be stopped.
    return PROGRESS_CONTINUE;
}




CMyButton::CMyButton()
{
    //Initialize data members .                                                  
    m_clrFore =RGB(255 , 255 , 0) ;   // yellow text                            
    m_clrBack=RGB(0 , 0 , 255) ;    // blue background                          
    m_brush.CreateSolidBrush(m_clrBack );                                   
}

CMyButton::~CMyButton()
{
}

HBRUSH CMyButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor )                       
{                                                                                
    pDC->SetTextColor(m_clrFore);                                              
    pDC->SetBkColor(m_clrBack);                                               
                                                                                 
    return (HBRUSH) m_brush.GetSafeHandle();
}

BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CPETBoxControlUIDlg dialog


CPETBoxControlUIDlg::CPETBoxControlUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPETBoxControlUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_Mouse);
}

CPETBoxControlUIDlg::~CPETBoxControlUIDlg()
{
	if ( m_hVHSdevice )
	// Close driver
	CloseHandle(m_hVHSdevice);

}

void CPETBoxControlUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_System_Information, m_Static_System_Info);
	DDX_Control(pDX, IDC_BUTTON_Start_Acq, m_Button_Start_Acq);
	DDX_Control(pDX, IDC_BUTTON_Stop_Acq, m_Button_Stop_Acq);
	DDX_Control(pDX, IDC_BUTTON_Test_Acq, m_Button_Test_Acq);
	DDX_Control(pDX, IDC_BUTTON_FPGA_Apply_Settings, m_Button_FPGA_Apply_Setting);
	DDX_Control(pDX, IDC_BUTTON_FPGA_Reset, m_Button_FPGA_Reset);
	DDX_Control(pDX, IDOK, m_Button_Exit);
	DDX_Control(pDX, IDC_RADIO_Coinc, m_Radio_Coinc);  
	DDX_Control(pDX, IDC_RADIO_Singles, m_Radio_Singles);
	DDX_Control(pDX, IDC_RADIO_Acq_Free_Running, m_Radio_Acq_Free_Running);
	DDX_Control(pDX, IDC_RADIO_Acq_by_Time, m_Radio_Acq_by_Time);
	DDX_Control(pDX, IDC_RADIO_Acq_by_Counts, m_Radio_Acq_by_Counts);
	DDX_Control(pDX, IDC_EDIT_Acq_Preset_Time, m_Edit_Acq_Preset_Time_in_Minutes);
	DDX_Control(pDX, IDC_EDIT_Acq_Preset_Counts, m_Edit_Acq_Preset_Counts);
	DDX_Control(pDX, IDC_AboutBUTTON, m_Button_About);
	DDX_Control(pDX, IDC_COMBO_TIMESCALE, m_combo_Acqtime_Scale);
	DDX_Control(pDX, IDC_STATIC_Elapsed_Time, m_static_elapsed_time);
	DDX_Control(pDX, IDC_STATIC_Elapsed_Time2, m_static_elapsed_time2);
	DDX_Control(pDX, IDC_COMBO_TIMER_PERIOD, m_Combo_Timer_Interval);
}

BEGIN_MESSAGE_MAP(CPETBoxControlUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_AboutBUTTON, &CPETBoxControlUIDlg::OnBnClickedAboutbutton)
	ON_BN_CLICKED(IDOK, &CPETBoxControlUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FPGA_Reset, &CPETBoxControlUIDlg::OnBnClickedButtonFpgaReset)
	ON_BN_CLICKED(IDC_BUTTON_Start_Acq, &CPETBoxControlUIDlg::OnBnClickedButtonStartAcq)
	ON_BN_CLICKED(IDC_BUTTON_Test_Acq, &CPETBoxControlUIDlg::OnBnClickedButtonTestAcq)
	ON_BN_CLICKED(IDC_BUTTON_Stop_Acq, &CPETBoxControlUIDlg::OnBnClickedButtonStopAcq)
	ON_EN_CHANGE(IDC_EDIT_File_Path, &CPETBoxControlUIDlg::OnEnChangeEditFilePath)
	ON_EN_SETFOCUS(IDC_EDIT_File_Path, &CPETBoxControlUIDlg::OnEnSetfocusEditFilePath)
	ON_BN_CLICKED(IDC_BUTTON_Choose_Path, &CPETBoxControlUIDlg::OnBnClickedButtonChoosePath)
	ON_BN_CLICKED(IDC_CHECK_SavetoFile, &CPETBoxControlUIDlg::OnBnClickedCheckSavetofile)
	ON_MESSAGE(WM_NEWFRAME, OnNewFrame)
	ON_MESSAGE(WM_REFRESHGUI, OnRefreshGUI)
	ON_MESSAGE(WM_DUMPACQDATA, OnDumpAcqData)

	ON_BN_CLICKED(IDC_RADIO_Coinc, &CPETBoxControlUIDlg::OnBnClickedRadioCoinc)
	ON_BN_CLICKED(IDC_RADIO_Singles, &CPETBoxControlUIDlg::OnBnClickedRadioSingles)
	ON_BN_CLICKED(IDC_RADIO_Acq_Free_Running, &CPETBoxControlUIDlg::OnBnClickedRadioAcqFreeRunning)
	ON_BN_CLICKED(IDC_RADIO_Acq_by_Time, &CPETBoxControlUIDlg::OnBnClickedRadioAcqbyTime)
	ON_BN_CLICKED(IDC_RADIO_Acq_by_Counts, &CPETBoxControlUIDlg::OnBnClickedRadioAcqbyCounts)
	ON_EN_KILLFOCUS(IDC_EDIT_Acq_Preset_Time, &CPETBoxControlUIDlg::OnEnKillfocusEditAcqPresetTime)
	ON_EN_KILLFOCUS(IDC_EDIT_Acq_Preset_Counts, &CPETBoxControlUIDlg::OnEnKillfocusEditAcqPresetCounts)
	ON_BN_CLICKED(IDC_BUTTON_Prog_FPGA, &CPETBoxControlUIDlg::OnBnClickedButtonProgFpga)
	ON_BN_CLICKED(IDC_BUTTON_Change_FPGA, &CPETBoxControlUIDlg::OnBnClickedButtonChangeFpga)
END_MESSAGE_MAP()


// CPETBoxControlUIDlg message handlers

BOOL CPETBoxControlUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//Get Current DC
    CDC* pDC=GetDC();

    m_select_font.CreateFont(MulDiv(14, pDC->GetDeviceCaps(LOGPIXELSY),72),
                0,0,0,FW_BOLD,1,0,0,
                ANSI_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
                DRAFT_QUALITY,
                VARIABLE_PITCH |FF_SWISS, _T ("Arial") );

    m_time_display_font.CreateFont(MulDiv(14, pDC->GetDeviceCaps(LOGPIXELSY),72),
                0,0,0,FW_ULTRABOLD,0,0,0,
                ANSI_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
                DRAFT_QUALITY,
                VARIABLE_PITCH |FF_SWISS, _T ("Arial") );

    m_time_display_font2.CreateFont(MulDiv(54, pDC->GetDeviceCaps(LOGPIXELSY),72),
                0,0,0,FW_ULTRABOLD,0,0,0,
                ANSI_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
                DRAFT_QUALITY,
                VARIABLE_PITCH |FF_SWISS, _T ("Arial") );



	
    m_unselect_font.CreateFont(MulDiv(12, pDC->GetDeviceCaps(LOGPIXELSY),72),
                0,0,0,FW_NORMAL,0,0,0,
                ANSI_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
                DRAFT_QUALITY,
                VARIABLE_PITCH |FF_SWISS, _T ("Arial") );

	m_pushButton_font.CreateFont(MulDiv(18, pDC->GetDeviceCaps(LOGPIXELSY),72),
                0,0,0,FW_BOLD,1,0,0,
                ANSI_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
                DRAFT_QUALITY,
                VARIABLE_PITCH |FF_SWISS, _T ("Arial") );

    ReleaseDC(pDC);



	// TODO: Add extra initialization here
	// Init the VHS board here
	m_VHS_Brd_Init_Sucessful=VHS_Brd_Init();

	if (!m_VHS_Brd_Init_Sucessful)
	{
		//disable all functional buttons
/*		m_Button_FPGA_Apply_Setting.EnableWindow(FALSE);
		m_Button_FPGA_Reset.EnableWindow(FALSE);
		m_Button_Start_Acq.EnableWindow(FALSE);
		m_Button_Stop_Acq.EnableWindow(FALSE);
		m_Button_Test_Acq.EnableWindow(FALSE);
*/
	}


	m_Button_Stop_Acq.EnableWindow(FALSE);
	m_bSave2File=FALSE;
	m_acqStTime=0;
	m_acqCurTime=0;
	m_acqEndTime=0;
	m_curBuffer=0;
	m_numRxBytes=0;
	m_numSavedBytes=0;
	m_elapsedTime=0;
	m_hSaveFile=NULL;
	dwUser=0;
	m_dumpIndex=0;

	m_bPETMode_Coinc=TRUE;
	m_AcqMode=0;	 /*0: free running; 1: acq. by time; 2: acq. by counts*/
	m_preset_Acq_Time=0;    /*in minutes*/
	m_preset_Acq_Counts=0; /**/
	
	msInterval=80;   //80ms timer
	m_Button_Start_Acq.SetFont(&m_pushButton_font);
	m_Button_Stop_Acq.SetFont(&m_pushButton_font);
	refreshFreq=13;

	if (m_bPETMode_Coinc)
	{
		m_Radio_Coinc.SetCheck(BST_CHECKED);
		m_Radio_Coinc.SetFont(&m_select_font);
	}
	else
	{	m_Radio_Singles.SetCheck(BST_CHECKED);
		m_Radio_Singles.SetFont(&m_select_font);
	}
	m_Radio_Acq_Free_Running.SetCheck(BST_CHECKED);
	m_Radio_Acq_Free_Running.SetFont(&m_select_font);
	m_Edit_Acq_Preset_Time_in_Minutes.EnableWindow(FALSE);
	m_Edit_Acq_Preset_Counts.EnableWindow(FALSE);

	int  disposition;
	int  size;
	unsigned long pathtype=REG_SZ;
	int FPGA_offset_value;
	HKEY  petBoxAcqRegKey;
	RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			PET_BOX_ACQ_REG_KEY, 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&petBoxAcqRegKey,
			(LPDWORD)&disposition);								  
	
	// Registry key already exists, then read it
	if (disposition==2) {
		RegQueryValueEx(petBoxAcqRegKey, "Channel_1_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);
		FPGA_ch1_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_2_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		FPGA_ch2_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_3_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		FPGA_ch3_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_4_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		FPGA_ch4_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_5_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		FPGA_ch5_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_6_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		FPGA_ch6_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_7_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		FPGA_ch7_offset=FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, "Channel_8_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);
		FPGA_ch8_offset=FPGA_offset_value;
	}
	else {
	FPGA_ch1_offset= -5175;
	FPGA_ch2_offset= -5073;
	FPGA_ch3_offset= -5024;
	FPGA_ch4_offset= -5165;
	FPGA_ch5_offset= -5165;
	FPGA_ch6_offset= -5267;
	FPGA_ch7_offset= -5271;
	FPGA_ch8_offset= -5053;
	}
	FPGA_Noise_Threshold= 50;
	FPGA_Event_Threshold= 300;
	FPGA_Blocking_Time= 1000;
	FPGA_Integration_Time= 800;
	FPGA_Coinc_Window= 20;

	SetDlgItemInt(IDC_EDIT_FPGA_Noise_Threshold,FPGA_Noise_Threshold);
	SetDlgItemInt(IDC_EDIT_FPGA_Event_Threshold,FPGA_Event_Threshold);
	SetDlgItemInt(IDC_EDIT_FPGA_Blocking_Time,FPGA_Blocking_Time);
	SetDlgItemInt(IDC_EDIT_FPGA_Integration_Time,FPGA_Integration_Time);
	SetDlgItemInt(IDC_EDIT_FPGA_Coinc_Window,FPGA_Coinc_Window);
	SetDlgItemInt(IDC_EDIT_FPGA_ch1_offset,FPGA_ch1_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch2_offset,FPGA_ch2_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch3_offset,FPGA_ch3_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch4_offset,FPGA_ch4_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch5_offset,FPGA_ch5_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch6_offset,FPGA_ch6_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch7_offset,FPGA_ch7_offset);
	SetDlgItemInt(IDC_EDIT_FPGA_ch8_offset,FPGA_ch8_offset);

	m_Combo_Timer_Interval.SetCurSel(0);
	m_combo_Acqtime_Scale.SetCurSel(0);
	m_static_elapsed_time.SetFont(&m_time_display_font);
	m_static_elapsed_time2.SetFont(&m_time_display_font2);
	SetDlgItemText(IDC_STATIC_Elapsed_Time2,"");
	m_static_elapsed_time2.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPETBoxControlUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPETBoxControlUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}


}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPETBoxControlUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPETBoxControlUIDlg::VHS_FPGA_Run(bool bRun)
{

	if (bRun)
		Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Run_Reg, 1+FPGA_Coinc_Window*2);
	else
		Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Run_Reg, FPGA_Coinc_Window*2);

	return;
}

bool CPETBoxControlUIDlg::VHS_Brd_Init()
{
	int iStatus;
	// Open VHS-ADAC API driver
	iStatus = Vhsadac16_Open(&m_VHSContext);
	if (iStatus == -1)
	{
		SetDlgItemText(IDC_STATIC_System_Information,"No valid handle returned by Vhsadac16_Open routine  !!!");
		return FALSE;
	}
	m_hVHSdevice = Vhsadac16_getHandle( VHS_BT, VHS_ID );
	if ( m_hVHSdevice == INVALID_HANDLE_VALUE )
	{
		SetDlgItemText(IDC_STATIC_System_Information,"VHS Board not found!");
		return FALSE;
	}
	else
		SetDlgItemText(IDC_STATIC_System_Information,"Found VHS Board!");

	//load FPGA bitstream
	iStatus =Vhsadac16_Flash_Boot(m_hVHSdevice);
	if (iStatus != 0)
	{
		SetDlgItemText(IDC_STATIC_System_Information,"Oooops...Configuring FPGA from Flash Failed!!!");
		return FALSE;
	}

	//init clock
	VHSADC_HostControlClock(m_hVHSdevice,TRUE);
    VHSADC_SetClockSource(m_hVHSdevice, ONBOARD);
    VHSADC_DCMsReset(m_hVHSdevice);

	// clear the run signal
	VHS_FPGA_Run(FALSE);

	//Setting ADC Run State
	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);

	//set FPGA registers
	unsigned int reg_value=0;

	
	reg_value=65536*FPGA_Event_Threshold+FPGA_Noise_Threshold;
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Threshold_Reg, reg_value);

	reg_value=65536*FPGA_Blocking_Time+FPGA_Integration_Time;
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Timing_Reg, reg_value);

	reg_value=65536*(unsigned short(FPGA_ch4_offset))+(unsigned short(FPGA_ch3_offset));
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_CH34_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(FPGA_ch2_offset))+(unsigned short(FPGA_ch1_offset));
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_CH12_OFFSET_Reg, reg_value);


	// clear the run signal
	VHS_FPGA_Run(TRUE);

	//Setting ADC Run State
	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_ON);

	// clear the run signal
	VHS_FPGA_Run(FALSE);

	//Setting ADC Run State
	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);

	// Reset and Start RTDEX
	Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);


	return TRUE;


}


void CPETBoxControlUIDlg::OnBnClickedAboutbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CPETBoxControlUIDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();

	// Close driver
	CloseHandle(m_hVHSdevice);

}

void CPETBoxControlUIDlg::OnBnClickedButtonFpgaReset()
{
	if (m_VHS_Brd_Init_Sucessful)
	{
		// clear the run signal
		VHS_FPGA_Run(FALSE);

		// reset ADC Run State
		VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);

		char string[500];
		GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
		SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
		SetDlgItemText(IDC_STATIC_System_Information,"FPGA reset");
	}

}

void CPETBoxControlUIDlg::OnBnClickedButtonStartAcq()
{
	// TODO: 在此添加控件通知处理程序代码

	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);

	// init buffer
	m_acqBuffer = (PUCHAR) malloc(numAcqBuffer*acqBufferSize);
	if ( m_acqBuffer == NULL)
	{
		char string[500];
		GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
		SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
		SetDlgItemText(IDC_STATIC_System_Information,"Error allocating acquisition buffer!");
		return ;
	}
	// Init acqBuffer with 0x0
	memset(m_acqBuffer, 0, numAcqBuffer*acqBufferSize);
	m_acqWritePointer=m_acqBuffer;
	m_acqReadPointer=m_acqBuffer;

	m_curBuffer=0;
	m_numRxBytes=0;
	m_numSavedBytes=0;
	m_elapsedTime=0;
	m_acqStTime=0;
	m_acqCurTime=0;
	m_acqEndTime=0;
	m_numCallBacks=0;
	m_numDiskWrite=0;
	m_numBufferRead=0;
	m_dumpIndex=0;
	m_saveFileName="";
	m_saveFileNameDest="";
	m_bPreset_time_reached=FALSE;

	char string_t[100];
	_itoa_s(0,string_t,100,10);
	SetDlgItemText(IDC_STATIC_Coinc_Count_Rate,(LPCTSTR)string_t);

	SetDlgItemText(IDC_STATIC_Coinc_Counts,(LPCTSTR)string_t);

	FPGA_Noise_Threshold = GetDlgItemInt(IDC_EDIT_FPGA_Noise_Threshold,NULL,FALSE);
	FPGA_Event_Threshold= GetDlgItemInt(IDC_EDIT_FPGA_Event_Threshold,NULL,FALSE);
	FPGA_Blocking_Time= GetDlgItemInt(IDC_EDIT_FPGA_Blocking_Time,NULL,FALSE)/10;
	FPGA_Integration_Time= GetDlgItemInt(IDC_EDIT_FPGA_Integration_Time,NULL,FALSE)/10;
	int FPGA_Coinc_Window_t= GetDlgItemInt(IDC_EDIT_FPGA_Coinc_Window,NULL,FALSE);

	float coinc_w_f;
	
	coinc_w_f=FPGA_Coinc_Window_t*1.664;
	FPGA_Coinc_Window=Math::Round(coinc_w_f);


	FPGA_ch1_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch1_offset,NULL,TRUE);
	FPGA_ch2_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch2_offset,NULL,TRUE);
	FPGA_ch3_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch3_offset,NULL,TRUE);
	FPGA_ch4_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch4_offset,NULL,TRUE);
	FPGA_ch5_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch5_offset,NULL,TRUE);
	FPGA_ch6_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch6_offset,NULL,TRUE);
	FPGA_ch7_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch7_offset,NULL,TRUE);
	FPGA_ch8_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch8_offset,NULL,TRUE);
	
	VHS_FPGA_Run(FALSE);

	unsigned int reg_value=0;
	
	reg_value=65536*FPGA_Event_Threshold+FPGA_Noise_Threshold;
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Threshold_Reg, reg_value);

	reg_value=65536*FPGA_Blocking_Time+FPGA_Integration_Time;
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Timing_Reg, reg_value);

	reg_value=65536*(unsigned short(FPGA_ch4_offset))+(unsigned short(FPGA_ch3_offset));
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_CH34_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(FPGA_ch2_offset))+(unsigned short(FPGA_ch1_offset));
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_CH12_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(FPGA_ch8_offset))+(unsigned short(FPGA_ch7_offset));
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_CH78_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(FPGA_ch6_offset))+(unsigned short(FPGA_ch5_offset));
	Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_CH56_OFFSET_Reg, reg_value);
	SetDlgItemText(IDC_STATIC_Elapsed_Time2,"");


	unsigned int timer_int=m_Combo_Timer_Interval.GetCurSel();
	if (timer_int==0)
		msInterval=25;
	else if (timer_int==1) 
		msInterval=30;
	else if (timer_int==2) 
		msInterval=40;
	else if (timer_int==3) 
		msInterval=50;
	else if (timer_int==4) 
		msInterval=60;
	else if (timer_int==5) 
		msInterval=70;
	else if (timer_int==6) 
		msInterval=80;
	refreshFreq= 1000/msInterval; 
	
	if (m_AcqMode==1)
	{
			unsigned int tScale,tTime;
			tScale=m_combo_Acqtime_Scale.GetCurSel();
			tTime=GetDlgItemInt(IDC_EDIT_Acq_Preset_Time,NULL,FALSE);
			if (tScale==0)
				m_preset_Acq_Time=tTime*60;
			else
				m_preset_Acq_Time=tTime;

			if (m_preset_Acq_Time<=0)
			{
				MessageBox("You must specify the acquisition Time!!","Error!",MB_OK);
				return;
			}
			SetDlgItemText(IDC_STATIC_Elapsed_Time_Name,"Remaining Time(s): ");
			m_static_elapsed_time2.ShowWindow(SW_SHOWNOACTIVATE);
	}
	else if (m_AcqMode==2 | m_AcqMode==0)
	{
		SetDlgItemText(IDC_STATIC_Elapsed_Time_Name,"Elapsed Time(s): ");
		m_static_elapsed_time2.ShowWindow(SW_HIDE);
	}
//	m_combo_Acqtime_Scale.SetCurSel(0);
//	unsigned int m_preset_Acq_Time;    /*in seconds*/
//	unsigned long m_preset_Acq_Counts; /**/

	// open file to save data
	errno_t err;
	CString saveFilePath_t,saveStudyName_t;
	GetDlgItemText(IDC_EDIT_File_Path,saveFilePath_t);
	GetDlgItemText(IDC_EDIT_Study_Name,saveStudyName_t);
	if (m_bSave2File)
	{	
		if (saveStudyName_t=="" | saveFilePath_t=="")
		{ 
			MessageBox("You must specify the directory and/or study name!!","Error!",MB_OK);
			char string[500];
			GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
			SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
			SetDlgItemText(IDC_STATIC_System_Information,"Please specify and directory and/or study name before you start acquisition...");
			return;
		}
		m_saveFileNameDest+=saveFilePath_t;
		m_saveFileNameDest+="\\";
		m_saveFileNameDest+=saveStudyName_t;
		m_saveFileNameDest+=".dat";

		m_saveFileName+="E:\\PETBox\\StudyRawDataFirstStorage\\";
		m_saveFileName+=saveStudyName_t;
		m_saveFileName+=".dat";
		
		if( (err  = fopen_s(&m_hSaveFileDest,m_saveFileNameDest,"r")) ==0 )
		{
			fclose(m_hSaveFileDest);
			if (MessageBox("Study name exists. Are you sure to overwrite the existing data file?","Warning!",MB_YESNO)==IDNO)
			{
					char string[500];
					GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
					SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
					SetDlgItemText(IDC_STATIC_System_Information,"Please select a new study name and then start acquisition again...");
					return;
			}
			DeleteFile(m_saveFileNameDest);
		}
		if(m_hSaveFileDest)
			fclose(m_hSaveFileDest);

/*		if( (err  = fopen_s(&m_hSaveFile,m_saveFileName,"r")) ==0 )
		{
			fclose(m_hSaveFile);
			if (MessageBox("Study name exists. Are you sure to overwrite the existing data file?","Warning!",MB_YESNO)==IDNO)
			{
					char string[500];
					GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
					SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
					SetDlgItemText(IDC_STATIC_System_Information,"Please select a new study name and then start acquisition again...");
					return;
			}
			DeleteFile(m_saveFileName);
		}
		if(m_hSaveFile)
			fclose(m_hSaveFile);
*/

		if( (err  = fopen_s(&m_hSaveFile,m_saveFileName,"wb")) !=0 )
		{	 
			char string[500];
			GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
			SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
			SetDlgItemText(IDC_STATIC_System_Information,"Error openinging temporary data file!");
			return;
		}
	}

	//set timer
	TIMECAPS tc;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
	{
		char string[500];
		GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
		SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
		SetDlgItemText(IDC_STATIC_System_Information,"Windows MMTimer Error.");
		return ;
	}

	m_wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	timeBeginPeriod(m_wTimerRes); 
	SetDlgItemText(IDC_STATIC_System_Information2,"Windows MMTimer Started.");

	SetDlgItemText(IDC_STATIC_System_Information,"Starting Acquisition...");
	m_Button_Start_Acq.EnableWindow(FALSE);
	m_Button_Stop_Acq.EnableWindow(TRUE);
	m_Button_Exit.EnableWindow(FALSE);
	m_Button_About.EnableWindow(FALSE);
	// Reset and Start RTDEX
	Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);
	// set ADC Run State
	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_ON);

	// start timer event
    m_timerID = timeSetEvent(msInterval,m_wTimerRes, (LPTIMECALLBACK) &AcqTimeCallBackProc,(DWORD_PTR) &dwUser, TIME_PERIODIC|TIME_KILL_SYNCHRONOUS ); 

    if(m_timerID==NULL)
	{	
		char string[500];
		GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
		SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
		SetDlgItemText(IDC_STATIC_System_Information,"Error strating MMTimer!");
		m_Button_Start_Acq.EnableWindow(TRUE);
		m_Button_Stop_Acq.EnableWindow(FALSE);
		m_Button_Exit.EnableWindow(TRUE);
		m_Button_About.EnableWindow(TRUE);
		return;
	}
	// set the run signal
	VHS_FPGA_Run(TRUE);
	m_acqStTime=timeGetTime();
//	m_Button_Stop_Acq.SetState(TRUE);

}


void CPETBoxControlUIDlg::OnBnClickedButtonStopAcq()
{
	char string[500]= "Acquisition stopped. Total time(ms):";
	char string_t[100];
	unsigned int time_t;
	unsigned int fpgaRegValue;
	unsigned int fpgaPromptCounts,numCountsRead;
	PUCHAR p_acqBuffer;


	// TODO: 在此添加控件通知处理程序代码
	m_acqEndTime=timeGetTime();


	// kill timer first 
	if (m_timerID!=NULL)
		timeKillEvent(m_timerID);

	//end timer period
	timeEndPeriod(m_wTimerRes);

	fpgaPromptCounts=(Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Acq_Status_Reg))/2;
	
	//dump the remaining data to disk
	if (m_bSave2File)
	{
		int bytes_diff;
		//check if anything left in the PC buffer
		bytes_diff=m_numRxBytes-m_numSavedBytes;
		
		if (bytes_diff>0)
			fwrite(m_acqWritePointer, sizeof(char), bytes_diff, m_hSaveFile);
		
		//check if anything left in the FPGA buffer
		bytes_diff=fpgaPromptCounts*4-m_numRxBytes;

		if (bytes_diff>0)
		{
			p_acqBuffer = (PUCHAR) malloc(2*acqBufferSize);
			if ( p_acqBuffer == NULL)
			{
				char string_s[500];
				GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
				SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
				SetDlgItemText(IDC_STATIC_System_Information,"Error allocating buffer for left over events!");
				return ;
			}
			numCountsRead=Vhsadac16_RTDExReadBuffer(m_hVHSdevice,p_acqBuffer, bytes_diff);

			if (numCountsRead!=bytes_diff)
			{
				_itoa_s(numCountsRead,string_t,100,10);
				strcat(string, string_t);
				int okay;
				VHSADAC_RTDEX_ERROR_INFO errInfo;

				okay = Vhsadac16_GetRTDExError(m_hVHSdevice, &errInfo);
				if( okay == 0 )
					strcat(string, "  RTDEx Transfer Error!");
				
				else
					strcat(string, "  GET_RTDEX_ERROR IOCTL ERROR!!!");
				MessageBox(string,"Error!",MB_OK);
				free(p_acqBuffer);
				return;
			}
			// clear the run signal
			VHS_FPGA_Run(FALSE);
			// reset ADC Run State
			VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);
			Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);
			//dumping to file
			fwrite(p_acqBuffer, sizeof(char), numCountsRead, m_hSaveFile);
			free(p_acqBuffer);
			m_numRxBytes+=numCountsRead;
		}
	}
	// clear the run signal
	VHS_FPGA_Run(FALSE);
	// reset ADC Run State
	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);
	// Reset RTDEX
	Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);

	if (m_acqBuffer)
		free(m_acqBuffer);

	if(m_hSaveFile)
		fclose(m_hSaveFile);

	//update total numer of counts
	if (m_bPETMode_Coinc)
		fpgaRegValue=fpgaPromptCounts/2;
	else
		fpgaRegValue=fpgaPromptCounts;

	m_static_elapsed_time2.ShowWindow(SW_HIDE);
	//clear time display
	if (m_AcqMode==1 && m_bPreset_time_reached)
		SetDlgItemText(IDC_STATIC_Elapsed_Time,"0");

	SetDlgItemText(IDC_STATIC_Elapsed_Time2,"");

	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_Coinc_Counts,(LPCTSTR)string_t);

	//update coincidence count rate
	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL10);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_Coinc_Count_Rate,(LPCTSTR)string_t);

	//update singles count rate
	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL8);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_TopHead_Count_Rate,(LPCTSTR)string_t);
	
	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL9);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_BottomHead_Count_Rate,(LPCTSTR)string_t);

	m_Button_Start_Acq.EnableWindow(TRUE);
	m_Button_Stop_Acq.EnableWindow(FALSE);
	m_Button_Exit.EnableWindow(TRUE);
	m_Button_About.EnableWindow(TRUE);

	time_t=(m_acqEndTime-m_acqStTime);
	_itoa_s(time_t,string_t,100,10);
	strcat(string, string_t);
	strcat(string, ". Callbacks:");
	_itoa_s(m_numCallBacks,string_t,100,10);
	strcat(string, string_t);
	strcat(string, ". Buffer read op:");
	_itoa_s(m_numBufferRead,string_t,100,10);
	strcat(string, string_t);
	strcat(string, ". Disk write op:");
	_itoa_s(m_numDiskWrite,string_t,100,10);
	strcat(string, string_t);

	strcat(string, ". Num. counts:");
	if (m_bPETMode_Coinc)
		_itoa_s(m_numRxBytes/8,string_t,100,10);
	else
		_itoa_s(m_numRxBytes/4,string_t,100,10);

	strcat(string, string_t);

	SetDlgItemText(IDC_STATIC_System_Information2,(LPCTSTR)string);


	if (m_bSave2File)
	{
		// Copy the file.
		bool iRet;
		iRet=CopyFileEx( m_saveFileName,m_saveFileNameDest,	CopyProgressRoutine,0,FALSE,0 );
		if (iRet)
		{
			SetDlgItemText(IDC_STATIC_System_Information,"Data successfully transferred.");
		}
		else
		{
			SetDlgItemText(IDC_STATIC_System_Information,"Oooops...Transferring Data Failed. It's time to call us...");
		}
	}

	return;

}

void CPETBoxControlUIDlg::OnBnClickedButtonTestAcq()
{

	// Callback function for handling progress notification.

		return;
}


void CPETBoxControlUIDlg::OnEnChangeEditFilePath()
{
	// TODO:  在此添加控件通知处理程序代码
//	SetDlgItemText(IDC_STATIC_System_Information,"Path changed");

}

void CPETBoxControlUIDlg::OnEnSetfocusEditFilePath()
{
	// TODO: 在此添加控件通知处理程序代码
//	SetDlgItemText(IDC_STATIC_System_Information,"Path Clicked");
}

void CPETBoxControlUIDlg::OnBnClickedButtonChoosePath()
{
	// TODO: 在此添加控件通知处理程序代码

	  FolderBrowserDialog^ folderBrowserDialog1=gcnew System::Windows::Forms::FolderBrowserDialog;
      // Set the help text description for the FolderBrowserDialog.
      folderBrowserDialog1->Description = "Select the directory to save the listmode file (please choose a network disk)...";

      // Do not allow the user to create new files via the FolderBrowserDialog.
      folderBrowserDialog1->ShowNewFolderButton = TRUE;

      // Default to the My Documents folder.
	  folderBrowserDialog1->RootFolder =Environment::SpecialFolder::MyComputer; 
	  folderBrowserDialog1->SelectedPath= "N:\\PETbox\\data";
	  if (folderBrowserDialog1->ShowDialog()== DialogResult::OK)
	  {
		  String ^saveFilePath;
		  saveFilePath=folderBrowserDialog1->SelectedPath;
		  
		  IntPtr ptr = Marshal::StringToHGlobalAnsi(saveFilePath);
		  LPCTSTR string = reinterpret_cast<LPCTSTR>(ptr.ToPointer());

		  if(saveFilePath != "")
		  {
			  SetDlgItemText(IDC_EDIT_File_Path,string);
		  }
		  Marshal::FreeHGlobal(ptr);
	  }


/*      SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;
      saveFileDialog1->Filter = "dat files (*.dat)|*.dat";
      saveFileDialog1->FilterIndex = 2;
      saveFileDialog1->RestoreDirectory = true;
	  saveFileDialog1->ShowDialog();
	  String ^saveFileName;
	  saveFileName=saveFileDialog1->FileName;
	  
	  IntPtr ptr = Marshal::StringToHGlobalAnsi(saveFileName);
	  LPCTSTR string = reinterpret_cast<LPCTSTR>(ptr.ToPointer());

	  if(saveFileName != "")
	  {
		  SetDlgItemText(IDC_EDIT_File_Path,string);
	  }
	  Marshal::FreeHGlobal(ptr);
*/
}


void CPETBoxControlUIDlg::OnBnClickedCheckSavetofile()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bSave2File)
		m_bSave2File=FALSE;
	else
		m_bSave2File=TRUE;

}

LRESULT CPETBoxControlUIDlg::OnNewFrame(WPARAM param1, LPARAM param2)
{
	unsigned int fpgaStatus;
	unsigned int fpgaHalfFull=0;
	unsigned int numCountsRead=0;

	m_numCallBacks++;

	// poll ststus of halffull
	fpgaStatus=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Acq_Status_Reg);
	
	fpgaHalfFull=fpgaStatus & 1;

	if (!fpgaHalfFull)
	{
		m_acqCurTime=timeGetTime();
		m_elapsedTime=(m_acqCurTime-m_acqStTime)/1000;
		if (m_AcqMode==1)
		{
				if (m_elapsedTime>=m_preset_Acq_Time)
				{	
					m_bPreset_time_reached=TRUE;
					OnBnClickedButtonStopAcq();
					return 0;
				}
		}
		if (m_numCallBacks%refreshFreq==0)
			::PostMessage(this->m_hWnd,WM_REFRESHGUI,NULL,NULL);
		return 0;

	}

	numCountsRead=Vhsadac16_RTDExReadBuffer(m_hVHSdevice,m_acqReadPointer, acqBufferSize);
	m_numBufferRead++;
	

//	numCountsRead=acqBufferSize;
	
	
	if (numCountsRead!=acqBufferSize)
	{
		timeKillEvent(m_timerID);
		char string[1000]= "Read Bytes:";
		char string_t[500];
		_itoa_s(numCountsRead,string_t,500,10);
		strcat(string, string_t);
		int okay;
		VHSADAC_RTDEX_ERROR_INFO errInfo;

		okay = Vhsadac16_GetRTDExError(m_hVHSdevice, &errInfo);
		if( okay == 0 )
			strcat(string, "  RTDEx Transfer Error!");
		
		else
			strcat(string, "  GET_RTDEX_ERROR IOCTL ERROR!!!");
		MessageBox(string,"Error!",MB_OK);
		OnBnClickedButtonStopAcq();
		return -1;
	}

	Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);
	
	//updating pointer & index
	m_curBuffer+=1;
	m_curBuffer=m_curBuffer%numAcqBuffer;
	m_numRxBytes+=numCountsRead;

	m_acqReadPointer=m_acqBuffer+m_curBuffer*acqBufferSize;

	m_acqCurTime=timeGetTime();
	m_elapsedTime=(m_acqCurTime-m_acqStTime)/1000;
	if (m_AcqMode==1)
	{
			if (m_elapsedTime>=m_preset_Acq_Time)
			{	
				m_bPreset_time_reached=TRUE;
				OnBnClickedButtonStopAcq();
				return 0;
			}
	}

	if(m_curBuffer%(numAcqBuffer/2)==0)
		::PostMessage(this->m_hWnd,WM_DUMPACQDATA,NULL,NULL);



	if (m_numCallBacks%refreshFreq==0)
		::PostMessage(this->m_hWnd,WM_REFRESHGUI,NULL,NULL);


	return 0;
}

LRESULT CPETBoxControlUIDlg::OnDumpAcqData(WPARAM param1, LPARAM param2)
{
	char string[100]= "Dumping Data to disk... number:";
	char string_t[100];

	if (!m_bSave2File)
		return -1;

	if (m_hSaveFile==NULL)
		return -1;

	unsigned int size_w=0;
	size_w=fwrite(m_acqWritePointer, sizeof(char), acqBufferSize*numAcqBuffer/2, m_hSaveFile);
	if (size_w!=acqBufferSize*numAcqBuffer/2)
	{
		timeKillEvent(m_timerID);
		char string[1000]= "Write Bytes:";
		char string_t[500];
		_itoa_s(size_w,string_t,500,10);
		strcat(string, string_t);
		strcat(string, ". Write file error!!");
		MessageBox(string,"Error!",MB_OK);
		OnBnClickedButtonStopAcq();
		return -1;
	}

	m_numSavedBytes+=size_w;

	m_dumpIndex++;
	m_dumpIndex=m_dumpIndex%2;

	m_acqWritePointer=m_acqBuffer+m_dumpIndex*(acqBufferSize*numAcqBuffer/2);

	m_numDiskWrite++;

	_itoa_s(m_numDiskWrite,string_t,100,10);
	strcat(string, string_t);
	SetDlgItemText(IDC_STATIC_System_Information2,(LPCTSTR)string);

	return 0;
}


LRESULT CPETBoxControlUIDlg::OnRefreshGUI(WPARAM param1, LPARAM param2)
{

	char string[500]= "Acquisition Elapsed time(s):";
	char string_t[100];


	unsigned int fpgaRegValue;
	
	//update total numer of counts
	if (m_bPETMode_Coinc)
		fpgaRegValue=(Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Acq_Status_Reg))/4;
	else
		fpgaRegValue=(Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Acq_Status_Reg))/2;

	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_Coinc_Counts,(LPCTSTR)string_t);

	//update coincidence count rate
	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL10);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_Coinc_Count_Rate,(LPCTSTR)string_t);

	//update singles count rate
	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL8);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_TopHead_Count_Rate,(LPCTSTR)string_t);
	
	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL9);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_BottomHead_Count_Rate,(LPCTSTR)string_t);

	fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHSVIRTEX_USER_CTRL11);
	_itoa_s(fpgaRegValue,string_t,100,10);
	SetDlgItemText(IDC_STATIC_Random_Rate,(LPCTSTR)string_t);

	int rtime_t;
	if (m_AcqMode==0 | m_AcqMode==2)
	{
//		SetDlgItemText(IDC_STATIC_Elapsed_Time_Name,"Elapsed Time(s): ");
		_itoa_s(m_elapsedTime,string_t,100,10);
		SetDlgItemText(IDC_STATIC_Elapsed_Time,(LPCTSTR)string_t);
	}
	else if (m_AcqMode==1)
	{	//SetDlgItemText(IDC_STATIC_Elapsed_Time_Name,"Remaining Time(s): ");
		rtime_t=m_preset_Acq_Time-m_elapsedTime;
		if (rtime_t>=0)
		{	_itoa_s(rtime_t,string_t,100,10);
			SetDlgItemText(IDC_STATIC_Elapsed_Time,(LPCTSTR)string_t);
			SetDlgItemText(IDC_STATIC_Elapsed_Time2,(LPCTSTR)string_t);
		}
	}
	//update system log info
	_itoa_s(m_elapsedTime,string_t,100,10);
	strcat(string, string_t);
	strcat(string, "  Current Buffer Index:");
	_itoa_s(m_curBuffer,string_t,100,10);
	strcat(string, string_t);

/*	char string_s[500];
	GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
	SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
*/	SetDlgItemText(IDC_STATIC_System_Information,(LPCTSTR)string);


	return 0;
}


void CPETBoxControlUIDlg::OnBnClickedRadioCoinc()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Radio_Singles.SetFont(&m_unselect_font);
	m_Radio_Coinc.SetFont(&m_select_font);
	m_bPETMode_Coinc=TRUE;
	return;
}


void CPETBoxControlUIDlg::OnBnClickedRadioSingles()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Radio_Singles.SetFont(&m_select_font);
	m_Radio_Coinc.SetFont(&m_unselect_font);
	m_bPETMode_Coinc=FALSE;
	return;
}

void CPETBoxControlUIDlg::OnBnClickedRadioAcqFreeRunning()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Radio_Acq_Free_Running.SetFont(&m_select_font);
	m_Radio_Acq_by_Time.SetFont(&m_unselect_font);
	m_Radio_Acq_by_Counts.SetFont(&m_unselect_font);
	m_Edit_Acq_Preset_Time_in_Minutes.EnableWindow(FALSE);
	m_Edit_Acq_Preset_Counts.EnableWindow(FALSE);

	m_AcqMode=0;
}

void CPETBoxControlUIDlg::OnBnClickedRadioAcqbyTime()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Radio_Acq_Free_Running.SetFont(&m_unselect_font);
	m_Radio_Acq_by_Time.SetFont(&m_select_font);
	m_Radio_Acq_by_Counts.SetFont(&m_unselect_font);
	m_Edit_Acq_Preset_Time_in_Minutes.EnableWindow(TRUE);
	m_Edit_Acq_Preset_Counts.EnableWindow(FALSE);
	m_AcqMode=1;
}

void CPETBoxControlUIDlg::OnBnClickedRadioAcqbyCounts()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Radio_Acq_Free_Running.SetFont(&m_unselect_font);
	m_Radio_Acq_by_Time.SetFont(&m_unselect_font);
	m_Radio_Acq_by_Counts.SetFont(&m_select_font);
	m_Edit_Acq_Preset_Time_in_Minutes.EnableWindow(FALSE);
	m_Edit_Acq_Preset_Counts.EnableWindow(TRUE);
	m_AcqMode=2;
}


void CPETBoxControlUIDlg::OnEnKillfocusEditAcqPresetTime()
{
	// TODO: 在此添加控件通知处理程序代码
//	m_preset_Acq_Time=GetDlgItemInt(IDC_EDIT_Acq_Preset_Time,NULL,FALSE)*60;    /*in seconds*/
//	unsigned long m_preset_Acq_Counts; /**/
}

void CPETBoxControlUIDlg::OnEnKillfocusEditAcqPresetCounts()
{
	// TODO: 在此添加控件通知处理程序代码
//	m_preset_Acq_Counts=GetDlgItemInt(IDC_EDIT_Acq_Preset_Time,NULL,FALSE);    /*in minutes*/

}

void CPETBoxControlUIDlg::OnBnClickedButtonProgFpga()
{
	// TODO: 在此添加控件通知处理程序代码
	  UCHAR bitFileName[200]="";

	  Stream^ myStream;
	  OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
      openFileDialog1->Filter = "bitstream files (*.bit)|*.bit";
      openFileDialog1->FilterIndex = 2;
      openFileDialog1->RestoreDirectory = true;

      if ( openFileDialog1->ShowDialog() != System::Windows::Forms::DialogResult::OK )
		return;

	 if ( (myStream = openFileDialog1->OpenFile()) == nullptr )
     {
		MessageBox("Bitstream file is empty","Error!",MB_OK);
        return;
     }
     myStream->Close();

	  String ^bitFileNameS;
	  bitFileNameS=openFileDialog1->FileName;
		
	  pin_ptr<const wchar_t> str2 = PtrToStringChars(bitFileNameS);
  	  
// Convert to a char*
	 size_t origsize = wcslen(str2) + 1;
	 size_t convertedChars = 0;
	 wcstombs_s(&convertedChars, (char*)bitFileName, origsize, str2, _TRUNCATE);

	char string_s[500];
	GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
	SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
	SetDlgItemText(IDC_STATIC_System_Information,"Programming FPGA Now! Please Wait...");

	long iStatus;
	// Open VHS-ADAC API driver
	iStatus = Vhsadac16_FpgaLoad(m_hVHSdevice,(UCHAR*)bitFileName);

	if (iStatus!=0)
	{ 
		MessageBox("Programming FPGA Failed!!","Error!",MB_OK);
		char string_s[500];
		GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
		SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
		SetDlgItemText(IDC_STATIC_System_Information,"oooops...Programming FPGA Failed. Call us...");
		return;
	}
	

	//init clock
	VHSADC_HostControlClock(m_hVHSdevice,TRUE);
    VHSADC_SetClockSource(m_hVHSdevice, ONBOARD);
    VHSADC_DCMsReset(m_hVHSdevice);

	VHS_FPGA_Run(FALSE);
	// reset ADC Run State
	VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);
	// Reset RTDEX
	Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);

	char string_m[500];
	GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
	SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
	SetDlgItemText(IDC_STATIC_System_Information,"FPGA Programmed Successfully!");


}

void CPETBoxControlUIDlg::OnBnClickedButtonChangeFpga()
{
	// TODO: 在此添加控件通知处理程序代码

}
