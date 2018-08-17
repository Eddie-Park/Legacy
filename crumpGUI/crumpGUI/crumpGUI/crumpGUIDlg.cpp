
// crumpGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crumpGUI.h"
#include "crumpGUIDlg.h"
#include "afxdialogex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PET_BOX_ACQ_REG_KEY   "SOFTWARE\\Sofie Inc.\\PETBoxServer\\1.0\\Acquisition"

// CcrumpGUIDlg dialog

inline int round(float x) { return (floor(x + 0.5)); }

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
	strcat_s(string, string_t);
	strcat_s(string, "%");
	//::AfxGetApp()->m_pMainWnd->SetDlgItemText(IDC_STATIC_System_Information,(LPCTSTR)string);

    // Continue the file copy. It can also be stopped.
    return PROGRESS_CONTINUE;
}

CcrumpGUIDlg::CcrumpGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcrumpGUIDlg::IDD, pParent)
	, m_Ch1Offset(0)
	, m_Ch2Offset(0)
	, m_Ch3Offset(0)
	, m_Ch4Offset(0)
	, m_Ch5Offset(0)
	, m_Ch6Offset(0)
	, m_Ch7Offset(0)
	, m_Ch8Offset(0)
	, FPGA_Noise_Threshold(0)
	, FPGA_Event_Threshold(0)
	, FPGA_Blocking_Time(0)
	, FPGA_Integration_Time(0)
	, FPGA_Coinc_Window(0)
	, mAcqHours(0)
	, mAcqMins(0)
	, mAcqSecs(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcrumpGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_CH1, m_Ch1Offset);
	DDX_Text(pDX, EDIT_CH2, m_Ch2Offset);
	DDX_Text(pDX, EDIT_CH3, m_Ch3Offset);
	DDX_Text(pDX, EDIT_CH4, m_Ch4Offset);
	DDX_Text(pDX, EDIT_CH5, m_Ch5Offset);
	DDX_Text(pDX, EDIT_CH6, m_Ch6Offset);
	DDX_Text(pDX, EDIT_CH7, m_Ch7Offset);
	DDX_Text(pDX, EDIT_CH8, m_Ch8Offset);
	DDX_Text(pDX, EDIT_NOISE, FPGA_Noise_Threshold);
	DDX_Text(pDX, EDIT_EVENT, FPGA_Event_Threshold);
	DDX_Text(pDX, EDIT_BLOCKING, FPGA_Blocking_Time);
	DDX_Text(pDX, EDIT_INTEGRATION, FPGA_Integration_Time);
	DDX_Text(pDX, EDIT_COINC_TIMEWIN, FPGA_Coinc_Window);
	DDX_Control(pDX, IDC_MFCEDITBROWSE2, m_EditBrowseCtrl);
	DDX_Text(pDX, IDC_ACQ_HRS, mAcqHours);
	DDV_MinMaxInt(pDX, mAcqMins, 0, 100);
	DDX_Text(pDX, IDC_ACQ_MINS, mAcqMins);
	DDV_MinMaxInt(pDX, mAcqMins, 0, 60);
	DDX_Text(pDX, IDC_ACQ_SECS, mAcqSecs);
	DDV_MinMaxInt(pDX, mAcqSecs, 0, 60);
	DDX_Control(pDX, IDC_BTN_START, mBtnStart);
	DDX_Control(pDX, IDC_BTN_STOP, mBtnStop);
}

