/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SignalMaster                                              *
 *  Processor   : Host processor                                            *
 *  File        : smmgr.h                                                   *
 *  Description : SignalMaster Application Programming Interface (API).     *
 *                                                                          *
 ****************************************************************************
 *
 * "$Revision: 1.2 $"
 * "$Name: 1.2 $"
 * "$Date: 2008/06/20 19:23:37 $"
 * "$Author: francois.blackburn $"
 *
 ****************************************************************************/

#ifndef _SMMGR_H_
#define _SMMGR_H_

#include "smtypes.h"

#ifndef commHandle
typedef void * commHandle;
#endif

/****************************************************************************
 *                              Definitions                                 *
 ****************************************************************************/

#define SMAPI_VERSION_MAJOR          2
#define SMAPI_VERSION_MINOR          1
#define SMAPI_VERSION_BUILD          1
#define SMAPI_VERSION_DESCRIPTOR     "SignalMaster API"

// Version types
#define VERSION_HOSTSMAPI		0x0001
#define VERSION_HOSTCOMMDRV		0x0002  // Host Communication Driver
#define VERSION_HW				0x0100
#define VERSION_FPGAFIRMWARE	0x0200
#define VERSION_FPGACURRENT		0x0201
#define VERSION_MCUFIRMWARE		0x0300
#define VERSION_MCUCOMMDRV		0x0301  // Embedded Communication Driver
#define VERSION_DSPFIRMWARE		0x0400
#define VERSION_DSPCURRENT		0x0401

// Ethernet Configuration IP addresses
#define IP_LOCAL				0x0001
#define IP_NETMASK				0x0002
#define IP_ROUTER				0x0003
#define IP_DNS1					0x0004
#define IP_DNS2					0x0005
#define IP_SYSLOG				0x0006

// Ethernet Configuration Options
#define OPTION_DHCP				0x0001
#define OPTION_HOST_SERIAL		0x0002
#define OPTION_HOST_IP			0x0003
#define OPTION_AUTODETECT		0x0004

// File Formats
#define FILE_SREC					 1
#define FILE_SHARCLDR				 2
#define FILE_BINARY					 3
#define FILE_COFF					 4

// Device types
#define SMANY					0x0000
#define SM56301					0x0100
#define SMSHARC					0x0200
#define SMTMSC32				0x0300
#define SMELAN520				0x0400
#define SMC6XX					0x0401
#define SM21160					0x0402
#define SMQUADC6XX		  0x0403
#define SMDUAL          0x0404
#define SMGCS					0x0500
#define SWAVE					0x0501
#define SDREVMC6446				0x0600
#define SDREVMTMS470			0x0700
#define SMWIN32					0x1000
#define SMUNKNOWN				0xFFFF

// User types
#define USER_SMSHARC				 1
#define USER_SMTMSC32				 2
#define USER_SMC6XX					 3
#define USER_SMELAN					 4
#define USER_SMWIN32				 5
#define USER_SM21160				 6
#define USER_SMQUADC6XX			 7
#define USER_SMFPGA					 8
#define USER_SWAVE					 9
#define USER_SDREVMC6446			10
#define USER_SDREVMTMS470			11
#define USER_SMDUAL			     12

/****************************************************************************
 *                             Data Structures                              *
 ****************************************************************************/

typedef struct
{
	WORD major;
	WORD minor;
	WORD build;
	char descriptor[128];
} smVersion;

typedef struct
{
	WORD scale;
#define SCALE_LINEAR	0
#define SCALE_DB		1
	WORD steps;
	double minimum;
	double maximum;
} smSlider;

typedef struct
{
	int	entries;
	char **pList;
} smList;

typedef struct
{
	smList	inputList;
	smList	sfreqList;
	smSlider	volumeSlider;
} smAdcCapabilities;

typedef struct
{
	WORD input;
	WORD sfreq;
	WORD volume;
} smAdcParameters;

typedef struct
{
	smAdcParameters	params;
	WORD clipping;
} smAdcState;

typedef struct
{
	smList sfreqList;
	smSlider volumeSlider;
} smDacCapabilities;

typedef struct
{
	WORD sfreq;
	WORD mute;
	WORD volume;
} smDacParameters;

typedef struct
{
	smDacParameters params;
} smDacState;

typedef struct
{
	// Transport type
	WORD type;
#define ANY_TRANSPORT			0
#define INVALID_TRANSPORT       0
#define SERIAL_TRANSPORT        1
#define ETHERNET_TRANSPORT      2
#define IP_TRANSPORT			2
#define PCI_TRANSPORT			3
#define USB_TRANSPORT			4

	// serial port specific settings
	WORD comm;
	DWORD baud;

	// Ethernet specific settings
	BYTE my_ip_address[4];
	BYTE his_ip_address[4];
	WORD ip_port;

	// USB specific settings
	char dev_name[32];

	// Timeout parameters
	int connect_timeout; // (in msec.)

	// PCI specific settings
	int pci_bus;
	int pci_device;
	int pci_function;

	// Type of device: SMC6XX or SM21160
	int device_type;

} smTransport;


