
/*************************************************************************\
| FILE NAME:       SnapSDK.h                                              |
| TARGET PLATFORM: Windows 2000, Windows XP	                              |
|                                                                         |
|             Copyright (c) 2003-2006 Rad-icon Imaging Corp.              |
\*************************************************************************/

// Version 1.0 -- initial release March, 2003
//         1.1 -- added Snap_CameraPath to handle multiple cameras

#ifndef __INCLUDED_SNAPSDK_H
#define __INCLUDED_SNAPSDK_H

#ifdef SNAPSDK_EXPORTS
#define SNAPSDK_API __declspec(dllexport)
#else
#define SNAPSDK_API __declspec(dllimport)
#endif

// Error code definitions
#define SNAPERROR_NOERROR 0
#define SNAPERROR_NOCAMERA 1
#define SNAPERROR_IOERROR 2
#define SNAPERROR_OPENTHMBNAILFILE 3
#define SNAPERROR_OPENIMAGEFILE 4
#define SNAPERROR_INVALIDPARAMETER 5
#define SNAPERROR_INVALIDFILEPATH 6
#define SNAPERROR_INVALIDPIXMAPFILE 7
#define SNAPERROR_PIXMAPSYNTAX 8
#define SNAPERROR_INVALIDPCMETHOD 9
#define SNAPERROR_NOVALIDIMAGE 10
#define SNAPERROR_NOVALIDOFFSET 11
#define SNAPERROR_NOTENOUGHMEMORY 97
#define SNAPERROR_CALLNOTSUPPORTED 98
#define SNAPERROR_INVALIDERRCODE 99

// Pixel correction method definitions
#define SNAPMETHOD_MEAN 0
#define SNAPMETHOD_INTERPOLATE 1
#define SNAPMETHOD_GRADIENT 2
#define SNAPMETHOD_INT_HOR 1
#define SNAPMETHOD_INT_VERT 2
#define SNAPMETHOD_GRAD_HOR 3
#define SNAPMETHOD_GRAD_VERT 4

// Setup.txt status byte indices
#define SNAP_INDEXVALIDIMAGE 33
#define SNAP_INDEXVALIDOFFSET 51
#define SNAP_INDEXVALIDPIXMAP 69
#define SNAP_INDEXOFFSETCORR 87
#define SNAP_INDEXPIXELCORR 105
#define SNAP_INDEXIMAGEGAIN 123
#define SNAP_INDEXOFFSETGAIN 141
#define SNAP_INDEXRESETSTATE 159
#define SNAP_INDEXBINSTATE 177
#define SNAP_INDEXNDRSTATE 195
#define SNAP_INDEXINTTIME 209
#define SNAP_INDEXTIMINGMODE 231

// Pixel map constants and other definitions
#define SNAP_MAXPIXMAPSIZE 1000       // maximum number of entries
#define SNAP_MAXPIXMAPFILESIZE 10240  // maximum file size (10kBytes)
#define SNAP_MAXERRORMESSAGE 128      // maximum error message length
#define SNAP_READSETUP 1
#define SNAP_WRITESETUP 0
#define SNAP_SETUPFILESIZE 252
#define SNAP_IMAGEBUFSIZE 16384
#define SNAP_OFFSET_LIGHTER 1
#define SNAP_OFFSET_DARKER 0
/*
// Structure definition for pixel map entries array
typedef struct {
	char type;           // defect type: 'P'ixel, 'R'ow or 'C'olumn
	unsigned short x1;   // x (column) coordinate or start of column range
	unsigned short x2;   // end of column range
	unsigned short y1;   // y (row) coordinate or start of row range
	unsigned short y2;   // end of row range
	unsigned short mask; // correction mask to identify surrounding defects
	char flag;           // preferred correction method
} PIXMAPENTRY;
*/
#ifdef __cplusplus
extern "C" {
#endif

// ShadoCam Imaging Library function definitions
SNAPSDK_API short __stdcall Snap_OpenCamera(void);

SNAPSDK_API short __stdcall Snap_ReadSetupInfo(unsigned char *setupBuf, unsigned long *pBufSize);

SNAPSDK_API short __stdcall Snap_LoadThumbnail(unsigned char *imgBuf, unsigned long *pBufSize);

SNAPSDK_API short __stdcall Snap_LoadRawImage(unsigned short *imgBuf, unsigned long *pBufSize);

SNAPSDK_API short __stdcall Snap_LoadTifImage(unsigned char *imgBuf, unsigned long *pBufSize);

SNAPSDK_API short __stdcall Snap_LoadRawOffset(unsigned short *imgBuf, unsigned long *pBufSize);

SNAPSDK_API short __stdcall Snap_LoadTifOffset(unsigned char *imgBuf, unsigned long *pBufSize);

SNAPSDK_API short __stdcall Snap_CloseCamera(void);

SNAPSDK_API short __stdcall Snap_ErrorMessage(short errCode, char *errMessage);

SNAPSDK_API short __stdcall Snap_CameraIO(char bRead, unsigned long *pNumBytes);

SNAPSDK_API short __stdcall Snap_CameraPath(char chDriveLetter);

SNAPSDK_API short __stdcall SnapCmd_AcquireImage(void);

SNAPSDK_API short __stdcall SnapCmd_AcquireOffset(void);

SNAPSDK_API short __stdcall SnapCmd_GetImageGain(short *pImageGain);

SNAPSDK_API short __stdcall SnapCmd_SetImageGain(short nImageGain);

SNAPSDK_API short __stdcall SnapCmd_GetOffsetGain(short *pOffsetGain);

SNAPSDK_API short __stdcall SnapCmd_SetOffsetGain(short nOffsetGain);

SNAPSDK_API short __stdcall SnapCmd_GetOffsetCorrection(char *pOffsetCorr);

SNAPSDK_API short __stdcall SnapCmd_SetOffsetCorrection(char bOffsetCorr);

SNAPSDK_API short __stdcall SnapCmd_GetPixelCorrection(char *pPixelCorr);

SNAPSDK_API short __stdcall SnapCmd_SetPixelCorrection(char bPixelCorr);

SNAPSDK_API short __stdcall SnapCmd_LoadPixelMap(void);

SNAPSDK_API short __stdcall SnapCmd_GetIntegrationTime(unsigned short *pIntTime);

SNAPSDK_API short __stdcall SnapCmd_SetIntegrationTime(unsigned short nIntTime);

SNAPSDK_API short __stdcall SnapCmd_GetTimingMode(short *pTimingMode);

SNAPSDK_API short __stdcall SnapCmd_SetTimingMode(short nTimingMode);

SNAPSDK_API short __stdcall SnapCmd_GetResetState(char *pRESET);

SNAPSDK_API short __stdcall SnapCmd_SetResetState(char bRESET);

SNAPSDK_API short __stdcall SnapCmd_GetBinState(char *pBIN);

SNAPSDK_API short __stdcall SnapCmd_SetBinState(char bBIN);

SNAPSDK_API short __stdcall SnapCmd_GetNdrState(char *pNDR);

SNAPSDK_API short __stdcall SnapCmd_SetNdrState(char bNDR);

SNAPSDK_API short __stdcall SnapCmd_OffsetAdjust(short nChannel, char bDirection);

SNAPSDK_API short __stdcall SnapCmd_MemoryTest(void);

SNAPSDK_API short __stdcall SnapCmd_GetImageState(void);

SNAPSDK_API short __stdcall SnapCmd_GetOffsetState(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDED_SNAPSDK_H