
/*************************************************************************\
| FILE NAME:       Scilib21.h                                             |
|                  ShadoCam Imaging Library Header File                   |
| TARGET PLATFORM: Windows 95/98 (32-bit), Windows NT, 2000, XP, Vista, 7 |
|                                                                         |
|             Copyright (c) 2001-2010 Rad-icon Imaging Corp.              |
\*************************************************************************/

// Ver. 2.1.1  -- released April 19, 2011
//                fixed clear buffer error in ReadPixMap function
//                upgraded to Windows installer for Vista & Windows 7 compatibility
// Version 2.1 -- released Dec. 16, 2009
//                improve deinterlace algorithm for Shad-o-Box 4K
//                add support for new camera models
//                increase max. size of pixel map to 10000 entries (100kB)
//                add bright pixel filter algorithm
// Version 2.0 -- released July 16, 2004
//                add support for Shad-o-Snap cameras
//                add support for Shad-o-Box 4K
//                double max. size of pixel map to 2000 entries (20kB)
// Version 1.1 -- released April 3, 2003
//                add compatibility for regular C-calls
//                expand pixel correction to partial rows & columns
//                repaired Row1 pixel correction bug
// Version 1.0 -- initial release Oct. 03, 2001

#ifndef __INCLUDED_SCIMGLIB_H
#define __INCLUDED_SCIMGLIB_H

#ifdef SCIMGLIB_EXPORTS
#define SCIMGLIB_API __declspec(dllexport)
#else
#define SCIMGLIB_API __declspec(dllimport)
#endif

// CamType definitions
#define SCCAMTYPE_512        0
#define SCCAMTYPE_1024       1
#define SCCAMTYPE_1024A      2
#define SCCAMTYPE_1536       3
#define SCCAMTYPE_2048       4
#define SCCAMTYPE_4K         5
#define SCCAMTYPE_512HS      6
#define SCCAMTYPE_CUSTOM     7
#define SCCAMTYPE_SKIA8      8
#define SCCAMTYPE_SKIA10     9
#define SCCAMTYPE_SNAP1024  10
#define SCCAMTYPE_SNAP2048  11
#define SCCAMTYPE_SNAP1K    12
#define SCCAMTYPE_SNAP4K    13
#define SCCAMTYPE_REMOTE1   15
#define SCCAMTYPE_REMOTE2   16
#define SCCAMTYPE_REMOTEHR  17
#define SCCAMTYPE_REMOTE200 18
#define SCCAMTYPE_1024HS    22

// Error code definitions
#define SCERROR_NOERROR 0
#define SCERROR_INVALIDIMAGESIZE 1
#define SCERROR_INVALIDCAMTYPE 2
#define SCERROR_INVALIDIMAGEWIDTH 3
#define SCERROR_INSUFFICIENTMEMORY 4
#define SCERROR_INVALIDBUFFERWIDTH 5
#define SCERROR_INVALIDFILEPATH 6
#define SCERROR_INVALIDPIXMAPFILE 7
#define SCERROR_PIXMAPSYNTAX 8
#define SCERROR_INVALIDPCMETHOD 9
#define SCERROR_INVALIDERRCODE 99

// Pixel correction method definitions
#define SCMETHOD_MEAN 0
#define SCMETHOD_INTERPOLATE 1
#define SCMETHOD_GRADIENT 2
#define SCMETHOD_INT_HOR 1
#define SCMETHOD_INT_VERT 2
#define SCMETHOD_GRAD_HOR 3
#define SCMETHOD_GRAD_VERT 4

// Pixel map constants definitions
#define SCMAXPIXMAPSIZE 20000      // maximum number of entries
#define SCMAXPIXMAPFILESIZE 102400 // maximum file size (100kBytes)
#define SCMAXERRORMESSAGE 128      // maximum error message length

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

// ShadoCam Imaging Library function definitions
#ifdef __cplusplus
extern "C" {
#endif

SCIMGLIB_API short __stdcall ScDeinterlace(short *imgBuf, unsigned int nBufSize, unsigned int nWidth, unsigned int nHeight, unsigned short CamType, BOOL bGapSpace);

SCIMGLIB_API short __stdcall ScOffsetCorrection(short *imgBuf, short *ofstBuf, unsigned int nWidth, unsigned int nHeight);

SCIMGLIB_API short __stdcall ScGainCorrection(short *imgBuf, short *gainBuf, unsigned int nWidth, unsigned int nHeight, short nMean);

SCIMGLIB_API short __stdcall ScPixelCorrection(short *imgBuf, unsigned int nWidth, unsigned int nHeight, PIXMAPENTRY *pixMap, unsigned short nPixMapCount, unsigned short pcMethod);

SCIMGLIB_API void  __stdcall ScFixPixel(short *imgBuf, unsigned int nWidth, unsigned int nHeight, int nCol, int nRow, int nMethod, int nMask);

SCIMGLIB_API short __stdcall ScReadPixMap(char *filePath, PIXMAPENTRY *pixMap, int *numEntries);

SCIMGLIB_API short __stdcall ScCalcPixMapMask(PIXMAPENTRY *pixMap, unsigned short nPixMapCount);

SCIMGLIB_API short __stdcall ScImageFilter(short* imageBuf, int nCols, int nRows, short nValue, int nLimits);

SCIMGLIB_API short __stdcall ScPixelFilter(short* pList, int nElements, int nLimits);

SCIMGLIB_API short __stdcall ScErrorMessage(short errCode, char *errMessage);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDED_SCIMGLIB_H