BEGIN_MESSAGE_MAP(CcrumpGUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_BN_CLICKED(IDC_BTN_START, &CcrumpGUIDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_RADIO_FREE_RUN, &CcrumpGUIDlg::OnBnClickedRadioFreeRun)
	ON_BN_CLICKED(IDC_RADIO_DURATION, &CcrumpGUIDlg::OnBnClickedRadioDuration)
	ON_BN_CLICKED(IDC_BTN_STOP, &CcrumpGUIDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CcrumpGUIDlg message handlers

BOOL CcrumpGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_VHS_Brd_Init_Successful = VHS_Brd_Init();

	InitFPGAOffsets();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CcrumpGUIDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CcrumpGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CcrumpGUIDlg::VHS_FPGA_Run(bool bRun)
{
	int res;
	if (bRun)
		//Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Run_Reg, 1+FPGA_Coinc_Window*2);
	//DLP replace with res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  1+FPGA_Coinc_Window*2 ); //run off
        res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  1+FPGA_Coinc_Window*2 );
	else
		//Vhsadac16_WriteFpgaReg(m_hVHSdevice, VHS_FPGA_Run_Reg, FPGA_Coinc_Window*2);
//DLP replace with res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,FPGA_Coinc_Window*2 ); //run off
     	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0, FPGA_Coinc_Window*2 );

	return;
}

bool CcrumpGUIDlg::VHS_Brd_Init()
{
 //DLP Here are the first steps in initialization of the Nutaq
	res = eapi_init();
	//dlp: initialize Nutaq stuff here
    hConn = NULL;
    hBusAccess = NULL;
    hRecPlay = NULL;
	channelc = 1;
	trigsourcec = 0;  
	clocksource = 0;
	char IpAddress[]="192.168.0.101";
	pVal=0;
	//dlp


 //DLP add board initialization of the Nutaq Mi125 here:
   //open ethernet connection here
   res  = BusAccess_Ethernet_OpenWithoutConnection(&hBusAccess, (int8_t *)IpAddress, 0);

   if (res)
   {
	   return false;
   }
   // get MAC address of target and host:
   res = iGetConnectionPeerMacAddr( hBusAccess, (unsigned char *)pDeviceMacAddr);
   if (res)
   {
	   return false;
   }
   res = iGetConnectionLocalMacAddr( hBusAccess, (unsigned char *)pHostMacAddr);
   if (res)
   {
	   return false;
   }
   // Initialized all needed mi125 boards to capture
   for (ik=1; ik <= (1 + (unsigned int)(channelc > 1)); ik++)  //channelc should be 2 for 2 boards
   {
	   res = InitMi125( hBusAccess,  ik, clocksource);
	   printf( "Res Init Mi125 should be 0. %u  \n",res);
   }
   //Reset RTDEx Core then RTDEx Open and Frame Gap Val
   res = RTDExResetCoreEth(hBusAccess,module_rtdex_eth_base);    
   res = RTDExOpenEth( &hConn,
		                (unsigned char *)pHostMacAddr,
						eHost,
						(unsigned char *)pDeviceMacAddr,
                        hBusAccess,
                        module_rtdex_eth_base, 
						0,
						eFromFpga,
                        eContinuous );  //eContinuous eSingle
   res = RTDExSetTxFrameGapValEth( hBusAccess, module_rtdex_eth_base, FRAME_GAP );
//
//  Here set the registers for the run 
//
   	// clear the run signal (from original code here)
	VHS_FPGA_Run(FALSE);
	//set FPGA registers
	unsigned int reg_value=0;

	reg_value=65536*FPGA_Event_Threshold+FPGA_Noise_Threshold;  
	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_2, VHS_FPGA_Threshold_Reg);

	reg_value=65536*FPGA_Blocking_Time+FPGA_Integration_Time;
	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,VHS_FPGA_Timing_Reg);

	reg_value=65536*(unsigned short(m_Ch4Offset))+(unsigned short(m_Ch3Offset));
	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_4, VHS_FPGA_CH34_OFFSET_Reg);

	reg_value=65536*(unsigned short(m_Ch2Offset))+(unsigned short(m_Ch1Offset));
	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_5, VHS_FPGA_CH12_OFFSET_Reg);

//DLP

	return TRUE;


}