typedef struct _smResource
{

	WORD type;
	WORD usertype;

	WORD mode;
#define SM_DISCONNECTED 0
#define SM_CONNECTED    1

	WORD lock;
#define SM_LOCKED       1
#define SM_UNLOCKED     0

	commHandle commChannel[3];
#define SM_CHANNEL      0
#define MCU_CHANNEL     1
#define DSP_CHANNEL     2

	smTransport transport;

	int connection;

	// General
	int (*gettype)    ( struct _smResource *p, WORD *pType );
	int (*getusertype)( struct _smResource *p, WORD *pUserType );
	int (*getversion) ( struct _smResource *p, int versionType, smVersion *pVersion );
	int (*setbaudrate)( struct _smResource *p, DWORD baud_rate );
	int (*getmcubist) ( struct _smResource *p, DWORD *pAddress, DWORD *pLength );

	// User Management
	int (*getusers)( struct _smResource *p, char *szUserList );
	int (*setuser) ( struct _smResource *p, char *szUser );

	// Lock Management
	int (*setlock)( struct _smResource *p, WORD lock );
	int (*getlock)( struct _smResource *p, WORD *pLock );

	// Ethernet Configuration
	int (*setethermac)   ( struct _smResource *p, char mac[6] );
	int (*setetheroption)( struct _smResource *p, int optionType, int optionVal );
	int (*setetherip)    ( struct _smResource *p, int ipType, char ip[4] );
	int (*getethermac)   ( struct _smResource *p, char mac[6] );
	int (*getetheroption)( struct _smResource *p, int optionType, int *pOptionVal );
	int (*getetherip)    ( struct _smResource *p, int ipType, char ip[4] );

	// MEM Management Functions
	int nbMemDevices;
#define MAX_MEM_DEVICES	1
	struct
	{
		char szDescriptor[25];
		int (*malloc) ( struct _smResource *p, int len, DWORD *pAddr );
		int (*read)   ( struct _smResource *p, DWORD addr, int len, BYTE *pData );
		int (*write)  ( struct _smResource *p, DWORD addr, int len, BYTE *pData );
		int (*free)   ( struct _smResource *p, DWORD addr );
	} memDevices[MAX_MEM_DEVICES];

	// FILESYS Management Functions
	struct
	{
		char szDescriptor[25];
		int (*fopen)  ( struct _smResource *p, char *mode, char *name, DWORD *pAddr );
		int (*fread)  ( struct _smResource *p, DWORD addr, int len, BYTE *pData );
		int (*fwrite) ( struct _smResource *p, DWORD addr, int len, BYTE *pData );
		int (*fclose) ( struct _smResource *p, DWORD addr );
		int (*fdelete)( struct _smResource *p, char *remote );
		int (*fput)   ( struct _smResource *p, char *local, char *remote );
		int (*fget)   ( struct _smResource *p, char *local, char *remote );
		int (*dllload)   ( struct _smResource *p, char *remote, DWORD *pAddr );
		int (*dllunload) ( struct _smResource *p, DWORD addr );
	} filesysDevice;

	// FLASH Management Functions
	int nbFlashDevices;
#define MAX_FLASH_DEVICES	1
	struct
	{
		char szDescriptor[25];
		int (*upgrade)( struct _smResource *p, WORD fileType, char *szFileName );
	} flashDevices[MAX_FLASH_DEVICES];


	// DSP Management Functions
	int nbDspDevices;
#define MAX_DSP_DEVICES		4
	struct
	{
		char szDescriptor[25];
		int (*reset)( struct _smResource *p, int dspId );
		int (*download)( struct _smResource *p,  int dspId, WORD fileType, char *szFileName );
		int (*flash)( struct _smResource *p, int dspId, WORD fileType, char *szFileName );
		int (*read )( struct _smResource *p, int dspId, int dspPage, int dspSize, int dspCount, DWORD dspAddress, DWORD dspInc, DWORD *pDspData );
		int (*write)( struct _smResource *p, int dspId, int dspPage, int dspSize, int dspCount, DWORD dspAddress, DWORD dspInc, DWORD *pDspData );
	} dspDevices[MAX_DSP_DEVICES];

	// FPGA Management Functions
	int nbFpgaDevices;
#define MAX_FPGA_DEVICES	2
	struct
	{
		char szDescriptor[25];
		int (*download)( struct _smResource *p, WORD fileType, char *szFileName );
		int (*flash)( struct _smResource *p, int dspId, WORD fileType, char *szFileName );
	} fpgaDevices[MAX_FPGA_DEVICES];

	// LCD Management Functions
	int nbDisplayDevices;
#define MAX_DISPLAY_DEVICES	1
	struct
	{
		char szDescriptor[25];
		int (*clear)( struct _smResource *p );
		int (*writeString )( struct _smResource *p, WORD x, WORD y, char *pString );
		int (*setContrast)( struct _smResource *p, BYTE val );
	} displayDevices[MAX_DISPLAY_DEVICES];

	// ADC Management Functions
	int     nbAdcDevices;
#define MAX_ADC_DEVICES		2
	struct
	{
		char szDescriptor[25];
		int (*get_caps  )( struct _smResource *p, smAdcCapabilities *pAdcCaps   );
		int (*set_params)( struct _smResource *p, smAdcParameters   *pAdcParams );
		int (*get_state )( struct _smResource *p, smAdcState        *pAdcState  );
		int (*set_gain  )( struct _smResource *p, smAdcParameters   *pDacParams );
		int (*set_input )( struct _smResource *p, smAdcParameters   *pDacParams );
	} adcDevices[MAX_ADC_DEVICES];

	// DAC Management Functions
	int     nbDacDevices;
#define MAX_DAC_DEVICES		2
	struct
	{
		char szDescriptor[25];
		int (*get_caps  )( struct _smResource *p, smDacCapabilities *pDacCaps   );
		int (*set_params)( struct _smResource *p, smDacParameters   *pDacParams );
		int (*get_state )( struct _smResource *p, smDacState        *pDacState  );
		int (*set_atten )( struct _smResource *p, smDacParameters   *pDacParams );
		int (*set_mute  )( struct _smResource *p, smDacParameters   *pDacParams );
	} dacDevices[MAX_DAC_DEVICES];

	int nbHardwareModule;
#define MAX_HARDWARE_MODULE	    2
	struct
	{
		char szDescriptor[25];
		int (*downloadHwModule)( struct _smResource *p, WORD fileType, char *szFileName );
        int (*resetHwModule)( struct _smResource *p );
	} hardwareModule[MAX_HARDWARE_MODULE];

	// SMTK Functions
	void *smtk_progress_bar_context;
	void * (*smtk_progress_bar_new)( void * handle, char *title, int min, int max );
	int (*smtk_progress_bar_set_value)( void * obj, int val );
	int (*smtk_progress_bar_get_value)( void * obj );
	int (*smtk_progress_bar_delete)( void * obj );

} smResource;

