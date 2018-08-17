
// crumpGUIDlg.h : header file
//

#pragma once
#include "Mmsystem.h"

#include "adp_types.h"
#include "adp_media_types.h"
#include "eapi.h"
#include "adp_buff_fct.h"
#include "adp_bus_access_media_types.h"
#include "adp_bus_access_types.h"
#include "adp_bus_access.h"

#include "adp_recplay_types.h"
#include "adp_recplay.h"
#include "adp_recplay_test_cnt.h"

#include "adp_rtdex_types.h"
#include "adp_rtdex_media.h"
#include "adp_rtdex_eth_types.h"
#include "adp_rtdex.h"
#include "adp_rtdex_access.h"
#include "adp_rtdex_eth.h"

#include "mac_utils.h"
#include "memory_map.h"
#include "mi125_settings.h"
#include "afxeditbrowsectrl.h"
#include "afxwin.h"

#define TIMER_INTERVAL_MS (25)

#define numAcqBuffer 64			//64*128KB=8MB
#define acqBufferSize 131072   //32*1024*4Byte

#define WM_NEWFRAME WM_APP+30
#define WM_REFRESHGUI WM_APP+31
#define WM_DUMPACQDATA WM_APP+40

#define TARGET_RESOLUTION 1         // 1-millisecond target resolution
#define FRAME_GAP 200000
#define FRAME_GAP_CLK 125000000
#define ETH_SPEED 1000000000
#define ETH_FRAME_OVERHEAD 38
#define PACKET_OVERHEAD 30
#ifdef NUSE_JUMBO_FRAME
#define FRAMESIZE 8000
#else
#define FRAMESIZE 1472
#endif

#define VHS_BT		BT_VHSADC16
#define VHS_ID		0
#define VHS_FPGA_Run_Reg VHSVIRTEX_USER_CTRL0
#define VHS_FPGA_Acq_Status_Reg TARGET_CUSTOMREGS_1
#define VHS_FPGA_Threshold_Reg TARGET_CUSTOMREGS_2
#define VHS_FPGA_Timing_Reg TARGET_CUSTOMREGS_3
#define VHS_FPGA_CH34_OFFSET_Reg TARGET_CUSTOMREGS_4
#define VHS_FPGA_CH12_OFFSET_Reg TARGET_CUSTOMREGS_5
#define VHS_FPGA_CH56_OFFSET_Reg TARGET_CUSTOMREGS_7
#define VHS_FPGA_CH78_OFFSET_Reg TARGET_CUSTOMREGS_6
#define VHS_FPGA_Top_Singles_CntRateReg TARGET_CUSTOMREGS_8
#define VHS_FPGA_Bottom_Singles_CntRateReg TARGET_CUSTOMREGS_9
#define VHS_FPGA_Coinc_CntRateReg TARGET_CUSTOMREGS_10
#define VHS_FPGA_Random_CntRateReg TARGET_CUSTOMREGS_11
// CcrumpGUIDlg dialog
class CcrumpGUIDlg : public CDialogEx
{
// Construction
public:
	CcrumpGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CRUMPGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	enum AcqTypes { FreeRun = 0, Duration = 1, };

// Implementation
protected:
	HICON m_hIcon;

	// Member Variables
	bool m_VHS_Brd_Init_Successful;

	adp_result_t res;
	adp_handle_rtdex_conn hConn;
	adp_handle_bus_access hBusAccess;
	adp_handle_recplay hRecPlay;

	unsigned int channelc;
	unsigned int trigsourcec;
	unsigned int clocksource;
	unsigned int ik;
	int rxtime;
	unsigned int pValue;
	unsigned int nframe_used;
	unsigned int pVal;

	char pHostMacAddr[6];
	char pDeviceMacAddr[6];


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void VHS_FPGA_Run(bool bRun);
	bool VHS_Brd_Init();
	void InitFPGAOffsets();

	CString m_saveFileName;			// The temporary file
	CString m_saveFileNameDest;		// The final output
	FILE* m_hSaveFile;
	FILE* m_hSaveFileDest;

	PUCHAR m_acqBuffer;
	PUCHAR m_acqWritePointer;
	PUCHAR m_acqReadPointer;
	unsigned int m_curBuffer;
	unsigned long m_numRxBytes;
	unsigned long m_numSavedBytes;
	unsigned long m_elapsedTime;
	unsigned long m_acqStTime;
	unsigned long m_acqCurTime;
	unsigned long m_acqEndTime;
	unsigned long m_numCallBacks;
	unsigned long m_numDiskWrite;
	unsigned long m_numBufferRead;
	unsigned int m_dumpIndex;
	bool m_bPETMode_Coinc;
public:
	AcqTypes m_acqType;
	// FPGA  Setup
	int m_Ch1Offset;
	int m_Ch2Offset;
	int m_Ch3Offset;
	int m_Ch4Offset;
	int m_Ch5Offset;
	int m_Ch6Offset;
	int m_Ch7Offset;
	int m_Ch8Offset;

	int FPGA_Noise_Threshold; 
	int FPGA_Event_Threshold; 
	int FPGA_Blocking_Time; 
	int FPGA_Integration_Time;
	int FPGA_Coinc_Window;  //in Q6.4 format

	unsigned int m_preset_Acq_Time;    /*in seconds*/
	bool m_bPreset_time_reached;
	UINT m_wTimerRes;
	DWORD dwUser;

	CMFCEditBrowseCtrl m_EditBrowseCtrl;
	
	int mAcqHours;
	int mAcqMins;
	int mAcqSecs;
	int msInterval;
	int refreshFreq;

	MMRESULT  m_timerID;
	CButton mBtnStart;
	CButton mBtnStop;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedRadioFreeRun();
	afx_msg void OnBnClickedRadioDuration();

	LRESULT OnNewFrame(WPARAM param1, LPARAM param2);
	LRESULT OnDumpAcqData(WPARAM param1, LPARAM param2);
	LRESULT OnRefreshGUI(WPARAM param1, LPARAM param2);
	
};
