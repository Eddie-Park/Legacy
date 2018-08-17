/*I²C*/

#ifndef I2C_HEADER
#define I2C_HEADER

#include "error.h"

#ifdef SMQUADCXXPCI
  #include "smquadcxx.h"
  extern int I2C_fpgaid;
  #define WRITEMEM(address,data) (SmQuadcxxWriteFpgaDword(I2C_pQuad,I2C_fpgaid,(unsigned int) address,1,4,&(data)))
  #define READMEM(address,data) (SmQuadcxxReadFpgaDword(I2C_pQuad,I2C_fpgaid,(unsigned int) address,1,4,&(data)))
#elif SMDPCI
  #include "smd.h"
  #define WRITEMEM(address,data) (SmDWriteFpgaDword(I2C_pDual,(unsigned int) address,1,4,&(data)))
  #define READMEM(address,data) (SmDReadFpgaDword(I2C_pDual,(unsigned int) address,1,4,&(data)))
#else
  #define WRITEMEM(address,data) (*((volatile unsigned int *)address)=data)
  #define READMEM(address,data) (data = *((volatile unsigned int *)address))
#endif //SMQUADCXXPCI

#define I2C_TIMEOUT 500000
#define I2C_READOP    1
#define I2C_WRITEOP   0

/** \addtogroup I2C_register_add
* I2C register addresses
@{ */
#define I2C_CLOCK_SCALE_LOW   0x90000100  /*!< Base Frequency multiplier - low bits */
#define I2C_CLOCK_SCALE_HIGH    0x90000104  /*!< Base Frequency multiplier - high bits */
#define I2C_CONTROL       0x90000108  /*!< Control register */
#define I2C_DATA          0x9000010C  /*!< Transmit (Write) and receive (Read) registers */
#define I2C_STATUS_COMMAND    0x90000110  /*!< Status (Read) and command (Write) registers */
/** @} */

/**
* I²C module errors
*/
enum I2C_ERROR {
  I2C_NOERROR = MAKE_RESULT(LYR_SEVERITY_SUCCESS,MODULE_I2C,0),         /*!< No error */
  I2C_ARBITATIONLOST = MAKE_RESULT(LYR_SEVERITY_ERROR,MODULE_I2C,1),    /*!< Arbitration of the bus was lost */
  I2C_TIMEOUTERR = MAKE_RESULT(LYR_SEVERITY_ERROR,MODULE_I2C,2),        /*!< I²C slave didn't respond in a timely manner*/
  I2C_NOANSWER = MAKE_RESULT(LYR_SEVERITY_WARNING,MODULE_I2C,3),        /*!< I²C slave didn't respond in a timely manner to slave select */
  I2C_NOTDONE = MAKE_RESULT(LYR_SEVERITY_ERROR,MODULE_I2C,4),     /*!< Transfer (tx) didn't end before timeout */
  I2C_BUSYERR = MAKE_RESULT(LYR_SEVERITY_ERROR,MODULE_I2C,5)            /*!< I²C bus is busy, timeout occured */
};

/**
* Control Register
*/
typedef enum {
  I2C_EN =  0x80, /*!< Enable I²C */
  I2C_IEN = 0x40  /*!< Enable interrupt */
} I2C_CONTROL_REGISTER;

/**
* Command register
*/
typedef enum {
  I2C_START =   0x80, /*!< Send start signal */
  I2C_STOP =    0x40, /*!< Send stop (end of) signal */
  I2C_READ =    0x20, /*!< Read data */
  I2C_WRITE =   0x10, /*!< Write data */
  I2C_NACK =    0x8,  /*!< Send nack signal */
  I2C_IACK =    0x1   /*!< Interrupt acknowledge, clear interrupt */
} I2C_COMMAND_REGISTER;

/**
* Status register
*/
typedef enum {
  I2C_RXACK =     0x80, /*!< Slave acknowledge */
  I2C_BUSY =      0x40, /*!< I²C bus is busy */
  I2C_ARBITRATIONLOST=0x20, /*!< Arbitration lost, start signal detected on the line but it's not us */
  I2C_TIP =     0x2,  /*!< Transfer in progress */
  I2C_IF =      0x1   /*!< Interrupt flag */
} I2C_STATUS_REGISTER;

//   ---   Functione prototypes   ---

#ifdef SMQUADCXXPCI
  extern PSMQUADC6XX_DEVICE I2C_pQuad;
 #define I2C_Init               I2C_Init_smq
 #define I2C_Disable            I2C_Disable_smq
 #define I2C_Start              I2C_Start_smq
 #define I2C_SelectSlave        I2C_SelectSlave_smq
 #define I2C_Stop               I2C_Stop_smq
 #define I2C_WriteData          I2C_WriteData_smq
#elif SMDPCI
  extern PSMD_DEVICE            I2C_pDual;
 #define I2C_Init               I2C_Init_smd
 #define I2C_Disable            I2C_Disable_smd
 #define I2C_Start              I2C_Start_smd
 #define I2C_SelectSlave        I2C_SelectSlave_smd
 #define I2C_Stop               I2C_Stop_smd
 #define I2C_WriteData          I2C_WriteData_smd
#else

#endif

/**
* Setup and enable I²C
*/
extern Result I2C_Init();

/**
* Disable I²C
*/
extern Result I2C_Disable();

/**
* Send start signal on bus (gain arbitration of the bus)
*/
extern Result I2C_Start();

/**
* Gain bus arbitration, send slave address and wait for slave acknowledge
*/
extern Result I2C_SelectSlave(char address, char read);

/**
* Release bus arbitration, send stop signal
*/
extern Result I2C_Stop();

/**
* Send a byte and wait for acknowledge
*/
extern Result I2C_WriteData(char data);


// Signal Master Quad API function wrapper
#ifdef SMQUADCXXPCI
extern Result SmQuadcxx_I2C_Init(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id);
extern Result SmQuadcxx_I2C_Disable(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id);
extern Result SmQuadcxx_I2C_Start(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id);
extern Result SmQuadcxx_I2C_SelectSlave(PSMQUADC6XX_DEVICE pQuadStruct, int fpga_id, char address, char read);

#elif SMDUALPCI

 extern Result SmD_I2C_Init(PSMD_DEVICE pDualStruct);
 extern Result SmD_I2C_Disable(PSMD_DEVICE pDualStruct);
 extern Result SmD_I2C_Start(PSMD_DEVICE pDualStruct);
 extern Result SmD_I2C_SelectSlave(PSMD_DEVICE pDualStruct, char address, char read);

#endif


#endif //I2C_HEADER