typedef void * smHandle;

typedef struct
{
	BYTE	type;
	BYTE	length;
	BYTE	data[256];
} smSRecord;


#if defined(OS_WIN32)
#ifndef _WINDOWS_
#include <winsock2.h>
#endif
#elif defined(OS_LINUX)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#endif


#pragma pack(push,4)
typedef struct _smDetectInfo
{
	char szName[32];
	struct in_addr ip_address;
	int type;
	int dspid;
	int lock;
	int lockOwner;
	smVersion hwVersion;
	smVersion fpgaFirmwareVersion;
	smVersion fpgaCurrentVersion;
	smVersion mcuCommDrvVersion;
	smVersion mcuFirmwareVersion;
	smVersion dspFirmwareVersion;
	smVersion defaultVersion;
	int		DspType;
	float	DspClock;
	int		DspClockMode; 
	int		DspBootMode;
	int		DspMap;
	int		DspEndianess;
	int		DspJtagSource;
	int		AddressSize;	/* 0-> Byte, 1->Word, 2->Dword */
	int		FpgaBoot;		/* unused, for now */
	int		FpgaModel;

	// PCI specific settings
	int pci_bus;
	int pci_device;
	int pci_function;

	int transport_type;

	// USB specific
	char usbDevName[32];

} smDetectInfo,*PSmDetectInfo;
#pragma pack(pop)

//#endif

/****************************************************************************
 *                           Function Prototypes                            *
 ****************************************************************************/

#if defined(OS_WIN32) && defined(SMDLL_EXPORTS)
#define FUNC_DECL __declspec(dllexport)
#elif defined(OS_WIN32) && defined(SMDLL_IMPORTS)
#define FUNC_DECL __declspec(dllimport)
#else
#define FUNC_DECL
#endif
FUNC_DECL int sm_init( void );
FUNC_DECL int sm_quit( void );
FUNC_DECL int sm_detect( smTransport *transportTemplate, smTransport transportList[], int maxDetections );
FUNC_DECL int sm_detectV2( smTransport *transportTemplate, smDetectInfo* InfoList, int maxDetections, struct in_addr* startIP, struct in_addr* stopIP);
FUNC_DECL int sm_open( smTransport *pTransport, int obsoleteArgument, smHandle *h, WORD *pSmType );
FUNC_DECL int sm_close( smHandle h );
FUNC_DECL int sm_get( smHandle h, DWORD getId, ... );
FUNC_DECL int sm_set( smHandle h, DWORD setId, ... );
FUNC_DECL int sm_recv( smHandle h, DWORD **hData, int *pLength );