void CcrumpGUIDlg::InitFPGAOffsets() 
{
	int disposition = 0;
	HKEY petBoxAcqRegKey;
	int FPGA_offset_value = 0;
	int size = 0;

	RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			(LPCWSTR)PET_BOX_ACQ_REG_KEY, 
			0, 
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&petBoxAcqRegKey,
			(LPDWORD)&disposition);	

	// Registry key already exists, then read it
	if (disposition==2) 
	{
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_1_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);
		m_Ch1Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_2_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		m_Ch2Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_3_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		m_Ch3Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_4_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		m_Ch4Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_5_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		m_Ch5Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_6_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		m_Ch6Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_7_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);	
		m_Ch7Offset = FPGA_offset_value;
		RegQueryValueEx(petBoxAcqRegKey, (LPCWSTR)"Channel_8_Offset",  NULL, NULL, (LPBYTE)&FPGA_offset_value,  (LPDWORD)&size);
		m_Ch8Offset = FPGA_offset_value;
	}
	else 
	{
		m_Ch1Offset = -7300;
		m_Ch2Offset = -7300;
		m_Ch3Offset = -7300;
		m_Ch4Offset = -7300;
		m_Ch5Offset = -7300;
		m_Ch6Offset = -7300;
		m_Ch7Offset = -7300;
		m_Ch8Offset = -7300;
	}

	FPGA_Noise_Threshold= 50;
	FPGA_Event_Threshold= 300;
	FPGA_Blocking_Time= 1000;
	FPGA_Integration_Time= 800;
	FPGA_Coinc_Window= 20;
}


////////////////////////////////////////////////////////////////////////////////
// Event Handlers
////////////////////////////////////////////////////////////////////////////////

void CcrumpGUIDlg::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码

	////////VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);

	// init buffer
	m_acqBuffer = (PUCHAR) malloc(numAcqBuffer*acqBufferSize);
	
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
	m_wTimerRes = 0;
	msInterval = 25;
	dwUser = 0;
	m_bPETMode_Coinc = TRUE;

	//char string_t[100];
	//_itoa_s(0,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_Coinc_Count_Rate,(LPCTSTR)string_t);

	//SetDlgItemText(IDC_STATIC_Coinc_Counts,(LPCTSTR)string_t);

	//FPGA_Noise_Threshold = GetDlgItemInt(IDC_EDIT_FPGA_Noise_Threshold,NULL,FALSE);
	//FPGA_Event_Threshold= GetDlgItemInt(IDC_EDIT_FPGA_Event_Threshold,NULL,FALSE);
	//FPGA_Blocking_Time= GetDlgItemInt(IDC_EDIT_FPGA_Blocking_Time,NULL,FALSE)/10;
	//FPGA_Integration_Time= GetDlgItemInt(IDC_EDIT_FPGA_Integration_Time,NULL,FALSE)/10;
	int FPGA_Coinc_Window_t = FPGA_Coinc_Window;
	float coinc_w_f;
	coinc_w_f = FPGA_Coinc_Window_t * 1.664;
	FPGA_Coinc_Window = round(coinc_w_f);

	//float coinc_w_f;
	
	//coinc_w_f=FPGA_Coinc_Window_t*1.664;
	//FPGA_Coinc_Window=Math::Round(coinc_w_f);


	//FPGA_ch1_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch1_offset,NULL,TRUE);
	//FPGA_ch2_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch2_offset,NULL,TRUE);
	//FPGA_ch3_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch3_offset,NULL,TRUE);
	//FPGA_ch4_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch4_offset,NULL,TRUE);
	//FPGA_ch5_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch5_offset,NULL,TRUE);
	//FPGA_ch6_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch6_offset,NULL,TRUE);
	//FPGA_ch7_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch7_offset,NULL,TRUE);
	//FPGA_ch8_offset= GetDlgItemInt(IDC_EDIT_FPGA_ch8_offset,NULL,TRUE);
	
	VHS_FPGA_Run(FALSE);

	unsigned int reg_value=0;
	
	reg_value=65536*FPGA_Event_Threshold+FPGA_Noise_Threshold;
	res = BusAccess_WriteReg( hBusAccess, VHS_FPGA_Threshold_Reg, reg_value);

	reg_value=65536*FPGA_Blocking_Time+FPGA_Integration_Time;
	res = BusAccess_WriteReg( hBusAccess, VHS_FPGA_Timing_Reg, reg_value);

	reg_value=65536*(unsigned short(m_Ch4Offset))+(unsigned short(m_Ch3Offset));
	res = BusAccess_WriteReg( hBusAccess, VHS_FPGA_CH34_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(m_Ch2Offset))+(unsigned short(m_Ch1Offset));
    res = BusAccess_WriteReg( hBusAccess, VHS_FPGA_CH12_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(m_Ch8Offset))+(unsigned short(m_Ch7Offset));
	res = BusAccess_WriteReg( hBusAccess, VHS_FPGA_CH78_OFFSET_Reg, reg_value);

	reg_value=65536*(unsigned short(m_Ch6Offset))+(unsigned short(m_Ch5Offset));
	res = BusAccess_WriteReg( hBusAccess, VHS_FPGA_CH56_OFFSET_Reg, reg_value);
	

	refreshFreq= 1000/msInterval; 
	
	if (m_acqType== Duration)
	{
		m_preset_Acq_Time = (mAcqHours * 60 * 60) + (mAcqMins * 60) + mAcqSecs; 
			
		if (m_preset_Acq_Time<=0)
		{
			MessageBox((LPCTSTR)"You must specify the acquisition Time!!",(LPCTSTR)"Error!",MB_OK);
			return;
		}
			
	}
	else if (m_acqType== FreeRun)
	{
		
	}
