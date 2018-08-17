/***************************************************************************
 *
 *
 *         **  **     **  ******  ********  ********  ********  **    **
 *        **    **   **  **   ** ********  ********  ********  **    **
 *       **     *****   **   **    **     **        **        **    **
 *      **       **    ******     **     ****      **        ********
 *     **       **    **  **     **     **        **        **    **
 *    *******  **    **   **    **     ********  ********  **    **
 *   *******  **    **    **   **     ********  ********  **    **
 *
 *
 ***************************************************************************
 *
 *  Project     : Common
 *  File        : register_operation.h
 *  Description : Operation on register or variable.
 *
 *  Author(s)   : Simon Boucher
 *
 *                Copyright (c) Lyrtech 2008
 *
 ***************************************************************************
 *
 *  "$Revision: 1.3 $"
 *  "$Name: 1.3 $"
 *  "$Date: 2008/01/17 17:25:44 $"
 *  "$Author: simon.boucher $"
 *
 **************************************************************************/

#ifndef _REGISTER_OPERATION_H_
#define _REGISTER_OPERATION_H_

/************************************************************************//**
 *
 * Configure the compilation mode for DSP (inline) and host (not inline).
 *
 ****************************************************************************/
#ifdef WIN32

    #define INLINE_COMPILATION

#else

    #define INLINE_COMPILATION inline

#endif

/************************************************************************//**
 *
 * Points to a register or memory value.
 *
 * @param [in]  REG
 *     Register or memory address.
 *
 * @return
 *      The contents of the register or memory.
 *
 ****************************************************************************/
#define REG_REGISTER(REG)      *((volatile unsigned int *)(REG))


/************************************************************************//**
 *
 * Raises a register or memory bit.
 *
 * @param [in]  REG
 *     Register or memory address.
 *
 * @param [in]  BIT
 *     Bit position.
 *
 ****************************************************************************/
#define REG_SET_BIT(REG, BIT)    (REG_REGISTER(REG) |= (1 << BIT))


/************************************************************************//**
 *
 * Clears a register or memory bit.
 *
 * @param [in]  REG
 *     Register or memory address.
 *
 * @param [in]  BIT
 *     Bit position.
 *
 ****************************************************************************/
#define REG_CLEAR_BIT(REG, BIT) (REG_REGISTER(REG) &= ~(1 << BIT))


/************************************************************************//**
 *
 * Retrieves the value of a register or memory bit.
 *
 * @param [in]  REG
 *     Register or memory address.
 *
 * @param [in]  BIT
 *     Bit position.
 *
 * @return
 *      The binary value.
 *
 ****************************************************************************/
#define REG_TEST_BIT(REG, BIT)  ((REG_REGISTER(REG) >> BIT) & 1)


/************************************************************************//**
 *
 * Raises one bit in a register or in memory and resets all other bits.
 *
 * @param [in]  REG
 *     Register or memory address.
 *
 * @param [in]  BIT
 *     Bit position.
 *
 ****************************************************************************/
#define REG_SET_BIT_WO(REG, BIT)    (REG_REGISTER(REG) = (1 << BIT))


/****************************************************************************
 * Function: HM_SetRegister
 ************************************************************************//**
 *
 * Assigns the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address to write to.
 *
 * @param [in]  u32Value
 *      Data to write to the register.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_SetRegister( Uint32 u32Address, Uint32 u32Value );

/****************************************************************************
 * Function: HM_GetRegister
 ************************************************************************//**
 *
 * Retrieves the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address from where to read.
 *
 * @param [out]  pu32Value
 *      Data read from the register returned by reference.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_GetRegister( Uint32 u32Address, Uint32* pu32Value );


/****************************************************************************
 * Function: HM_SetRegisterBit
 ************************************************************************//**
 *
 * Assigns the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address to write to.
 *
 * @param [in]  u32BitPos
 *      Bit position to write in the register.
 *
 * @param [in]  bBitValue
 *      Bit value to write at position u32BitPos in the register.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_SetRegisterBit( Uint32 u32Address,
                                             Uint32 u32BitPos,
                                             Bool bBitValue );

/****************************************************************************
 * Function: HM_GetRegisterBit
 ************************************************************************//**
 *
 * Retrieves the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address from where to read.
 *
 * @param [in]  pu32Value
 *      Data read from the register returned by reference.
 *
 * @param [out]  pbBitValue
 *      Bit value read at position u32BitPos in the register.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_GetRegisterBit( Uint32 u32Address,
                                             Uint32 u32BitPos,
                                             Bool* pbBitValue );



#ifndef WIN32 // DSP only

/****************************************************************************
 * Function: HM_SetRegister
 ************************************************************************//**
 *
 * Assigns the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address where to write.
 *      See CamLnk_Registers enumeration for more information.
 *
 * @param [in]  u32Value
 *      Data to write to the register.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_SetRegister( Uint32 u32Address, Uint32 u32Value )
{

    REG_REGISTER( u32Address ) = u32Value;

    return OK;

}

/****************************************************************************
 * Function: HM_GetRegister
 ************************************************************************//**
 *
 * Retrieves the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address from where to read.
 *      See CamLnk_Registers enumeration for more information.
 *
 * @param [out]  pu32Value
 *      Data read from the register returned by reference.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_GetRegister( Uint32 u32Address, Uint32* pu32Value )
{

    *pu32Value = REG_REGISTER( u32Address );

    return OK;
}


/****************************************************************************
 * Function: HM_SetRegisterBit
 ************************************************************************//**
 *
 * Assigns the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address where to write.
 *
 * @param [in]  u32BitPos
 *      Bit position to write in the register.
 *
 * @param [in]  bBitValue
 *      Bit value to write at position u32BitPos in the register.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_SetRegisterBit( Uint32 u32Address,
                                             Uint32 u32BitPos,
                                             Bool bBitValue )
{
    if(bBitValue)
    {
        REG_SET_BIT(u32Address, u32BitPos);
    }

    else
    {
        REG_CLEAR_BIT(u32Address, u32BitPos);
    }

    return OK;
}

/****************************************************************************
 * Function: HM_GetRegisterBit
 ************************************************************************//**
 *
 * Retrieves the value of a hardware module register.
 *
 * @param [in]  u32Address
 *      Register address from where to read.
 *
 * @param [in]  pu32Value
 *      Data read from the register returned by reference.
 *
 * @param [out]  pbBitValue
 *      Bit value read at position u32BitPos in the register.
 *
 * @return
 *      Success or error code.
 *
 ****************************************************************************/

Result INLINE_COMPILATION HM_GetRegisterBit( Uint32 u32Address,
                                             Uint32 u32BitPos,
                                             Bool *pbBitValue )
{

    *pbBitValue = REG_TEST_BIT(u32Address, u32BitPos);


    return OK;
}



#endif    // WIN32

#endif    // _REGISTER_OPERATION_H_
