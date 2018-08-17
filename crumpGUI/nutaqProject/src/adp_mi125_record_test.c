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
 * Test version of adp_mi125_record_test.c to transition to RTDEx 
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

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


// WARNING: to run this demo under LINUX, you may need root privilege

#define BUFFER_CREATE_STRUCT CreateTransferBuffData
#define BUFFER_OPEN AdpOpenBuffMem
#define BUFFER_CREATE AdpCreateBuffMem
#define BUFFER_DESTROY AdpDestroyBuffMem
#define BUFFER_GETDATA AdpGetBuffData
#define BUFFER_OFFSETS(pTransfer, StartOff, EndOff) AdpOffsetsBuffMem((pTransfer), (StartOff), (EndOff))

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

#define iMaxt 300
#define numAcqBuffer 64
#define acqBufferSize 131072 //1051008 //131072;

int InitMi125( adp_handle_bus_access hBusAccess, unsigned nb, unsigned int clocksource);

int main( int argc, char* argv[] )
{
   adp_result_t res;
   int res1;
   adp_handle_rtdex_conn hConn = NULL;
   adp_handle_bus_access hBusAccess = NULL;
   adp_handle_recplay hRecPlay = NULL;

FILE *fp1;
char HuiFileName[]="C:\\Nutaq\\ADP6\\ADP_MicroTCA\\examples_perseus6010\\perseus6010_mi125_record_playback\\bin\\buffer1.dat"; // argv[3]
PUCHAR m_acqBuffer;
PUCHAR m_acqReadPointer;
PUCHAR m_acqWritePointer;
unsigned int size_w=0;
////unsigned int numAcqBuffer=64;
////unsigned int acqBufferSize=1051008; //131072;

char Channels[]="1"; //argv[1]
char TrigSource[]="0"; // argv[2]
char RecordFileName[]="C:\\Nutaq\\ADP6\\ADP_MicroTCA\\examples_perseus6010\\perseus6010_mi125_record_playback\\bin\\Hui15_3.bin"; // argv[3]
char IpAddress[]="192.168.0.101"; // argv[4]
char ClkSrc[]="0"; // argv[5]
char CHAR_CAPTURED_SAMPLES_PER_CHANNEL[]="32768";     //"65536"; // atoi(argv[6])
int CAPTURED_SAMPLES_PER_CHANNEL=atoi(CHAR_CAPTURED_SAMPLES_PER_CHANNEL);
//unsigned int *pValue = NULL;
//uint32_t pValue = 9999;
unsigned int pValue;
unsigned int ii,tcount;

   ////unsigned char uInitDone;
   ////unsigned char uTransferOver;
   ////unsigned int uAddrTrig;
   unsigned char uTrigOffset;

   unsigned int* pRecordBuffer;

   unsigned int testit0[300];
   unsigned int testit1[300];
   unsigned int testit2[300];
   unsigned int jj;
   void* pRecordMem;
   unsigned int i,channelc, trigsourcec, timeout, clocksource;
   unsigned int recordsize = 0; 
   char pHostMacAddr[6];
   char pDeviceMacAddr[6];
   int rxtime;
   unsigned int needed_transfer, addbytes, nframe_used;
   unsigned int buffer_size;

   for (ii=0;ii<300;ii++)
	{
		testit0[ii]=999;
		testit1[ii]=888;
		testit2[ii]=777;
   }

   // Initialize eapi before use 
   res = eapi_init();
   channelc = atoi(Channels);
   trigsourcec = atoi(TrigSource);
   clocksource = atoi(ClkSrc);
   pRecordMem = BUFFER_CREATE_STRUCT();

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
   printf( "Res allocating memory for the Record File should be 0. %u  \n",res);
   // Get memory buffers pointers for playback and records
   pRecordBuffer = (unsigned int*) BUFFER_GETDATA( pRecordMem );
   printf( "Res allocating memory for the Record File should be different from 0. %u  \n",pRecordBuffer);
   
   //open ethernet connection here
   res  = BusAccess_Ethernet_OpenWithoutConnection(&hBusAccess, (int8_t *)IpAddress, 0);
   printf( "Res allocating memory for the Record File should be 0. %u  \n",res);   
   // get MAC address of target and host:
   res = iGetConnectionPeerMacAddr( hBusAccess, (unsigned char *)pDeviceMacAddr);
   printf( "Res MAC address target should be 0. %u  \n",res);
   printf( "MAC address of Perseus is: '");
   for( i = 0; i < 5; i++)
       printf( "%02X:", (unsigned char)pDeviceMacAddr[ i ]);
       printf( "%02X'\n", (unsigned char)pDeviceMacAddr[ i ]);

   res = iGetConnectionLocalMacAddr( hBusAccess, (unsigned char *)pHostMacAddr);
   printf( "Res MAC address host should be 0. %u  \n",res);

   printf( "MAC Address of Host is: '");
   for( i = 0; i < 5; i++)
       printf( "%02X:", (unsigned char)pHostMacAddr[ i ]);
   printf( "%02X'\n\n", (unsigned char)pHostMacAddr[ i ]);
   //here finished opening the ehternet connection
 
 
   // Initialized all needed mi125 boards to capture
   for (i=1; i <= (1 + (unsigned int)(channelc > 1)); i++)
   {
	   res = InitMi125( hBusAccess,  i, clocksource);
	   printf( "Res Init Mi125 should be 0. %u  \n",res);
   }
	

   //Reset RTDEx Core then RTDEx Open and Frame Gap Val
   res = RTDExResetCoreEth(hBusAccess,module_rtdex_eth_base); 
   
    res = RTDExOpenEth( &hConn,
		                (unsigned char *)pHostMacAddr,
						eHost,
						(unsigned char *)pDeviceMacAddr,
                        hBusAccess,
                        module_rtdex_eth_base, 
						0,
						eFromFpga,
                        eContinuous );  //eContinuous eSingle
   printf( "Res RTDExOpenEth( should be 0. %u  \n",res);
   res = RTDExSetTxFrameGapValEth( hBusAccess, module_rtdex_eth_base, FRAME_GAP );

   //write and read to registers
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  (unsigned int) 0 ); //run off
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_0,  &pValue );
   printf("Acq ON: %u \n",pValue);
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_2,  (unsigned int) 1 ); //reset on
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_2,  &pValue );
   printf("Reset ON: %u \n",pValue);
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_2,  (unsigned int) 0 ); //reset off
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_2,  &pValue );
   printf("Reset ON: %u \n",pValue);
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 500 ); //threshold set to 500
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
   printf("Threshold: %u \n",pValue);