//	m_combo_Acqtime_Scale.SetCurSel(0);
//	unsigned int m_preset_Acq_Time;    /*in seconds*/
//	unsigned long m_preset_Acq_Counts; /**/

	// open file to save data
	errno_t err;
	//CString saveFilePath_t,saveStudyName_t;

	CString path;
	GetDlgItemText(IDC_MFCEDITBROWSE2, path);
	m_saveFileNameDest = path;
	//GetDlgItemText(IDC_EDIT_File_Path,saveFilePath_t);
	//GetDlgItemText(IDC_EDIT_Study_Name,saveStudyName_t);
	
	/*	
	if (saveStudyName_t=="" | saveFilePath_t=="")
	{ 
		MessageBox((LPCTSTR)"You must specify the directory and/or study name!!",(LPCTSTR)"Error!",MB_OK);
		return;
	}
	m_saveFileNameDest+=saveFilePath_t;
	m_saveFileNameDest+="\\";
	m_saveFileNameDest+=saveStudyName_t;
	m_saveFileNameDest+=".dat";
	*/
	if (path == "")
	{ 
		MessageBox((LPCTSTR)"You must specify the directory and/or study name!!",(LPCTSTR)"Error!",MB_OK);
		return;
	}
	//m_saveFileName+="E:\\PETBox\\StudyRawDataFirstStorage\\";
	//m_saveFileName+=saveStudyName_t;
	m_saveFileName += path;
	m_saveFileName += ".dat";
	
	
	if( (err  = _tfopen_s(&m_hSaveFileDest, m_saveFileNameDest,_T("r"))) ==0 )
	{
		fclose(m_hSaveFileDest);
		if (MessageBox((LPCTSTR)"Study name exists. Are you sure to overwrite the existing data file?",(LPCTSTR)"Warning!",MB_YESNO)==IDNO)
		{

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

	if( (err  = _tfopen_s(&m_hSaveFile,m_saveFileName,_T("wb"))) !=0 )
	{	 
		//char string[500];
		//GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string,500);
		//SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string);
		//SetDlgItemText(IDC_STATIC_System_Information,"Error openinging temporary data file!");
		return;
	}
	

	//set timer
	TIMECAPS tc;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
	{
		
		return ;
	}

	m_wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	timeBeginPeriod(m_wTimerRes); 
	
	mBtnStart.EnableWindow(FALSE);
	mBtnStop.EnableWindow(FALSE);
	


	//must put in rxtime for the new RTDEx (figure out later what this should be)
    rxtime = (int)(((long long)(nframe_used) * FRAME_GAP / FRAME_GAP_CLK) +
	        ((long long)(nframe_used) * ((FRAMESIZE + PACKET_OVERHEAD + ETH_FRAME_OVERHEAD) * 8) / ETH_SPEED) + 1);

	// Reset and Start RTDEX  should the rtdex start be put here?
	res = RTDExResetFifoEth(hConn); //is this the correct reset ? also have full reset in VHS_BRD_INIT()
	res = RTDExStart( hConn,FRAMESIZE,acqBufferSize); //Start the RTDEx here (outside loop)
	// set ADC Run State
	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  (unsigned int) 0 ); //run off
	res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  (unsigned int) 1 ); //acquisition on

	// start timer event
    m_timerID = timeSetEvent(msInterval,m_wTimerRes, (LPTIMECALLBACK) &AcqTimeCallBackProc,(DWORD_PTR) &dwUser, TIME_PERIODIC|TIME_KILL_SYNCHRONOUS ); 

    if(m_timerID==NULL)
	{	
		
		mBtnStart.EnableWindow(TRUE);
		mBtnStop.EnableWindow(FALSE);
		
		return;
	}
	// set the run signal
	VHS_FPGA_Run(TRUE);
	m_acqStTime=timeGetTime();
