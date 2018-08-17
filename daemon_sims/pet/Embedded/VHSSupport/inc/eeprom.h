
#ifndef EEPROM_HEADER
#define EEPROM_HEADER

#include "I2C.h"

#define EEPROM_ADDRESS 0xA0 //eeprom is at 1010xxx0b

enum EEPROM_ERROR {
  EEPROM_NOERROR = MAKE_RESULT(LYR_SEVERITY_SUCCESS,MODULE_I2C_EEPROM,0),
  EEPROM_TIMEOUT  = MAKE_RESULT(LYR_SEVERITY_ERROR,MODULE_I2C_EEPROM,1)
};

//   ---   Functions prototype   ---

/**
* Read eeprom
*
* \param[in] address start address of read on eeprom
* \param[in] size number of bytes to read
* \param[out] iodata pointer to a properly allocated buffer or array
*
* \return standard Lyrtech error code format
*/


#ifdef SMQUADCXXPCI
 #define eeprom_ReadBlock    eeprom_ReadBlock_smd
 #define eeprom_Write        eeprom_Write_smd
 #define eeprom_ReadCurrent  eeprom_ReadCurrent_smd
#elif SMDPCI
 #define eeprom_ReadBlock    eeprom_ReadBlock_smq
 #define eeprom_Write        eeprom_Write_smq
 #define eeprom_ReadCurrent  eeprom_ReadCurrent_smq
#else

#endif

extern Result eeprom_ReadBlock(unsigned char address, unsigned char size, unsigned char * iodata);
extern Result eeprom_Write(unsigned char address, unsigned char iodata);
extern Result eeprom_ReadCurrent(unsigned char *iodata);

#ifdef SMQUADCXXPCI
 extern Result SmQuadcxx_eeprom_ReadBlock(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id, unsigned char address, unsigned char size, unsigned char * iodata);

 extern Result SmQuadcxx_eeprom_WriteByte(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id, unsigned char address, unsigned char iodata);

 extern Result SmQuadcxx_eeprom_ReadCurrent(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id, unsigned char * iodata);

#elif SMDPCI
 extern Result SmD_eeprom_ReadBlock(PSMD_DEVICE pDualStruct, unsigned char address, unsigned char size, unsigned char * iodata);

 extern Result SmD_eeprom_WriteByte(PSMD_DEVICE pDualStruct, unsigned char address, unsigned char iodata);

 extern Result SmD_eeprom_ReadCurrent(PSMD_DEVICE pDualStruct, unsigned char * iodata);
#endif

#endif //EEPROM_HEADER

