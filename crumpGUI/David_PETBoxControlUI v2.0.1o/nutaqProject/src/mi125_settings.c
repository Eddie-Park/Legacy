/****************************************************************************  
 *
 *    ****                              I                                    
 *   ******                            ***                                   
 *   *******                           ****                                  
 *   ********    ****  ****     **** *********    ******* ****    ***********
 *   *********   ****  ****     **** *********  **************  *************
 *   **** *****  ****  ****     ****   ****    *****    ****** *****     ****
 *   ****  ***** ****  ****     ****   ****   *****      ****  ****      ****
 *  ****    *********  ****     ****   ****   ****       ****  ****      ****
 *  ****     ********  ****    ****I  ****    *****     *****  ****      ****
 *  ****      ******   ***** ******   *****    ****** *******  ****** *******
 *  ****        ****   ************    ******   *************   *************
 *  ****         ***     ****  ****     ****      *****  ****     *****  ****
 *                                                                       ****
 *          I N N O V A T I O N  T O D A Y  F O R  T O M M O R O W       ****
 *                                                                        ***
 *
 ************************************************************************//**
 *
 * @file       mi125_settings.c
 * @brief      mi125 rtdex example board configuration.
 *
 * $Date: 2013/08/16 18:39:58 $
 * $Revision: 1.5 $
 *
 * (c) 2012, Nutaq Inc. All rights reserved.              
 *
 ***************************************************************************/

#include <stdio.h>
#ifdef __linux__
#include <stdlib.h>
#include <Nutaq/adp_types.h>
#include <Nutaq/adp_media_types.h>
#include <Nutaq/eapi.h>
#include <Nutaq/adp_bus_access_media_types.h>
#include <Nutaq/adp_bus_access_types.h>
#include <Nutaq/adp_bus_access.h>
#include <Nutaq/eapi_mi125.h>
#include <mi125_settings.h>
#else
#include "adp_types.h"
#include "adp_media_types.h"
#include "eapi.h"
#include "adp_bus_access_media_types.h"
#include "adp_bus_access_types.h"
#include "adp_bus_access.h"
#include "eapi_mi125.h"
#include "mi125_settings.h"
#endif

// -----------------------------------------------------
// Setup example configuration begin

// Uncomment to do optionnal board configuration
#define DO_OPTIONNAL_CONFIG 1

#ifdef DO_OPTIONNAL_CONFIG 

// Uncomment to do an extra adc calibration for test purpose
//#define DO_FORCE_CALIB 1

// Uncomment to do an i2c bus devices scan test (for diagnostic purpose)
//#define DO_I2CSCAN 1

// Uncomment to do a local oscillator switch test (for diagnostic purpose)
//#define DO_LOCALOSC_SW_TEST 1

// Uncomment to do adc capture simulation by using a user defined output data pattern (for diagnostic purpose)
//#define DO_ADC_SIMULATION 1

// Uncomment to do adc test: this verifies we can talk with each ADC device correctly (for diagnostic purpose)
//#define DO_ADC_TEST 1

#endif
// Setup example configuration end
// -----------------------------------------------------