//	m_Button_Stop_Acq.SetState(TRUE);

}

void CcrumpGUIDlg::OnBnClickedRadioFreeRun()
{
	m_acqType = FreeRun;
}

void CcrumpGUIDlg::OnBnClickedRadioDuration()
{
	m_acqType = Duration;
}



LRESULT CcrumpGUIDlg::OnNewFrame(WPARAM param1, LPARAM param2)
{
	unsigned int fpgaStatus;
	unsigned int fpgaHalfFull=0;
	unsigned int numCountsRead=0;

	m_numCallBacks++;

	// poll ststus of halffull
	res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Acq_Status_Reg,  &pValue );
    fpgaStatus=pValue;
	fpgaHalfFull=fpgaStatus & 1;

	if (!fpgaHalfFull)
	{
		m_acqCurTime=timeGetTime();
		m_elapsedTime=(m_acqCurTime-m_acqStTime)/1000;
		if (m_acqType == Duration)
		{
				if (m_elapsedTime>=m_preset_Acq_Time)
				{	
					m_bPreset_time_reached=TRUE;
					OnBnClickedBtnStop();
					return 0;
				}
		}
		if (m_numCallBacks%refreshFreq==0)
			::PostMessage(this->m_hWnd,WM_REFRESHGUI,NULL,NULL);
		return 0;

	}

	//numCountsRead=Vhsadac16_RTDExReadBuffer(m_hVHSdevice,m_acqReadPointer, acqBufferSize);
	res = RTDExReceive( hConn,
					 m_acqReadPointer,
                     acqBufferSize, 
                     eRTDExWaitTimeout, rxtime * 1000 * 2);
	numCountsRead=res;

	m_numBufferRead++;
	