int sm_transaction( commHandle h, int connection, BYTE *pOutPaquet, int outSize, BYTE **hInPaquet, int *pInSize );
int sm_timed_transaction( commHandle h, int connection, BYTE *pOutPaquet, int outSize, BYTE **hInPaquet, int *pInSize, DWORD timeout );

/****************************************************************************
 *                        sm_set/sm_get Operations                          *
 ****************************************************************************/

// Global SMLIC Configuration operations
#define SM_SET_SMTK_PROGRESS_BAR_FUNCS	0x10000001

// Lock/Unlock operations
#define SM_GET_MODE             0x00000001
#define SM_SET_LOCK             0x00000002
#define SM_GET_LOCK             0x00000003

// Type/Version querying operations
#define SM_GET_SMTYPE           0x00010002
#define SM_GET_VERSION          0x00010003
#define SM_GET_USERTYPE         0x00010004

// BaudRate Control operations
#define SM_SET_BAUD_RATE        0x00010101

// User Management
#define SM_GET_USERS			0x00010201
#define SM_SET_USER				0x00010202

// MCU operations
#define SM_GET_MEM_ALLOC		0x00011001
#define SM_GET_MEM_READ			0x00011002
#define SM_SET_MEM_WRITE		0x00011003
#define SM_SET_MEM_FREE			0x00011004
#define SM_GET_MCU_BIST			0x00011006
#define SM_GET_FS_OPEN			0x00011007
#define SM_GET_FS_READ			0x00011008
#define SM_SET_FS_WRITE			0x00011009
#define SM_SET_FS_CLOSE			0x0001100A
#define SM_SET_FS_DELETE		0x0001100B
#define SM_SET_FS_PUT			0x0001100C
#define SM_GET_FS_GET			0x0001100D
#define SM_GET_DLL_LOAD			0x00011011
#define SM_SET_DLL_UNLOAD		0x00011012

// DSP operations
#define SM_SET_DSPAPPLICATION   0x00020001
#define SM_SET_DSP_WRITE		0x00020011
#define SM_GET_DSP_READ			0x00020012
// Multi-DSP operations
#define SM_SET_MDSP_RESET		0x00020020
#define SM_SET_MDSP_LOAD		0x00020021
#define SM_SET_MDSP_WRITE		0x00020022
#define SM_GET_MDSP_READ		0x00020023
//#define SM_SET_MDSP_FLASH		0x00020024	// not available yet

// Firmware Upgrading operations
#define SM_SET_FLASHROM         0x00030001
#define SM_SET_FLASHDSP			0x00030002
#define SM_SET_FLASHFPGA		0x00030003

// FPGA operations
#define SM_SET_FPGABITSTREAM	0x00070001

// Display operations
#define SM_SET_DISPLAY_CLEAR    0x00040001
#define SM_SET_DISPLAY_TEXT     0x00040002
#define SM_SET_DISPLAY_CONTRAST	0x00040003

// ADC management operations
#define SM_GET_ADC_CHANNELS     0x00050001
#define SM_GET_ADC_CAPS         0x00050002
#define SM_SET_ADC_PARAMS       0x00050003
#define SM_GET_ADC_STATE        0x00050004
#define SM_SET_ADC_GAIN			0x00050005
#define SM_SET_ADC_INPUT		0x00050006

// DAC management operations
#define SM_GET_DAC_CHANNELS     0x00060001
#define SM_GET_DAC_CAPS         0x00060002
#define SM_SET_DAC_PARAMS       0x00060003
#define SM_GET_DAC_STATE        0x00060004
#define SM_SET_DAC_ATTEN        0x00060005	/* Set DAC Attenuation */
#define SM_SET_DAC_MUTE			0x00060006	/* Mute or unmute DAC  */

// Ethernet Configuration management operations
#define SM_GET_ETHER_MAC		0x00070001
#define SM_GET_ETHER_OPTION		0x00070002
#define SM_GET_ETHER_IP			0x00070003
#define SM_SET_ETHER_MAC		0x00070101
#define SM_SET_ETHER_OPTION		0x00070102
#define SM_SET_ETHER_IP			0x00070103

// Hardware Module operations
#define SM_SET_HM_FPGA_LOAD     0x00080001
#define SM_SET_HM_FPGA_RESET    0x00080002


#endif /* _SMMGR_H_ */