int InitMi125( adp_handle_bus_access hBusAccess, unsigned nb, unsigned int clocksource)
{
    adp_result_t res;
    connection_state state;
    //unsigned int core_version, drv_version;
    unsigned int channellanecalib, channelcalibstatus;
    //short temp;
    unsigned int master;
    MI125_mi125_config config[MI125_NBMAX] = {{MI125_16CHANNELS,MI125_TERMON1750UA,MI125_RANDOMIZEOFF,MI125_TWOCOMPLEMENTFORMAT},
                                               {MI125_16CHANNELS,MI125_TERMON1750UA,MI125_RANDOMIZEOFF,MI125_TWOCOMPLEMENTFORMAT}};
    MI125_mi125_clock_source clksrc[MI125_NBMAX] = {MI125_CLKSRC125MHZ,MI125_CLKSRCBOTTOMFMC};
    MI125_mi125_trigout trigout[MI125_NBMAX] = {MI125_TRIGOUTOFF,MI125_TRIGOUTOFF};
#ifdef DO_ADC_SIMULATION
	unsigned short pattern[MI125_NBMAX] = {0x0249, 0x0492};
#endif

    // Check for a valid board instance
	if (nb != 1 && nb !=2)
	{
		printf("Invalid MI125 board instance specified (must be 1 or 2)\r\n");
		return -100;
	}

	// For the first instance (bottom board), configure the clk source
	if ((nb - 1) == 0)
	    clksrc[0] = clocksource;

	// retrieve connection state
	res = BusAccess_GetConnectionState(hBusAccess, &state);
	if( res != 0 )
	{
		printf("BusAccess_GetConnectionState error: 0x%08X\r\n",res);
		return res;
	}
    
	// Minimally verify a board is detected and mi125 fpga core is detected (WARNING: does not verify if a real mi125 board is used!!!)
	res = MI125_Presence_send(&state, nb);
	if( !SUCCESS(res) )
	{
		printf("MI125_Presence_send error: 0x%08X\r\n",res);
		return res;
	}
	/*
    // Display MI125 hardware core and driver versions (for info only)
    res = MI125_mi125_get_versions_send(&state, nb, &core_version, &drv_version);
    if(SUCCESS(res))
    {
        printf("MI125 Core Version: 0x%04X, Driver Version: 0x%04X\r\n" ,core_version, drv_version);
    }else{
        printf("MI125_mi125_get_versions error: 0x%08X\r\n",res);
        return (res);
    }*/
    
	// power up MI125
	res = MI125_powerup_send(&state, nb);
	if(!SUCCESS(res))
	{
		printf( "MI125_Powerup_send error: 0x%08X\r\n",res);
		return res;
	}
 
    //Reset and setup MI125 with basic default configuration (after module powerup, all 16 channels are configured)
	printf("Initializing MI125 board #%d \r\n",nb);
	res = MI125_mi125_reset_send(&state, nb);
	if( !SUCCESS(res) )
	{
		printf( "MI125_Reset_send error: 0x%08X\r\n",res);
 		return res;
	}

    // ----------------------------------------------------------------------------------------
    // Steps between these dash lines are not mandatory (begin)
    

    // Setup ADC clock source
	// This is only necessary if the clock source is not default internal clock
    res = MI125_mi125_set_clksrc_send(&state, nb, clksrc[nb-1]);
    if( SUCCESS(res) )
    {
    }else{
        printf("MI125_mi125_set_clksrc error: 0x%08X\r\n",res);
        return (res);
    }

#ifdef DO_OPTIONNAL_CONFIG

    // This is an optionnal i2c bus scan test (assume 2 mi125 modules in piggy back)
#ifdef DO_I2CSCAN
   {
       MI125_mi125_i2cscan scanraw;
       MI125_mi125_i2caddr scan_res; 
       unsigned char detect_ok, ndevices; 
       // This value hold the max number of i2c devices that should be detected on the bus. 0 means do not
       // check for any other devices than the current tested module instance. When we know how many module
       // instances are present, we can use for a perseus601x carrier a value of 5 (1 mi125 module) or 9 (2 mi125 modules). 
       unsigned char maxexpected = 0;
       
       // Do the i2c bus scan for maximum specified devices
       res = MI125_mi125_scan_i2c_send(&state, nb, maxexpected, &scanraw, &scan_res, &ndevices, &detect_ok);
       if(SUCCESS(res))
       {
            printf("MI125 pcb board #%d i2c bus scan valid: ",nb);
            if (detect_ok)
                printf("yes.\n");
            else
            { 
                printf("no.\n");
                if (scan_res.spibridge)
                    printf("Spi bridge detected.\n");
                else   
                    printf("Spi bridge not detected.\n");
                if (scan_res.sensor)
                    printf("Temperature sensor detected.\n");
                else   
                    printf("Temperature sensor not detected.\n");
                if (scan_res.eeprom)
                    printf("Eeprom detected.\n");
                else   
                    printf("Eeprom not detected.\n");
                if (scan_res.expander)
                    printf("IO expander detected.\n");
                else   
                    printf("IO expander not detected.\n");
                if (ndevices > maxexpected && maxexpected != 0)
                    printf("Number of devices detected: %d, max expected: %d.\n", ndevices, maxexpected);
            } 
       }else{
            printf("MI125_mi125_scan_i2c_send error: 0x%08X\r\n",res);
            return (res);
       }
   }
#endif

#ifdef DO_LOCALOSC_SW_TEST
   // This is an optionnal local oscillator test (this test changes the clock source)
   {
       unsigned char result_ok;
       
       // Do the local oscillator switch test
       res = MI125_mi125_test_localoscsw_send(&state, nb, &result_ok);
       if(SUCCESS(res))
       {
            printf("MI125 pcb board #%d local oscillator sw valid: ",nb);
            if (result_ok)
                printf("yes.\n");
            else
                printf("no.\n");
       }else{
            printf("MI125_mi125_test_localoscsw error: 0x%08X\r\n",res);
            return (res);
       }
   }

    // Setup ADC clock source to what we wanted because it may has been changed by local osc sw test above
	// This is only necessary if the clock source is not default internal clock
    res = MI125_mi125_set_clksrc_send(&state, nb, clksrc[nb-1]);
    if( SUCCESS(res) )
    {
    }else{
        printf("MI125_mi125_set_clksrc error: 0x%08X\r\n",res);
        return (res);
    }

#endif

#if DO_ADC_TEST
    // This is an optionnal communication test with each ADC
    {
       unsigned char written, verify, detected, count;
       MI125_mi125_adcid adcn[]={MI125_ADC0,MI125_ADC1,MI125_ADC2,MI125_ADC3};

  	  // Test each adc
      for(count=0;count<4;count++)
  	  {
           res = MI125_mi125_checkadc_send(&state, nb, adcn[count], &written, &verify, &detected);
           if(SUCCESS(res))
           {
                printf("MI125 pcb board #%d ADC #%d detected: ", nb, adcn[count]);
                if (detected)
                    printf("yes.\n");
                else
                    printf("no.\n");
           }else{
                 printf("MI125_mi125_checkadc_send FAILED: 0x%08X\n",res);
                return res;
            }
       }
    }
#endif

    // Setup board trigout function (do not connect an externel trig source on the external trig pin at the same time)
    // To use the trigout function, it must be connected somewhere in the FPGA user logic (not provided by default). 
    res = MI125_mi125_set_trigout_send(&state, nb, trigout[nb-1]);
    if(!SUCCESS(res))
    {
        printf("MI125_mi125_set_trigout_send FAILED: 0x%08X\n",res);
        return res;
    }
    
    //Config the MI125 with specified user setup (if different than default provided at reset)
	res = MI125_mi125_set_config_send(&state, nb, config[nb-1].groupch, config[nb-1].lvds, config[nb-1].randmode, config[nb-1].binmode);
    if( SUCCESS(res) )
    {
    }else{
        printf("MI125_mi125_set_config error: 0x%08X\r\n",res);
        return (res);
    }

#ifdef DO_FORCE_CALIB    
    // If user changed external clk frequency value (after module is already running) for this clock source mode MI125_CLKSRCEXT for example,
    // it will need to do a clock reset first, then a calibration.
	// If one module (in a multiple modules setup) is a clock master and the other is not,
	// if the clock is reset on the master module, the other module must be recalibrated as well...
    res = MI125_mi125_clockreset_send(&state, nb);
    if( SUCCESS(res) )
    {
    }else{
        printf("MI125_mi125_clockreset error: 0x%08X\r\n",res);
        return (res);
    }

	// Calibration can be done as needed, but the clock must be reset first if the clock frequency has changed:
	// this is done automatically in all cases in api functions, but not when the clock source is external which is impossible to
	// control what the user is doing with the external clock.
	res = MI125_mi125_digital_adccalibration_send(&state, nb);
    if( SUCCESS(res) )
    {
    }else{
        printf("MI125_mi125_digital_adccalibration error: 0x%08X\r\n",res);
        return (res);
    }
#endif        

#ifdef DO_ADC_SIMULATION        
    // If we want to enable adc test mode by using a specific test pattern
	printf("Using ADC test pattern: 0x%04X\r\n", pattern[nb-1]);
	res = MI125_mi125_set_testmode_send(&state, nb, MI125_TESTMODE2, pattern[nb-1]);
    if( SUCCESS(res) )
    {
    }else{
        printf("MI125_mi125_set_testmode error: 0x%08X\r\n",res);
        return (res);
    }

#endif        
#endif        

	// Display ADC channels lanes calibration information
    res = MI125_mi125_get_channelcalibstatus_send(&state, nb, &channellanecalib, &channelcalibstatus);
    if( SUCCESS(res) )
    {
        // Check if the last channels calibration was ok
        if (channelcalibstatus)
        {
            printf("    Board #%d, all channels calibration successfull!\r\n",nb);
        }
        else
        {
            printf("    Board #%d, calibration problem, lane status: %08X\r\n", nb, channellanecalib);
        }
    }else{
        printf("MI125_mi125_get_channelcalibstatus_send error: 0x%08X\r\n",res);
        return (res);
    }
    
    // Display if this module is a clock master or not.
    // In a dual stack board, a module which is not a clock master for the FPGA
    // is using the another module as clock source
    res = MI125_mi125_get_clkmaster_send(&state, nb, &master);
    if(SUCCESS(res))
    {
        printf("    MI125 pcb board #%d is a clk master: ",nb);
        if (master)
            printf("yes.\n");
        else
            printf("no.\n");
    }else{
        printf("MI125_mi125_get_clkmaster_send FAILED: 0x%08X\n",res);
        return res;
    }

    // Display the current MI125 pcb temperature in 0.1C resolution
    // WARNING, this temperature is for information only. This does not
    // give ADC core temperature information. User must make sure that
    // temperature of MI125 board (incl ADC) meet the requierements in his
    // system. Some calibration could be needed between this value
    // and different places/parts on the MI125 board. 
	/*
    res = MI125_mi125_get_temperature_send(&state, nb, MI125_TEMP0DOT1C, &temp);
    if( SUCCESS(res) )
    {
        printf("MI125 pcb board #%d temp: %d.%01dC\r\n",nb, temp/10,abs(temp%10));
    }else{
        printf("MI125_mi125_get_temperature_send error: 0x%08X\r\n",res);
        return (res);
    }*/


    // Steps between these dash lines are not mandatory (end)
    // ----------------------------------------------------------------------------------------
	printf("\n");
	return 0;
}