//Before Acquistion
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_6,  &pValue );
   printf("Counter (6): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_12,  &pValue );
   printf("Live Counter (12): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
   printf("FIFO full (8): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_11,  &pValue );
   printf("Trigger (11): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_9,  &pValue );
   printf("Live Trigger (9): %u \n",pValue);

      //Turn on Acquisition
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  (unsigned int) 1 ); //acquisition on
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_0,  &pValue );
   printf("Acq ON: %u \n",pValue);
   //finished reading and writing to registers and turning on the Acquistion
//Here try to open a new file are instead write data to this
	if((fp1=fopen(HuiFileName,"wb"))==NULL)
	{
		fprintf(stderr,"\nError opening file.");
	}
    m_acqBuffer=(PUCHAR)malloc(numAcqBuffer*acqBufferSize);
	memset(m_acqBuffer,0,numAcqBuffer*acqBufferSize);  /* 64*acqBufferSize */
    m_acqReadPointer=m_acqBuffer;
	m_acqWritePointer=m_acqBuffer;
 ////   size_w=fwrite(m_acqWritePointer,sizeof(char),numAcqBuffer/2*acqBufferSize,fp1);
 ////   if(size_w!=acqBufferSize*numAcqBuffer/2)
	////{
	////	fprintf(stderr,"\nError write data to file buffer.dat.");
	////	exit(1);
	////}
//Two loops to read out the RTDEx
for (ii=1; ii <= 120; ii++)
   {
	   printf("ii in loop: %u \n",ii);
	
   
   //Read Registers Just after Acquisition turned on	
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_6,  &pValue );
   //printf("Counter (6): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_12,  &pValue );
   //printf("Live Counter (12): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
   //printf("FIFO full in loop (8): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_11,  &pValue );
   //printf("Trigger (11): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_9,  &pValue );
   ////printf("Live Trigger (9): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_7,  &pValue );
   printf("Trigger Rate (7): %u \n",pValue);

   timeout = 0;
  
   //Stuff used in RecPlay.....
   // Check if trigger position is aligned with a memory block? If not, extra bytes should be transferred
   // so the transferred buffer could be aligned correctly at the trigger position in host memory for the expected
   // saved acquision size
  // if (uTrigOffset != 0)
	   needed_transfer = buffer_size;
   //else
	  // needed_transfer = buffer_size - addbytes;

   nframe_used = needed_transfer / FRAMESIZE;
   uTrigOffset=0; //added this !!!!!!!!!
   // Adjust the host buffer offsets according to trigger index position and desired data size
   // The effect here is to aligned the buffer saved section with the exact trigger position
    res = BUFFER_OFFSETS(pRecordMem, uTrigOffset, uTrigOffset + recordsize - 1);
   //removed set mode for RecPlay

   // Compute an estimate of worst transfer time
   rxtime = (int)(((long long)(nframe_used) * FRAME_GAP / FRAME_GAP_CLK) +
	        ((long long)(nframe_used) * ((FRAMESIZE + PACKET_OVERHEAD + ETH_FRAME_OVERHEAD) * 8) / ETH_SPEED) + 1);

   // Transfer time estimation is around (if no pause frame is used): (Inter frame time + packet time) * nbr frame
   // This assumes that the host is able to process these packets at that speed...
   // This estimate assumes the full ethernet bandwidth of the module <-> host link can be used for data transfer
   // Performance could be worst if OS page fault in the buffer occurs
////   printf( "MI125 Record transfert %u bytes, best time ~%u secs...\n\n", needed_transfer, rxtime );
   //finish stuff from RecPlay


   // Get the transfer data and wait until completed (it will hang if the transfer does not complete correctly)
    pValue=0;
	tcount=0;
	while(pValue==0)
	{
	   Sleep(1);
       res1 = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
       printf("FIFO full (8): %u \n",pValue);
	   tcount=tcount+1;
	   if(tcount > 200)
	   {
		  fprintf(stderr,"\nError write data to file buffer.dat.");
//		  exit(1);
          res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 400 ); //threshold set to 200
		  res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
	   }
	}    
    if (ii==1)
	{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 400 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		printf("Threshold: %u \n",pValue);
	}
	 if (ii==2)
	{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 300 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		printf("Threshold: %u \n",pValue);
	}
	  if (ii==3)
	{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 100 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		printf("Threshold: %u \n",pValue);
	}
	  if (ii==4)
	{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 300 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		printf("Threshold: %u \n",pValue);
	}
  	  if (ii==5)
	{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 500 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		printf("Threshold: %u \n",pValue);
	}
	if (ii==6)
	{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 3000000 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		printf("Threshold: %u \n",pValue);
	}
	if (ii>6)
	{
		if(ii%2==1)
		{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 300 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		//printf("Threshold: %u \n",pValue);
		}
		else
		{
		res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_3,  (unsigned int) 500 ); //threshold set to 200
		res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_3,  &pValue );
		//printf("Threshold: %u \n",pValue);
		}
	}

   res1 = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
   //printf("After Setting Threshold FIFO full (8): %u \n",pValue);
   for (jj=0; jj <= 199; jj++)
   {
	testit0[jj]=*(pRecordBuffer+jj);
   }

   ////res = RTDExStart( hConn,
   ////                  FRAMESIZE,
   ////                  needed_transfer);
   ////
   ////res = RTDExReceive( hConn,
			////		 (unsigned char*)pRecordBuffer,
   ////                  needed_transfer, 
   ////                  eRTDExWaitTimeout, rxtime * 1000 * 2);

			res = RTDExStart( hConn,
                     FRAMESIZE,
                     acqBufferSize);
   
			res = RTDExReceive( hConn,
					 m_acqReadPointer,
                     acqBufferSize, 
                     eRTDExWaitTimeout, rxtime * 1000 * 2);

    needed_transfer=acqBufferSize;
    size_w=fwrite(m_acqWritePointer,sizeof(char),acqBufferSize,fp1);
    if(size_w!=acqBufferSize)
	{
		fprintf(stderr,"\nError write data to file buffer.dat.");
		exit(1);
	}
    for (jj=0; jj <= 199; jj++)
   {
	    if (ii==1)
		{
			testit1[jj]=*(pRecordBuffer+jj);
		}
		else
		{
			testit2[jj]=*(pRecordBuffer+jj);
		}
	}
    //try to open file and write out the array of 

   res1 = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
   printf("Before Reset FIFO full (8): %u \n",pValue);
    res1 = RTDExResetFifoEth(hConn);
   res1 = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
   printf("After Reset FIFO full (8): %u \n",pValue);

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

   }
//After reading the RTDEx 
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_6,  &pValue );
   printf("Counter (6): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_12,  &pValue );
   printf("Live Counter (12): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_8,  &pValue );
   printf("FIFO full (8): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_11,  &pValue );
   printf("Trigger (11): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_9,  &pValue );
   printf("Live Trigger (9): %u \n",pValue);
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_7,  &pValue );
   printf("Trigger Rate (7): %u \n",pValue);

   //Stop Acquistion
   res = BusAccess_WriteReg( hBusAccess, TARGET_CUSTOMREGS_0,  (unsigned int) 0 ); //run off
   res = BusAccess_ReadReg( hBusAccess, TARGET_CUSTOMREGS_0,  &pValue );

   RTDExCloseEth( hConn );
   BusAccess_Ethernet_Close( hBusAccess );
   
   res = BUFFER_DESTROY( pRecordMem );
   

   fclose(fp1); //close the new file buffer.dat

   printf( "Press enter to terminate.\n");
   _getch();

   return 0;
}
