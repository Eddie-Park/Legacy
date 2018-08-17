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
 * @file       adp_mi125_record_test.c
 * @brief      mi125 rtdex example.
 *
 * $Date: 2013/10/04 15:30:43 $
 * $Revision: 1.17 $
 *
 * (c) 2012, Nutaq Inc. All rights reserved.              
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#ifdef __linux__
#include <stdlib.h>
#include <Nutaq/memory_map.h>
#include <Nutaq/adp_types.h>
#include <Nutaq/adp_media_types.h>
#include <Nutaq/eapi.h>
#include <Nutaq/adp_buff_fct.h>
#include <Nutaq/adp_bus_access_media_types.h>
#include <Nutaq/adp_bus_access_types.h>
#include <Nutaq/adp_bus_access.h>
#include <Nutaq/adp_rtdex_types.h>
#include <Nutaq/adp_rtdex_eth_types.h>
#include <Nutaq/adp_rtdex_media.h>
#include <Nutaq/adp_rtdex_access.h>
#include <Nutaq/adp_rtdex.h>
#include <Nutaq/adp_rtdex_eth.h>
#include <Nutaq/adp_recplay_types.h>
#include <Nutaq/adp_recplay.h>
#include <Nutaq/mac_utils.h>
#include <mi125_settings.h>
#else
#include <conio.h>
#include "memory_map.h"
#include "adp_types.h"
#include "adp_media_types.h"
#include "eapi.h"
#include "adp_buff_fct.h"
#include "adp_bus_access_media_types.h"
#include "adp_bus_access_types.h"
#include "adp_bus_access.h"
#include "adp_rtdex_types.h"
#include "adp_rtdex_eth_types.h"
#include "adp_rtdex_media.h"
#include "adp_rtdex_access.h"
#include "adp_rtdex.h"
#include "adp_rtdex_eth.h"
#include "adp_recplay_types.h"
#include "adp_recplay.h"
#include "adp_recplay_test_cnt.h"
#include "mi125_settings.h"
#include "mac_utils.h"
#endif

// WARNING: to run this demo under LINUX, you may need root privilege

#define BUFFER_CREATE_STRUCT CreateTransferBuffData
#define BUFFER_OPEN AdpOpenBuffMem
#define BUFFER_CREATE AdpCreateBuffMem
#define BUFFER_DESTROY AdpDestroyBuffMem
#define BUFFER_GETDATA AdpGetBuffData
#define BUFFER_OFFSETS(pTransfer, StartOff, EndOff) AdpOffsetsBuffMem((pTransfer), (StartOff), (EndOff))


// Uncomment following #define USE_JUMBO_FRAME to use ehernet jumbo frame which should improve data transfer
// performance. But to use this mode, all ethernet elements between the module and the host must support it
// (not all gigabit ethernet adapters support this). Software drivers configuration could also be needed to
// achieve this mode (see perseus documentation).
//#define NUSE_JUMBO_FRAME 1


// This sets the ethernet frame inter packets time pause in 125MHz clk counts when the module is sending data packets to the host.
// Could be used if data packets are lost. Hardware "Pause" frame support could also be used for this
// purpose, but all elements including the computer software drivers in the connection between
// the module and the host must support it and be configured for it. Make this number bigger
// to increase inter-packets delay. This number could be made lower if pause frame is supported
// Around 200000 ~ 1.6 msec inter packet delay
#define FRAME_GAP 200000


// Use for data transfer time estimation to the host (record). Estimation is for gigabit ethernet (see ETH_SPEED)...
#define FRAME_GAP_CLK 125000000
#define ETH_SPEED 1000000000
#define ETH_FRAME_OVERHEAD 38
#define PACKET_OVERHEAD 30


#ifdef NUSE_JUMBO_FRAME
#define FRAMESIZE 8000
#else
#define FRAMESIZE 1472
#endif

// Internal demo version
#define MYVERSION "V0.50"

// Defines for easy command line arguments access
#define Channels argv[1]
#define TrigSource argv[2]
#define RecordFileName argv[3]
#define IpAddress argv[4]
#define ClkSrc argv[5]
#define CAPTURED_SAMPLES_PER_CHANNEL atoi(argv[6])

int InitMi125( adp_handle_bus_access hBusAccess, unsigned nb, unsigned int clocksource);

#ifdef __linux__

#include <termios.h>
#include <unistd.h>

#define INFINITE -1

static int _getch(void);

static int _getch(void)
{
    struct termios oldt,
    newt;
    int ch;

    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

    return ch;
}

