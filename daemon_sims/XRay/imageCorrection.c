//*******************************************************************************
//*                                                                             *
//*  PETBox Acquisition and Management Graphical User Interface                 *
//*  XRAY DEAMON Software                                                       *
//*                                                                             *
//*  Copyright - 2011                                                           *
//*                                                                             *
//*  This code is the property of Sofie Bioscience                              *
//*  No warranty, implicit or explicit, is given as to the the suitability      *
//*  of this program for any purpose.                                           *
//*                                                                             *
//*  Written by Shang Shang and Richard                                         *
//*                                                                             *
//*  Created: May 2011                                                          *
//*                                                                             *
//*  Change log:   Changed 6008 layout on 08/08/11								*
//*				   Add chamber present info read in ResponsetoInquireStatus		*
//*					on 10/17/2011												*
//*				  Produce uncropped image										*
//*                                                                             *
//*******************************************************************************


#include "windows.h"
#include <NIDAQmx.h>
#include <tcpsupp.h>
#include <mmsystem.h>  
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "PGRFlyCapture.h" 
#include "Scilib21.h"

#include "xray_main.h"
#include "xrayDaemon.h" 
#include "GlobalDecls.h"
#include "XRAY_SERVER.h"


/* image correction related prototypes*/
void imageCorrection (unsigned short* ImgBuf);
void pinhole_Xray ( unsigned short* imgBuf);
void pinhole_SidePhoto ( unsigned char* imgBuf);
unsigned short Interpolation_Short ( unsigned short* imgBuf, int lWidth, int lHeight, float x, float y);
unsigned char Interpolation_Char ( unsigned char* imgBuf, int lWidth, int lHeight, float x, float y);
void pinhole_Xray_Para_Initial(void);
void pinhole_Xray_Para_Initial(void);

static char tempString[2048];	   // local temporary work string to be used with sprintf, etc 