//	numCountsRead=acqBufferSize;
	
	
	if (numCountsRead!=acqBufferSize)
	{
		timeKillEvent(m_timerID);
		char string[1000]= "Read Bytes:";
		char string_t[500];
		_itoa_s(numCountsRead,string_t,500,10);
		strcat_s(string, string_t);
		int okay;
		//VHSADAC_RTDEX_ERROR_INFO errInfo; //what is this here?

		//okay = Vhsadac16_GetRTDExError(m_hVHSdevice, &errInfo);
        // Did we lost some frames
		res = RTDExGetRxFramesLostCountEth(hConn, &pVal);
		okay=res;
		if( okay == 0 )
			strcat_s(string, "  RTDEx Transfer Error!");
		
		else
			strcat_s(string, "  GET_RTDEX_ERROR IOCTL ERROR!!!");
		MessageBox((LPCTSTR)string,(LPCTSTR)"Error!",MB_OK);
		OnBnClickedBtnStop();
		return -1;
	}

	//Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);
	res = RTDExResetFifoEth(hConn);

	//updating pointer & index
	m_curBuffer+=1;
	m_curBuffer=m_curBuffer%numAcqBuffer;
	m_numRxBytes+=numCountsRead;

	m_acqReadPointer=m_acqBuffer+m_curBuffer*acqBufferSize;

	m_acqCurTime=timeGetTime();
	m_elapsedTime=(m_acqCurTime-m_acqStTime)/1000;
	if (m_acqType == Duration)
	{
			if (m_elapsedTime>=m_preset_Acq_Time)
			{	
				m_bPreset_time_reached=TRUE;
				OnBnClickedBtnStop();
				return 0;
			}
	}

	if(m_curBuffer%(numAcqBuffer/2)==0)
		::PostMessage(this->m_hWnd,WM_DUMPACQDATA,NULL,NULL);



	if (m_numCallBacks%refreshFreq==0)
		::PostMessage(this->m_hWnd,WM_REFRESHGUI,NULL,NULL);


	return 0;
}

LRESULT CcrumpGUIDlg::OnDumpAcqData(WPARAM param1, LPARAM param2)
{
	char string[100]= "Dumping Data to disk... number:";
	char string_t[100];

	//if (!m_bSave2File)
	//	return -1;

	//if (m_hSaveFile==NULL)
	//	return -1;

	unsigned int size_w=0;
	size_w=fwrite(m_acqWritePointer, sizeof(char), acqBufferSize*numAcqBuffer/2, m_hSaveFile);
	if (size_w!=acqBufferSize*numAcqBuffer/2)
	{
		timeKillEvent(m_timerID);
		char string[1000]= "Write Bytes:";
		char string_t[500];
		_itoa_s(size_w,string_t,500,10);
		strcat_s(string, string_t);
		strcat_s(string, ". Write file error!!");
		MessageBox((LPCTSTR)string,(LPCTSTR)"Error!",MB_OK);
		OnBnClickedBtnStop();
		return -1;
	}

	m_numSavedBytes+=size_w;

	m_dumpIndex++;
	m_dumpIndex=m_dumpIndex%2;

	m_acqWritePointer=m_acqBuffer+m_dumpIndex*(acqBufferSize*numAcqBuffer/2);

	m_numDiskWrite++;

	//_itoa_s(m_numDiskWrite,string_t,100,10);
	//strcat_s(string, string_t);
	//SetDlgItemText(IDC_STATIC_System_Information2,(LPCTSTR)string);

	return 0;
}