#define Sleep(x) usleep( x*1000 )

#endif 

int main( int argc, char* argv[] )
{
   adp_result_t res;
   adp_handle_rtdex_conn hConn = NULL;
   adp_handle_bus_access hBusAccess = NULL;
   adp_handle_recplay hRecPlay = NULL;

   unsigned char uInitDone;
   unsigned char uTransferOver;
   unsigned int uAddrTrig;
   unsigned char uTrigOffset;

   unsigned int* pRecordBuffer;
   void* pRecordMem;
   unsigned int i,channelc, trigsourcec, timeout, clocksource;
   unsigned int recordsize = 0; 
   char pHostMacAddr[6];
   char pDeviceMacAddr[6];
   int rxtime;
   unsigned int needed_transfer, addbytes, nframe_used;
   unsigned int buffer_size;

   // Initialize eapi before use 
   res = eapi_init();
   if( res != 0 )
   {
       printf( "Error initializing eapi.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       return -1;
   }

   // Check for correct command line parameters
   if( argc != 7 )
   {
       printf( "Error: Wrong number of parameters\n"
		       "Parameters 1: the channels config used (1:16 bottom,\n"
		       "              2:16 top, 3:32 with downsampling).\n"
			   "Parameters 2: trigger source (0: soft, 1: external trig).\n"
		       "Parameters 3: the Record data file (write only).\n"
		       "Parameters 4: the FPGA ip address, ex 192.168.0.1\n"
			   "Parameters 5: clk source (0: int, 1: external clk).\n"
			   "Parameters 6: record size in number of samples per channel.\n"
               " The program will terminate. Push any key to continue\n");
       _getch();
       return -1;
   }

   // Extract the channels config
   channelc = atoi(Channels);

   if (channelc <1 || channelc>3)
   {
           printf( "Error: Wrong channels config parameter (different 1,2 or 3)\n"
                   " The program will terminate. Push any key to continue\n" );
           _getch();
           return -1;
   }

   // Extract the trig source
   trigsourcec = atoi(TrigSource);

   if (trigsourcec !=0 && trigsourcec != 1)
   {
           printf( "Error: Wrong trigger source config parameter (different 0, 1)\n"
                   " The program will terminate. Push any key to continue\n" );
           _getch();
           return -1;
   }

   // Extract the clk source
   clocksource = atoi(ClkSrc);

   if (clocksource !=0 && clocksource != 1)
   {
           printf( "Error: Wrong clock source config parameter (different 0, 1)\n"
                   " The program will terminate. Push any key to continue\n" );
           _getch();
           return -1;
   }

   // Remove any already existing record file
   // If not deleted, following with simply writes the needed part of any existing file
   //  remove(RecordFileName);

   pRecordMem = BUFFER_CREATE_STRUCT();

   if( pRecordMem == 0 )
   {
       printf( "Error allocating record memory.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       return -1;
   }

   // Add additionnals bytes for the transfer so we can center the buffer at the specified offset if needed
   // FRAMESIZE / gcd(FRAMESIZE,64) * 64 will find the correct additionnal bytes that meet both conditions:
   // size / 64 (record core) and size / framesize (rtdex core) (smallest common multiple)
   // Assumption here is that addbytes will fit on 32 bits
   addbytes = FRAMESIZE / gcd(FRAMESIZE,64) * 64;
   recordsize = CAPTURED_SAMPLES_PER_CHANNEL * 16 * 2;
   // Check if additionnals bytes will make buffer size > 32 bits
   if (((unsigned int)(4294967295) - addbytes) < recordsize)
   {
	   printf( "Cannot transfer additionnals bytes for buffer offset. Max tx: %u.\n"
			   "The program will terminate. Push any key to continue\n",  ((unsigned int)(4294967295) - addbytes) );
	   _getch();

       BUFFER_DESTROY( pRecordMem );
       return -1;
   }
   
   // Check if the recordsize if a multiple of FRAMESIZE
   if ((recordsize % FRAMESIZE) != 0)
       buffer_size = (recordsize/FRAMESIZE+1)*FRAMESIZE + addbytes; // Ceil to the next FRAMESIZE multiple
   else
       buffer_size = recordsize + addbytes;

   res = BUFFER_CREATE( pRecordMem, RecordFileName, buffer_size);

   if( res != 0 )
   {
       printf( "Error allocating memory for the Record File.\n"
               "Check the file path.\n" 
              " The program will terminate. Push any key to continue\n" );
       _getch();
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   // Get memory buffers pointers for playback and records
   pRecordBuffer = (unsigned int*) BUFFER_GETDATA( pRecordMem );
   if( pRecordBuffer == 0)
   {
       printf( "Error getting record data bufers.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   printf( "MI125 Record demo %s is started.\n\n", MYVERSION);
   printf( "Connecting to IP %s\n",IpAddress);

   res  = BusAccess_Ethernet_OpenWithoutConnection(&hBusAccess, (int8_t *)IpAddress, 0);

   if( res ) 
   {
	   printf( "The Perseus is not responding.\n" );
	   printf( ErrorToString(res) );
	   printf( "\nPlease ensure the Perseus IP address you specified (%s) is correct.\n\n", IpAddress );
	   printf( "The program will terminate. Push any key to continue\n" );
	   _getch();
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   // get MAC address of target and host:
   res = iGetConnectionPeerMacAddr( hBusAccess, (unsigned char *)pDeviceMacAddr);

   if( res ) 
   {
       printf( "Problem getting target MAC address.\n" );
       printf( "The program will terminate. Push any key to continue\n" );
       _getch();
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   printf( "MAC address of Perseus is: '");
   for( i = 0; i < 5; i++)
       printf( "%02X:", (unsigned char)pDeviceMacAddr[ i ]);
   printf( "%02X'\n", (unsigned char)pDeviceMacAddr[ i ]);
   res = iGetConnectionLocalMacAddr( hBusAccess, (unsigned char *)pHostMacAddr);

   if( res ) 
   {
       printf( "Problem getting host MAC address.\n" );
       printf( "The program will terminate. Push any key to continue\n" );
       _getch();
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   printf( "MAC Address of Host is: '");
   for( i = 0; i < 5; i++)
       printf( "%02X:", (unsigned char)pHostMacAddr[ i ]);
   printf( "%02X'\n\n", (unsigned char)pHostMacAddr[ i ]);

   res = RTDExResetCoreEth(hBusAccess,module_rtdex_eth_base); 

   if( res ) 
   {
       printf( "Cannot reset the rtdex core.\n" );
       _getch();
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }  
   
   if (clocksource == 0)
       printf( "Using internal bottom 125MHz clock source\n\n");
   else
       printf( "Using external bottom clock source\n\n");

   // Initialized all needed mi125 boards to capture
   for (i=1; i <= (1 + (unsigned int)(channelc > 1)); i++)
   {
	   res = InitMi125( hBusAccess,  i, clocksource);
	   if( res != 0 ) 
	   {
		   printf( "Error initializing the Mi125\n"
				   "The program will terminate. Push any key to continue\n");

		   _getch();
		   BusAccess_Ethernet_Close( hBusAccess );
		   BUFFER_DESTROY( pRecordMem );
		   return -1;
	   }
   }

   // Set the MI125 Mux to send the correct channels to the record core.
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  (channelc - 1) );
   if( res ) 
   {
       printf( "There was an error in channels record mux configuration.\n\n"
               "The program will terminate. Push any key to continue\n" );
       _getch();
	   
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   // Set mi125 to expected trigger mode (soft or external).
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_1,  trigsourcec );
   if( res ) 
   {
       printf( "There was an error in mi125 trig mode configuration.\n\n"
               "The program will terminate. Push any key to continue\n" );
       _getch();
	   
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   // Reset mi125 soft trig to default.
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_2,  0 );
   if( res ) 
   {
       printf( "There was an error in mi125 soft trig reset.\n\n"
               "The program will terminate. Push any key to continue\n" );
       _getch();
	   
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   // RecPlay
   res = RecPlay_Open( &hRecPlay, hBusAccess, module_rec_play_base );

   if( res ) 
   {
       printf( "There was an error creating a record playback object.\n\n"
               "The program will terminate. Push any key to continue\n" );
       _getch();
	   
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   res = RecPlayResetMemory( hRecPlay );

   if( res != 0 ) 
   {
       printf( "Error with RecPlayResetMemory.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   // Wait for the done
   Sleep( 1000 );

   res = RecPlayGetMemoryInitDoneStatus( hRecPlay, &uInitDone );

   if( res != 0 ) 
   {
       printf( "Error with RecPlayGetMemoryInitDoneStatus.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   if( uInitDone == 0 )
   {
       printf( "The DDR3 sodimm initialization is not done.\n"
               "Check if the DDR3 chip is correctly connected.\n"
               "The program will terminate. Push any key to continue\n"  );
       
       _getch();
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   else
   {
       printf( "\nThe DDR3 sodimm initialization is done.\n\n" );
   }

   // Display capture mode
   switch(channelc)
   {
		case 1:
           printf( "Recording bottom board 16 channels...\n");
		   break;

		case 2:
           printf( "Recording top board 16 channels...\n");
		   break;

		case 3:
           printf( "Recording bottom and top boards 32 channels (with decimation by 2)...\n");
		   break;
   }

   res = RecPlayResetCore( hRecPlay );

   if( res != 0 ) 
   {
       printf( "Error with RecPlayResetCore.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

    res = RTDExOpenEth( &hConn,
		                (unsigned char *)pHostMacAddr,
						eHost,
						(unsigned char *)pDeviceMacAddr,
                        hBusAccess,
                        module_rtdex_eth_base, 
						0,
						eFromFpga,
                        eSingle );

   if( res ) 
   {
      
       printf( "Error initializing the RTDEx connection.\n"
               "The program will terminate. Push any key to continue\n"  );
       _getch();
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );
       return -1;
   }

   // Set an external record core trigger controlled from mi125 core.
   res = RecPlaySetTriggerSource( hRecPlay, eRecPlayTrigExternal );

   if( res != 0 ) 
   {
       printf( "Error with RecPlaySetTriggerSource.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }
   printf( "Record size is %u samples per channel\n", CAPTURED_SAMPLES_PER_CHANNEL);
   printf( "Record size used is %u bytes\n",recordsize);

   res = RecPlaySetModeRecord( hRecPlay, 0,0, recordsize / 8 );

   if( res != 0 ) 
   {
       printf( "Error with RecPlaySetModeRecord.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   // Is this soft trig mode
   if (trigsourcec == 0)
   {
       printf("Applying software trigger\r\n");

	   // Generate a mi125 soft trig
	   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_2, 1);
	   if( res ) 
	   {
		   printf( "There was an error in mi125 soft trigger 1.\n\n"
				   "The program will terminate. Push any key to continue\n" );
		   _getch();
		   
		   RTDExCloseEth( hConn );
		   RecPlay_Close( hRecPlay );
		   BusAccess_Ethernet_Close( hBusAccess );
		   BUFFER_DESTROY( pRecordMem );
		   return -1;
	   }

	   // Reset mi125 soft trig
	   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_2,  0 );
	   if( res ) 
	   {
		   printf( "There was an error in mi125 soft trigger 0.\n\n"
				   "The program will terminate. Push any key to continue\n" );
		   _getch();
		   
		   RTDExCloseEth( hConn );
		   RecPlay_Close( hRecPlay );
		   BusAccess_Ethernet_Close( hBusAccess );
		   BUFFER_DESTROY( pRecordMem );
		   return -1;
	   }
   }
   else
   {
       printf("Waiting for trigger\r\n");
   }

   // Wait for the record transfer end.
   timeout = 0;
   do
   {
      Sleep( 100 );
      res = RecPlayGetTransferStatus( hRecPlay, &uTransferOver );
	  timeout++;
   }while(uTransferOver == 0 && res == 0 && timeout < 200);
   printf("Done.\r\n");

   if( res != 0 ) 
   {
       printf( "Error with RecPlayGetTransferStatus.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   if( uTransferOver == 0 )
   {
       printf( "The transfer is not over.\n"
               "The program will terminate. Push any key to continue\n"  );
       
       _getch();

       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }
   printf( "Record successfull\n\n");
   res = RecPlayGetTriggerPos( hRecPlay, &uAddrTrig, &uTrigOffset ); 

   printf( "Trigger position: 512 bits trig start address= %u,\n"
	   " trigger byte offset from start address= %u.\n",uAddrTrig, uTrigOffset);

   if( res != 0 ) 
   {
       printf( "Error with RecPlayGetTriggerPos.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   res = RTDExSetTxFrameGapValEth( hBusAccess, module_rtdex_eth_base, FRAME_GAP );

   if( res != 0 ) 
   {
       printf( "Error with SetRTDExEthTxFrameGapValEth.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   // Check if trigger position is aligned with a memory block? If not, extra bytes should be transferred
   // so the transferred buffer could be aligned correctly at the trigger position in host memory for the expected
   // saved acquision size
   if (uTrigOffset != 0)
	   needed_transfer = buffer_size;
   else
	   needed_transfer = buffer_size - addbytes;

   nframe_used = needed_transfer / FRAMESIZE;

   // Adjust the host buffer offsets according to trigger index position and desired data size
   // The effect here is to aligned the buffer saved section with the exact trigger position
   res = BUFFER_OFFSETS(pRecordMem, uTrigOffset, uTrigOffset + recordsize - 1);

   if (res != 0)
   {
       printf( "Buffer offsets configuration failed.\n"
               "The program will terminate. Push any key to continue\n"  );
       _getch();

       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   res = RecPlaySetModeRtdexMem2Host( hRecPlay,uAddrTrig, needed_transfer/8 );

   if( res != 0 ) 
   {
       printf( "Error with RecPlaySetModeRtdexMem2Host.\n"
              " The program will terminate. Push any key to continue\n" );
       _getch();
       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   // Compute an estimate of worst transfer time
   rxtime = (int)(((long long)(nframe_used) * FRAME_GAP / FRAME_GAP_CLK) +
	        ((long long)(nframe_used) * ((FRAMESIZE + PACKET_OVERHEAD + ETH_FRAME_OVERHEAD) * 8) / ETH_SPEED) + 1);

   // Transfer time estimation is around (if no pause frame is used): (Inter frame time + packet time) * nbr frame
   // This assumes that the host is able to process these packets at that speed...
   // This estimate assumes the full ethernet bandwidth of the module <-> host link can be used for data transfer
   // Performance could be worst if OS page fault in the buffer occurs
   printf( "MI125 Record transfert %u bytes, best time ~%u secs...\n\n", needed_transfer, rxtime );

   // Get the transfer data and wait until completed (it will hang if the transfer does not complete correctly)
   res = RTDExStart( hConn,
                     FRAMESIZE,
                     needed_transfer);

   if( res != 0 )
   {
       printf( "RTDEx transfer failed.\n");
	   printf( ErrorToString(res) );    
       printf( "The program will terminate. Push any key to continue\n"  );
       _getch();

       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }

   res = RTDExReceive( hConn,
					 (unsigned char*)pRecordBuffer,
                     needed_transfer, 
                     eRTDExWaitTimeout, rxtime * 1000 * 2);

   // Check if we got an error
   if( res < 0 )
   {
       printf( "RTDEx transfer failed.\n");
	   printf( ErrorToString(res) );    
       printf( "The program will terminate. Push any key to continue\n"  );
       _getch();

       RTDExCloseEth( hConn );
       RecPlay_Close( hRecPlay );
       BusAccess_Ethernet_Close( hBusAccess );
       BUFFER_DESTROY( pRecordMem );

       return -1;
   }
   else
   {
	   // Check if we received the expected size
	   if (res != needed_transfer)
	   {
		   unsigned int pVal=0;

		   // Did we lost some frames
		   res = RTDExGetRxFramesLostCountEth(hConn, &pVal);

		   if( res < 0 )
		   {
			   printf( "RTDEx transfer failed (RTDExGetRxFramesLostCountEth).\n"
					   "The program will terminate. Push any key to continue\n"  );
			   _getch();

			   RTDExCloseEth( hConn );
			   RecPlay_Close( hRecPlay );
			   BusAccess_Ethernet_Close( hBusAccess );
			   BUFFER_DESTROY( pRecordMem );

			   return -1;
		   }

		   if (pVal !=0)
		   {
			   printf( "WARNING: transfer frames lost: %d.\n" , pVal);
               printf( "The program will terminate. Push any key to continue\n"  );
		   }
		   else
		   {
			   printf( "WARNING: transfer not completed because of a small timeout.\n" );
               printf( "The program will terminate. Push any key to continue\n"  );
		   }
			   _getch();

			   RTDExCloseEth( hConn );
			   RecPlay_Close( hRecPlay );
			   BusAccess_Ethernet_Close( hBusAccess );
			   BUFFER_DESTROY( pRecordMem );

			   return -1;
	   }
	   else
	   {
		   printf( "Data received successfully.\n\n");
		   printf( "%u bytes will be saved to %s.\n\n", recordsize , RecordFileName);
	   }
   }

   RTDExCloseEth( hConn );
   RecPlay_Close( hRecPlay );
   BusAccess_Ethernet_Close( hBusAccess );
   
   res = BUFFER_DESTROY( pRecordMem );
   if( res != 0 )
   {
       printf( "Error with destroying record memory element.\n"
               "The program will terminate. Push any key to continue\n"  );
       _getch();

       return -1;
   }

   printf( "Press enter to terminate.\n");
   _getch();

   return 0;
}
