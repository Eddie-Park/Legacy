/****************************************************************************
 *                         Technologies Lyre Inc.                           *
 ****************************************************************************
 *                                                                          *
 *  Project     : SignalMaster                                              *
 *  Processor   : Host processor                                            *
 *  File        : smbinhdr.h                                                *
 *  Description : Header for SignalMaster binary Files.                     *
 *                                                                          *
 ****************************************************************************
 *
 * "$Revision: 1.1 $"
 * "$Name: 1.1 $"
 * "$Date: 2007/04/24 21:02:58 $"
 * "$Author: jf.robert $"
 *
 ****************************************************************************/

#ifndef __SMBINHDR_H__
#define __SMBINHDR_H__

#define SMBINHDR_MAJOR	2
#define SMBINHDR_MINOR	0

typedef struct
{
	unsigned long MagicId1;
#define SMBINHDR_MAGIC_ID1	0x4F594F59

	struct
	{
		unsigned short Major;
		unsigned short Minor;
	}
	Version;

	enum
	{
		CPU_TARGETS = 0x00010000,
		TARGET_NS486,
		TARGET_ELANSC520,

		DSP_TARGETS = 0x00020000,
		TARGET_SHARC,
		TARGET_C32,
		TARGET_C67xx,
		TARGET_C62xx,
		TARGET_21160,

		FPGA_TARGETS = 0x00030000,
		// details of FPGA device and package
		// can be extracted from XILINX header
		TARGET_XC4000,
		TARGET_VIRTEX,
	}
	TargetId;

	unsigned long Length;
	unsigned long Checksum;

	unsigned long SizeOfFile;

	// Compression
	struct
	{
		unsigned char Enable;
		unsigned char Reserved;
		unsigned short Algorithm;
#define	COMPRESSION_TYPE_ZLIB_1_1_4		0x1114
		unsigned long UncompressedLength;
		unsigned long CompressedLength;
	}
	Compression;

	unsigned long Id; // DSP_id or FPGA Id

	// PRIVATE TO FlashMgr
	unsigned long Valid; 
	unsigned long SizeUsed;

	// Padding to make sure that header is always 256 bytes
	unsigned char reserved[256 - (14 * sizeof(long))];

	unsigned long extra_flags;	// 0 => Old Version
								// 1 => Support SBC6713 for Labvolt
	unsigned long MagicId2;
#define SMBINHDR_MAGIC_ID2	0x43444341

}
smBinHeader;

#endif // __SMBINHDR_H__