LRESULT CcrumpGUIDlg::OnRefreshGUI(WPARAM param1, LPARAM param2)
{

	char string[500]= "Acquisition Elapsed time(s):";
	char string_t[100];


	unsigned int fpgaRegValue;
	
	//update total numer of counts
	if (m_bPETMode_Coinc)
	{//fpgaRegValue=(Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Acq_Status_Reg))/4;
		res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Acq_Status_Reg,  &pValue );
		fpgaRegValue=res/4;
	}
	else
	{
		//fpgaRegValue=(Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Acq_Status_Reg))/2;
		res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Acq_Status_Reg,  &pValue );
		fpgaRegValue=res;
	}
	//_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_Coinc_Counts,(LPCTSTR)string_t);

	//update coincidence count rate
	//fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Coinc_CntRateReg);
		res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Coinc_CntRateReg,  &pValue );
		fpgaRegValue=res;
	//_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_Coinc_Count_Rate,(LPCTSTR)string_t);

	//update singles count rate
	//fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Top_Singles_CntRateReg);
		res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Top_Singles_CntRateReg,  &pValue );
		fpgaRegValue=res;
	//_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_TopHead_Count_Rate,(LPCTSTR)string_t);
	
	//fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Bottom_Singles_CntRateReg);
		res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Bottom_Singles_CntRateReg,  &pValue );
		fpgaRegValue=res;
	//_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_BottomHead_Count_Rate,(LPCTSTR)string_t);

	//fpgaRegValue=Vhsadac16_ReadFpgaReg(m_hVHSdevice,VHS_FPGA_Random_CntRateReg);
		res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Random_CntRateReg,  &pValue );
		fpgaRegValue=res;
	//_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_Random_Rate,(LPCTSTR)string_t);

	int rtime_t;
	if (m_acqType==FreeRun)
	{
//		SetDlgItemText(IDC_STATIC_Elapsed_Time_Name,"Elapsed Time(s): ");
		//_itoa_s(m_elapsedTime,string_t,100,10);
		//SetDlgItemText(IDC_STATIC_Elapsed_Time,(LPCTSTR)string_t);
	}
	else if (m_acqType==Duration)
	{	//SetDlgItemText(IDC_STATIC_Elapsed_Time_Name,"Remaining Time(s): ");
		rtime_t=m_preset_Acq_Time-m_elapsedTime;
		if (rtime_t>=0)
		{	
			//_itoa_s(rtime_t,string_t,100,10);
			//SetDlgItemText(IDC_STATIC_Elapsed_Time,(LPCTSTR)string_t);
			//SetDlgItemText(IDC_STATIC_Elapsed_Time2,(LPCTSTR)string_t);
		}
	}
	//update system log info
	_itoa_s(m_elapsedTime,string_t,100,10);
	strcat_s(string, string_t);
	strcat_s(string, "  Current Buffer Index:");
	_itoa_s(m_curBuffer,string_t,100,10);
	strcat_s(string, string_t);

/*	char string_s[500];
	GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
	SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
	SetDlgItemText(IDC_STATIC_System_Information,(LPCTSTR)string);
	*/

	return 0;
}