/* Image correction, including pixel correction, offset correction and gain correction*/
void imageCorrection (unsigned short* imgBuf)
{
		//local variables
		unsigned long nBufSize = MAX_IMAGE_LENGTH;		//buffer size for xray image
		unsigned long gainBufSize = MAX_IMAGE_LENGTH;   //buffer size for gain image
		unsigned long offsetBufSize = MAX_IMAGE_LENGTH;  //buffer size for offset image
		unsigned int nWidth = IMAGE_WIDTH;		 //image wide
		unsigned int nHeight = IMAGE_HEIGHT;	 //image height
		
		unsigned int EntrySize = 2000;
		short errcode;
		unsigned long nBytes;
		
		short* gainImgBuf = (short*)malloc(gainBufSize);    //gain image buffer
		short* offsetImgBuf = (short*)malloc(offsetBufSize); //offset image buffer
		
		//image transformation
		
		for (int i=0; i<nBufSize/2; i++) 
    		imgBuf[i] = (((unsigned short)imgBuf[i])>>8) + (((unsigned char)imgBuf[i])<<8);
		
		
		/*offset correction*/
		
		// read offset image. note that the offset image used here should have pixel correction done.
		sprintf(tempString, "%s%s", systemFilesPath, correctedOffsetImageFilePath); 
		HANDLE oFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		
		if(oFile == INVALID_HANDLE_VALUE)
		{
			writeToLog("Open corrected offset image error.");
 		}
		errcode=ReadFile(oFile, offsetImgBuf, offsetBufSize-1, &nBytes, NULL) ;  
	
		//image transformation for offset image
		for (int i=0; i<offsetBufSize/2; i++)

        offsetImgBuf[i] = (((unsigned short)offsetImgBuf[i])>>8) + (((unsigned char)offsetImgBuf[i])<<8);
		
		//offset correction
		errcode =ScOffsetCorrection(imgBuf, offsetImgBuf, nWidth, nHeight);
		if (errcode !=0)
		{  ScErrorMessage (errcode, errMessage);
			writeToLog( errMessage);
		}	 
		
		/*gain correction*/
		
		//open gain image. note that the gain image used here should have pixel and offset correction done.
		sprintf(tempString, "%s%s", systemFilesPath, correctedGainImageFilePath);
		HANDLE gFile = CreateFile (tempString,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 		
		if(gFile==INVALID_HANDLE_VALUE)
		{
		 writeToLog("Open corrected gain image error.");	
		}
		errcode=ReadFile(gFile, gainImgBuf, gainBufSize-1, &nBytes, NULL) ;
		
		//image transformation for gain image
		for (int i=0; i<gainBufSize/2; i++)

        gainImgBuf[i] = (((unsigned short)gainImgBuf[i])>>8) + (((unsigned char)gainImgBuf[i])<<8);
		
		//gain correction
		errcode =ScGainCorrection(imgBuf, gainImgBuf, nWidth, nHeight,0);
		if (errcode !=0)
		{  ScErrorMessage (errcode, errMessage);
			writeToLog( errMessage);
		} 	  
				
		// Pixel correction
		sprintf(tempString, "%s%s", systemFilesPath, detectorPixelMap);
		char *filePath = tempString;		 // file path to detecotor's pixel map
		PIXMAPENTRY *pixMap;
		pixMap = (PIXMAPENTRY*)malloc(SCMAXPIXMAPSIZE * sizeof(PIXMAPENTRY));
	
		int *numEntries = (int *)malloc(EntrySize);

		errcode = ScReadPixMap(filePath, pixMap, numEntries);	   // read pixel map
		if (errcode !=0)
		{  ScErrorMessage (errcode, errMessage);
			writeToLog(errMessage);
		}
		
		unsigned short nPixMapCount= *numEntries;
		
		/*choose correction method"SCMETEOD_GRADIENT" */
		unsigned short pcMethod = SCMETHOD_GRADIENT;
		errcode = ScPixelCorrection(imgBuf, nWidth, nHeight, pixMap, nPixMapCount, pcMethod);		 // pixel correction

		if (errcode !=0)
		{  ScErrorMessage (errcode, errMessage);
			writeToLog( errMessage);
		} 
		
		//transform image back after all corrections done
		for (int i=0; i<nBufSize/2; i++) 
             imgBuf[i] = (((unsigned short)imgBuf[i])>>8) + (((unsigned char)imgBuf[i])<<8);
		
		//free buffer
		free(offsetImgBuf);
		free(gainImgBuf);
		free(pixMap); 
		free(numEntries); 
		
		
}

/* xray pinhole correction and crop parameter initilization*/

/*
Para_Xray: a structure of the intrinsic and extrinsic parameters
fl: the physical focal length
Spc_Out: the output pixel spacing of the 2D image
BndBx_Out: the bounding box region of output image 
KK: 3*3 transformation matrix
Dim_Xray_in: dimention of xray image before pinhole
Dim_Xray_Out: dimention of xray image after pinhole
*/

void pinhole_Xray_Para_Initial()
{
		float BndBx_Out[4];
		float fl;
		float x_sz, y_sz;
		float Spc_Out[2]; 
		float KK[3][3]={0};
		int i,j;  

 		x_sz = Dim_Xray_in[0] * Spc_Xray_in[0];
   		y_sz = Dim_Xray_in[1] * Spc_Xray_in[1];
		BndBx_Out[0] = BndBx_Xray[0] * x_sz;
		BndBx_Out[1] = BndBx_Xray[1] * x_sz;
		BndBx_Out[2] = BndBx_Xray[2] * y_sz;
		BndBx_Out[3] = BndBx_Xray[3] * y_sz;
		fl = (para_Xray.fc[0]*Spc_Xray_in[0]+ para_Xray.fc[1]*Spc_Xray_in[1])/2;
		Spc_Out[0] = Spc_Xray_out[0];
		Spc_Out[1] = Spc_Xray_out[1];
		Dim_In_xray[0] = Dim_Xray_in[0];
		Dim_In_xray[1] = Dim_Xray_in[1];
			 
		KK[0][0]= para_Xray.fc[0];
 		KK[0][1]= para_Xray.alpha_c*para_Xray.fc[0];
  		KK[0][2]= para_Xray.cc[0];
  		KK[1][1]= para_Xray.fc[1];
 		KK[1][2]= para_Xray.cc[1];
        KK[2][2]=1;
			 
 		Dim_Out_xray[0]=floor((BndBx_Out[1]-BndBx_Out[0])/Spc_Out[0]);
  		Dim_Out_xray[1]=floor((BndBx_Out[3]-BndBx_Out[2])/Spc_Out[1]);    
  		numPix_out_xray = Dim_Out_xray[0]*Dim_Out_xray[1];
  
		//The output image pixel coordinates on the imaging plane

 		float **cord = (float **)malloc(sizeof(float*) * numPix_out_xray); 
  		for(i=0;i<numPix_out_xray;i++)
	 		cord[i] = (float*)malloc(sizeof(float) *2) ;    
  		for (i=0;i<Dim_Out_xray[1]; i++)
	 	for(j=0;j<Dim_Out_xray[0];j++)
  			{   
	  		cord[(i*Dim_Out_xray[0]+j)][0] = (BndBx_Out[0] + j*Spc_Out[0])/fl;
	 		cord[(i*Dim_Out_xray[0]+j)][1] = (BndBx_Out[2] + i*Spc_Out[1])/fl;
  			};
 
		//distortion
  		float **cord2 = (float **)malloc(sizeof(float*) * numPix_out_xray);
  		for (i=0;i<numPix_out_xray; i++)
  		{
	  		cord2[i] = (float*)malloc(sizeof(float) *2) ;
	  		*(*(cord2+i)) = (*(*(cord+i))) * (*(*(cord+i))) ;
	  		*(*(cord2+i)+1) = (*(*(cord+i)+1)) * (*(*(cord+i)+1)) ;
  		}

  		float *r2 = (float *)malloc(sizeof(float)*numPix_out_xray);
  		float *xy = (float *)malloc(sizeof(float)*numPix_out_xray);
  		for (i=0;i<numPix_out_xray; i++)
  		{
  			r2[i]=(*(*(cord2+i))) + (*(*(cord2+i)+1));
  			xy[i] =(*(*(cord+i))) * (*(*(cord+i)+1));		 
  		}

  		float tempPara0, tempPara1,tempPara2;		  
  
  		for (i=0;i<numPix_out_xray; i++)
  		{
	  		tempPara0 = 1+para_Xray.kc[0]*r2[i] + para_Xray.kc[1]*r2[i]*r2[i] +para_Xray.kc[4]*r2[i]*r2[i]*r2[i];		//0.998 ;46862.875
	  		tempPara1 = 2 * para_Xray.kc[2]*xy[i] + para_Xray.kc[3]*(r2[i]+2 *cord2[i][0]);   //-0.0000662
	  		tempPara2 = 2 * para_Xray.kc[3]*xy[i]+para_Xray.kc[2]* (r2[i]+2*cord2[i][1]);	  //0.0000184
	  		cord[i][0] = cord[i][0] * tempPara0 + tempPara1;
	  		cord[i][1] = cord[i][1] * tempPara0 + tempPara2;
	  
  		}	 	 

  		for (i=0; i<numPix_out_xray; i++)
  		{
	  		tempPara0 = cord[i][0] * KK[0][0] + cord[i][1] * KK[0][1] + KK[0][2];
	  		tempPara1 = cord[i][0] * KK[1][0] + cord[i][1] * KK[1][1] + KK[1][2];
	  		cord[i][0] = tempPara0-1;
	  		cord[i][1] = tempPara1-1;
  		}
		
		// transfer coordinates to cord_xray
  		cord_xray=cord;

  		free(cord2);
    	free(xy);
  		free(r2);
	
}

/* Camera pinhole correction and crop parameter initilization*/

/*
Para_Cam: a structure of the intrinsic and extrinsic parameters
fl: the physical focal length
Spc_Out: the output pixel spacing of the 2D image
BndBx_Out: the bounding box region of output image 
KK: 3*3 transformation matrix
Dim_Cam_in: dimention of side photo before pinhole
Dim_Cam_Out: dimention of side photo after pinhole
*/
void pinhole_Cam_Para_Initial() 

{
		float BndBx_Out[4];
		float fl;
		float x_sz, y_sz;
		float Spc_Out[2]; 
		float KK[3][3]={0};
		int i,j;
	
   		x_sz = Dim_Cam_in[0] * Spc_Cam_in[0];
   		y_sz = Dim_Cam_in[1] * Spc_Cam_in[1];
		BndBx_Out[0] = BndBx_Cam[0] * x_sz;
		BndBx_Out[1] = BndBx_Cam[1] * x_sz;
		BndBx_Out[2] = BndBx_Cam[2] * y_sz;
		BndBx_Out[3] = BndBx_Cam[3] * y_sz;
		fl = (para_Cam.fc[0]*Spc_Cam_in[0]+ para_Cam.fc[1]*Spc_Cam_in[1])/2;
		Spc_Out[0] = Spc_Cam_out[0];
		Spc_Out[1] = Spc_Cam_out[1];
		Dim_In_cam[0] = Dim_Cam_in[0];
		Dim_In_cam[1] = Dim_Cam_in[1];
			 
		KK[0][0]= para_Cam.fc[0];
 		KK[0][1]= para_Cam.alpha_c*para_Cam.fc[0];
  		KK[0][2]= para_Cam.cc[0];
  		KK[1][1]= para_Cam.fc[1];
 		KK[1][2]= para_Cam.cc[1];
        KK[2][2]= 1;	 

  		Dim_Out_cam[0]=floor((BndBx_Out[1]-BndBx_Out[0])/Spc_Out[0]);
  		Dim_Out_cam[1]=floor((BndBx_Out[3]-BndBx_Out[2])/Spc_Out[1]);    
 
  		numPix_out_cam = Dim_Out_cam[0]*Dim_Out_cam[1];
  
		//The output image pixel coordinates on the imaging plane

  		float **cord = (float **)malloc(sizeof(float*) * numPix_out_cam);
  
  		for(i=0;i<numPix_out_cam;i++)
	 	cord[i] = (float*)malloc(sizeof(float) *2) ;   
  		for (i=0;i<Dim_Out_cam[1]; i++)
	  		for(j=0;j<Dim_Out_cam[0];j++)
  				{   
	  				cord[(i*Dim_Out_cam[0]+j)][0] = (BndBx_Out[0] + j*Spc_Out[0])/fl;
	 				cord[(i*Dim_Out_cam[0]+j)][1] = (BndBx_Out[2] + i*Spc_Out[1])/fl;
  				};
 
		//distortion
  		float **cord2 = (float **)malloc(sizeof(float*) * numPix_out_cam);
  		for (i=0;i<numPix_out_cam; i++)
  		{
	  		cord2[i] = (float*)malloc(sizeof(float) *2) ;
	  		*(*(cord2+i)) = (*(*(cord+i))) * (*(*(cord+i))) ;
	  		*(*(cord2+i)+1) = (*(*(cord+i)+1)) * (*(*(cord+i)+1)) ;
  		}

  		float *r2 = (float *)malloc(sizeof(float)*numPix_out_cam);
  		float *xy = (float *)malloc(sizeof(float)*numPix_out_cam);
  		for (i=0;i<numPix_out_cam; i++)
  		{
  			r2[i]=(*(*(cord2+i))) + (*(*(cord2+i)+1));
  			xy[i] =(*(*(cord+i))) * (*(*(cord+i)+1));		 
  		}

  		float tempPara0, tempPara1,tempPara2;
  		for (i=0;i<numPix_out_cam; i++)
  		{
	  		tempPara0 = 1+para_Xray.kc[0]*r2[i] + para_Xray.kc[1]*r2[i]*r2[i] +para_Xray.kc[4]*r2[i]*r2[i]*r2[i];		//0.998 ;46862.875
	  		tempPara1 = 2 * para_Xray.kc[2]*xy[i] + para_Xray.kc[3]*(r2[i]+2 *cord2[i][0]);   //-0.0000662
	  		tempPara2 = 2 * para_Xray.kc[3]*xy[i]+para_Xray.kc[2]* (r2[i]+2*cord2[i][1]);	  //0.0000184
	  		cord[i][0] = cord[i][0] * tempPara0 + tempPara1;
	  		cord[i][1] = cord[i][1] * tempPara0 + tempPara2;  
 		}	 
  		for (i=0; i<numPix_out_cam; i++)
  		{
	  		tempPara0 = cord[i][0] * KK[0][0] + cord[i][1] * KK[0][1] + KK[0][2];
	  		tempPara1 = cord[i][0] * KK[1][0] + cord[i][1] * KK[1][1] + KK[1][2];
	  		cord[i][0] = tempPara0-1;
	  		cord[i][1] = tempPara1-1;
  		}
		//transfer coorinates to cor_cam
  		cord_cam = cord;
 
  		free(cord2);
  		free(xy);
  		free(r2);
	
}

//pinhole correction for x_ray image
void pinhole_Xray ( unsigned short* imgBuf)	
{ 
   		int i,j;
  
  		for (int i=0; i<nBufSize/2; i++)   
        	imgBuf[i] = (((unsigned short)imgBuf[i])>>8) + (((unsigned char)imgBuf[i])<<8);
 		// image reverse  
  		unsigned short *r_imgBuf;
  		r_imgBuf = (unsigned short*)malloc(sizeof(unsigned short)*Dim_In_xray[0]*Dim_In_xray[1]);
  		for (i=0; i< Dim_In_xray[0]; i++)
	  		for (j=0; j<Dim_In_xray[1]; j++)
				{  r_imgBuf[j*Dim_In_xray[0]+i] = imgBuf[(Dim_In_xray[1]-j-1)*Dim_In_xray[0]+i] ;} 

 		//interpolation  
  		unsigned short temp;
  		for (i=0; i<numPix_out_xray; i++)
  			{
 				imgBuf[i]= Interpolation_Short ( r_imgBuf, Dim_In_xray[0], Dim_In_xray[1], cord_xray[i][0], cord_xray[i][1]); 
  			}
  
  		for (int i=0; i<nBufSize/2; i++)   
        	imgBuf[i] = (((unsigned short)imgBuf[i])>>8) + (((unsigned char)imgBuf[i])<<8);
 
  		free(r_imgBuf);		  
}


		//pinhole correction for side photo
void pinhole_SidePhoto ( unsigned char* imgBuf)		 
{
   		int i,j;
     
		// reverse the image
  		unsigned char *r_imgBuf;

  		r_imgBuf = (unsigned char*)malloc(sizeof(unsigned char)*Dim_In_cam[0]*Dim_In_cam[1]);
  		for (i=0; i< Dim_In_cam[0]; i++)
	  		for (j=0; j<Dim_In_cam[1]; j++)
		{  r_imgBuf[j*Dim_In_cam[0]+i] = imgBuf[(Dim_In_cam[1]-j-1)*Dim_In_cam[0]+i] ;} 

		//interpolate  
  		unsigned char temp;
  		for (i=0; i<numPix_out_cam; i++)
  		{
 			imgBuf[i]= Interpolation_Char ( r_imgBuf, Dim_In_cam[0], Dim_In_cam[1], cord_cam[i][0], cord_cam[i][1]); 
  		}  	 
   
  		free(r_imgBuf);
		}

// interpolation for unsigned short image type (x-ray image)
unsigned short Interpolation_Short ( unsigned short* imgBuf, int lWidth, int lHeight, float x, float y)
{
		// four adjacent coordinates
		int i1, i2;
		int j1, j2;
	
		// four adjacent pixel
		unsigned short f1,f2,f3,f4;
	
		// two interpolated pixel
		unsigned short f12, f34;
	
		// two coords are supposed to be the same if their difference is less then EXP
		float EXP;
		EXP = 0.0001;
	
		//calculate four adjacent coods
		i1 = (int)x;
		i2 = i1+1;
		j1 = (int)y;
		j2 = j1+1;
	
		// if the point is out of the image, return 0
		if ((x<0)||(x > lWidth-1)||(y<0)||(y>lHeight-1))	   //*****
			{ return 0;}
	
		else
		{if (fabs(x-lWidth+1)<=EXP) 
			{
		// if the point is at the right edge of the image
				if (fabs(y-lHeight+1)<=EXP)
					{
		//the point is at the right down corner of the image, return the original value
						f1 = *((unsigned short *)imgBuf+lWidth * j1 + i1);
						return f1;
					}
				else
					{
						f1 = *((unsigned short *)imgBuf + lWidth * j1+i1);
						f3 = *((unsigned short *)imgBuf + lWidth * j1 + i2);
						return ((unsigned short)(f1+(y-j1)*(f3-f1)));
					}
			}
	
		else if (fabs(y-lHeight +1)<=EXP)
 		{  
		//the point is at the lower edge of the image, but not the last point
	 		f1 = *((unsigned short *)imgBuf + lWidth * j1 + i1);
	 		f2 = *((unsigned short *)imgBuf + lWidth * j2 + i1);		 //*** char to short
	 		return ((unsigned short) (f1+(x-i1)*(f2-f1)));
 		}
 		else
 		{
		 f1 = imgBuf[lWidth * j1+i1];
	     f2 = imgBuf[lWidth * j2+i1];
		 f3 = imgBuf[lWidth * j1+i2];
		 f4 = imgBuf[lWidth * j2+i2];
		 
		 f12=(unsigned short)(f1+(x-i1)*(f2-f1));	  
		 f34=(unsigned short)(f3+(x-i1)*(f4-f3));
		 
		 return ((unsigned short)(f12+(y-j1)*(f34-f12)));
		} }}
			

		//interpolcation for char image type (side photo)
unsigned char Interpolation_Char ( unsigned char* imgBuf, int lWidth, int lHeight, float x, float y)
{
		// four adjacent coordinates
		int i1, i2;
		int j1, j2;
	
		// four adjacent pixel
		unsigned char f1,f2,f3,f4;
	
		// two interpolated pixel
		unsigned char f12, f34;
		
		// two coords are supposed to be the same if their difference is less then EXP
		float EXP;
		EXP = 0.0001;
	
		//calculate four adjacent coods
		i1 = (int)x;
		i2 = i1+1;
		j1 = (int)y;
		j2 = j1+1;
	
		// if the point is out of the image, return 0
		if ((x<0)||(x > lWidth-1)||(y<0)||(y>lHeight-1))	   //*****
			{ return 0;}
	
		else
			{if (fabs(x-lWidth+1)<=EXP) 
				{
					// if the point is at the right edge of the image
					if (fabs(y-lHeight+1)<=EXP)
						{
							//the point is at the right down corner of the image, return the original value
							f1 = *((unsigned char *)imgBuf+lWidth * j1 + i1);
							return f1;
						}
					else
						{
							f1 = *((unsigned char *)imgBuf + lWidth * j1+i1);
							f3 = *((unsigned char *)imgBuf + lWidth * j1 + i2);
							return ((unsigned char)(f1+(y-j1)*(f3-f1)));
						}
			}
	
 		else if (fabs(y-lHeight +1)<=EXP)
 			{   
				//the point is at the lower edge of the image, but not the last point
	 			f1 = *((unsigned char *)imgBuf + lWidth * j1 + i1);
	 			f2 = *((unsigned char *)imgBuf + lWidth * j2 + i1);		 //*** char to short
	 			return ((unsigned char) (f1+(x-i1)*(f2-f1)));
 			}
 		else
 			{
		 		f1 = imgBuf[lWidth * j1+i1];
	     		f2 = imgBuf[lWidth * j2+i1];
		 		f3 = imgBuf[lWidth * j1+i2];
		 		f4 = imgBuf[lWidth * j2+i2];
		 
		 		f12=(unsigned char)(f1+(x-i1)*(f2-f1));	  
		 		f34=(unsigned char)(f3+(x-i1)*(f4-f3));
		 
		 		return ((unsigned char)(f12+(y-j1)*(f34-f12)));
			}}}
		
