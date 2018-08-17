/*	LYR Signal Processing
 *	A division of LYRtech Inc.
 *	
 *	"$RCSfile: vhsadac_comm.h,v $"
 *	"$Author: jf.robert $"
 *  "$Date: 2007/04/25 17:38:14 $"
 *
 *	Description: DSP Communication API for VHSADC16 & VHSDAC16 Boards via PCI Pipes
 */
 
#ifndef _VHSADAC_COMM_H
#define _VHSADAC_COMM_H 
                                    
#define VHSADAC_CMD_PIPE_SIZE			128
#define VHSADAC_DATA_PIPE_SIZE			16384

#define VHSADAC_ACK						0x89897756

#define VHSADAC_CMD_CHANNEL				0x0020
#define VHSADAC_DATA_CHANNEL			0x0021

#define VHSADAC_CMD_FPGALOAD			0x0D00
#define VHSADAC_CMD_GET_CPLD_REGS		0x0D01
#define VHSADAC_CMD_READ_FPGA_REG		0x0D02
#define VHSADAC_CMD_WRITE_FPGA_REG		0x0D03
#define VHSADAC_CMD_READ_FPGA_MEM		0x0D04
#define VHSADAC_CMD_WRITE_FPGA_MEM		0x0D05
#define VHSADAC_CMD_OPEN				0x0D10
#define VHSADAC_CMD_INIT				0x0D20

#endif /* _VHSADAC_COMM_H */