void CcrumpGUIDlg::OnBnClickedBtnStop()

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
	res=BusAccess_ReadReg(hBusAccess, VHS_FPGA_Acq_Status_Reg,  &pValue );
	fpgaPromptCounts=pValue/2;

	//dump the remaining data to disk
	//if (m_bSave2File)
	//{
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
				//char string_s[500];
				//GetDlgItemText(IDC_STATIC_System_Information,(LPTSTR)string_s,500);
				//SetDlgItemText(IDC_STATIC_System_Information2,(LPTSTR)string_s);
				//SetDlgItemText(IDC_STATIC_System_Information,"Error allocating buffer for left over events!");
				return ;
			}
            res = RTDExReceive( hConn,m_acqReadPointer,acqBufferSize,eRTDExWaitTimeout, rxtime * 1000 * 2);
			numCountsRead = res;
			if (numCountsRead!=bytes_diff)
			{
				_itoa_s(numCountsRead,string_t,100,10);
				strcat_s(string, string_t);
				int okay;
				//VHSADAC_RTDEX_ERROR_INFO errInfo;  /what is this here?
				res = RTDExGetRxFramesLostCountEth(hConn, &pVal);
				okay=res;
				//okay = Vhsadac16_GetRTDExError(m_hVHSdevice, &errInfo);
				if( okay == 0 )
					strcat_s(string, "  RTDEx Transfer Error!");
				
				else
					strcat_s(string, "  GET_RTDEX_ERROR IOCTL ERROR!!!");
				MessageBox((LPCTSTR)string,(LPCTSTR)"Error!",MB_OK);
				free(p_acqBuffer);
				return;
			}
			// clear the run signal
			VHS_FPGA_Run(FALSE);
			// reset ADC Run State
			//VHSADC_SetAcquisitionStatus(m_hVHSdevice,ACQUISITION_OFF);
			//Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);
			res = RTDExResetCoreEth(hBusAccess,module_rtdex_eth_base);//or the other RTDEx reset?
			//dumping to file
			fwrite(p_acqBuffer, sizeof(char), numCountsRead, m_hSaveFile);
			free(p_acqBuffer);
			m_numRxBytes+=numCountsRead;
		}
	//}
	// clear the run signal
	VHS_FPGA_Run(FALSE);
	// reset ADC Run State
	
	// Reset RTDEX
	//Vhsadac16_RTDExReset(m_hVHSdevice, TRUE, FALSE);
	res = RTDExResetCoreEth(hBusAccess,module_rtdex_eth_base);//or the other RTDEx reset?
	if (m_acqBuffer)
		free(m_acqBuffer);

	if(m_hSaveFile)
		fclose(m_hSaveFile);

	//update total numer of counts
	if (m_bPETMode_Coinc)
		fpgaRegValue=fpgaPromptCounts/2;
	else
		fpgaRegValue=fpgaPromptCounts;

	//m_static_elapsed_time2.ShowWindow(SW_HIDE);
	//clear time display
	//if (m_acqType==Duration && m_bPreset_time_reached)
	//	SetDlgItemText(IDC_STATIC_Elapsed_Time,"0");

	//SetDlgItemText(IDC_STATIC_Elapsed_Time2,"");

	//_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_Coinc_Counts,(LPCTSTR)string_t);

	//update coincidence count rate
	res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Coinc_CntRateReg,  &pValue );
	fpgaRegValue=pValue;
	_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_Coinc_Count_Rate,(LPCTSTR)string_t);

	//update singles count rate
	res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Top_Singles_CntRateReg,  &pValue );
    fpgaRegValue=pValue;
	_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_TopHead_Count_Rate,(LPCTSTR)string_t);
	
	
	res = BusAccess_ReadReg( hBusAccess, VHS_FPGA_Bottom_Singles_CntRateReg,  &pValue );
    fpgaRegValue=pValue;
	_itoa_s(fpgaRegValue,string_t,100,10);
	//SetDlgItemText(IDC_STATIC_BottomHead_Count_Rate,(LPCTSTR)string_t);

	mBtnStart.EnableWindow(TRUE);
	mBtnStop.EnableWindow(FALSE);
	//m_Button_Exit.EnableWindow(TRUE);
	//m_Button_About.EnableWindow(TRUE);

	time_t=(m_acqEndTime-m_acqStTime);
	_itoa_s(time_t,string_t,100,10);
	strcat_s(string, string_t);
	strcat_s(string, ". Callbacks:");
	_itoa_s(m_numCallBacks,string_t,100,10);
	strcat_s(string, string_t);
	strcat_s(string, ". Buffer read op:");
	_itoa_s(m_numBufferRead,string_t,100,10);
	strcat_s(string, string_t);
	strcat_s(string, ". Disk write op:");
	_itoa_s(m_numDiskWrite,string_t,100,10);
	strcat_s(string, string_t);

	strcat_s(string, ". Num. counts:");
	if (m_bPETMode_Coinc)
		_itoa_s(m_numRxBytes/8,string_t,100,10);
	else
		_itoa_s(m_numRxBytes/4,string_t,100,10);

	strcat_s(string, string_t);

	//SetDlgItemText(IDC_STATIC_System_Information2,(LPCTSTR)string);


	//if (m_bSave2File)
	//{
		// Copy the file.
		bool iRet;
		iRet=CopyFileEx( m_saveFileName,m_saveFileNameDest,	CopyProgressRoutine,0,FALSE,0 );
		if (iRet)
		{
			//SetDlgItemText(IDC_STATIC_System_Information,"Data successfully transferred.");
		}
		else
		{
			//SetDlgItemText(IDC_STATIC_System_Information,"Oooops...Transferring Data Failed. It's time to call us...");
		}
	//}

	return;

}
