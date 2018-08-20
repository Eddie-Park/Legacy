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
//*  Change log:                                                                *
//*                                                                             *
//*******************************************************************************



#include "windows.h"
#include <tcpsupp.h>
#include <mmsystem.h>  
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <analysis.h>

#include "Headers.h"
#include "atlasRegistration.h" 
#include "GlobalDecls.h" 
#include "AtlasRegistrationMain.h" 



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

	x_sz = Dim_Xray_Detector[0] * Spc_Xray_Detector[0];
	y_sz = Dim_Xray_Detector[1] * Spc_Xray_Detector[1];
	BndBx_Out[0] = BndBx_Xray_Reg[0] * x_sz;
	BndBx_Out[1] = BndBx_Xray_Reg[1] * x_sz;
	BndBx_Out[2] = BndBx_Xray_Reg[2] * y_sz;
	BndBx_Out[3] = BndBx_Xray_Reg[3] * y_sz;
	fl = (Para_Xray.fc[0]*Spc_Xray_Detector[0]+ Para_Xray.fc[1]*Spc_Xray_Detector[1])/2;
	Spc_Out[0] = Spc_Xray_Detector[0];
	Spc_Out[1] = Spc_Xray_Detector[1];
		 
	KK[0][0]= Para_Xray.fc[0];
	KK[0][1]= Para_Xray.alpha_c*Para_Xray.fc[0];
	KK[0][2]= Para_Xray.cc[0];
	KK[1][1]= Para_Xray.fc[1];
	KK[1][2]= Para_Xray.cc[1];
    KK[2][2]=1;
	
	float dimTemp;
	
	dimTemp=(BndBx_Out[1]-BndBx_Out[0])/Spc_Out[0];	 
	Dim_Out_xray[0]=floor(dimTemp);
	Dim_Out_xray[1]=floor((BndBx_Out[3]-BndBx_Out[2])/Spc_Out[1]);    
	numPix_out_xray = Dim_Out_xray[0]*Dim_Out_xray[1];
	

	//The output image pixel coordinates on the imaging plane

	float **cord = (float **)malloc(sizeof(float*) * numPix_out_xray); 
	for(i=0;i<numPix_out_xray;i++) cord[i] = (float*)malloc(sizeof(float)*2) ;    
	
	for(i=0;i<Dim_Out_xray[1]; i++)
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
  		tempPara0 = 1+Para_Xray.kc[0]*r2[i] + Para_Xray.kc[1]*r2[i]*r2[i] +Para_Xray.kc[4]*r2[i]*r2[i]*r2[i];		//0.998 ;46862.875
  		tempPara1 = 2 * Para_Xray.kc[2]*xy[i] + Para_Xray.kc[3]*(r2[i]+2 *cord2[i][0]);   //-0.0000662
  		tempPara2 = 2 * Para_Xray.kc[3]*xy[i]+Para_Xray.kc[2]* (r2[i]+2*cord2[i][1]);	  //0.0000184
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

	for(i=0;i<numPix_out_xray;i++) free(cord2[i]);
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

	x_sz = Dim_Cam_Detector[0] * Spc_Cam_Detector[0];
	y_sz = Dim_Cam_Detector[1] * Spc_Cam_Detector[1];
	BndBx_Out[0] = BndBx_Cam_Reg[0] * x_sz;
	BndBx_Out[1] = BndBx_Cam_Reg[1] * x_sz;
	BndBx_Out[2] = BndBx_Cam_Reg[2] * y_sz;
	BndBx_Out[3] = BndBx_Cam_Reg[3] * y_sz;
	fl = (Para_Cam.fc[0]*Spc_Cam_Detector[0]+ Para_Cam.fc[1]*Spc_Cam_Detector[1])/2;
	Spc_Out[0] = Spc_Cam_Reg[0];
	Spc_Out[1] = Spc_Cam_Reg[1];
		 
	KK[0][0]= Para_Cam.fc[0];
	KK[0][1]= Para_Cam.alpha_c*Para_Cam.fc[0];
	KK[0][2]= Para_Cam.cc[0];
	KK[1][1]= Para_Cam.fc[1];
	KK[1][2]= Para_Cam.cc[1];
    KK[2][2]= 1;
	
	float dimTemp;

	dimTemp=(BndBx_Out[1]-BndBx_Out[0])/Spc_Out[0];
	Dim_Out_cam[0]=floor(dimTemp); 
	Dim_Out_cam[1]=floor((BndBx_Out[3]-BndBx_Out[2])/Spc_Out[1]);  
	Dim_Cam[0]= Dim_Out_cam[0];
	Dim_Cam[1]= Dim_Out_cam[1];
	

	numPix_out_cam = Dim_Out_cam[0]*Dim_Out_cam[1];
	Mask_Cam.x_left=0;
	Mask_Cam.x_right=Dim_Cam[0]-1;
	Mask_Cam.b_ChamberEdge=0;
	Mask_Cam.b_CoverEdge=Dim_Cam[1]-1;

	//The output image pixel coordinates on the imaging plane 
	float **cord = (float **)malloc(sizeof(float*) * numPix_out_cam); 
	for(i=0;i<numPix_out_cam;i++) cord[i] = (float*)malloc(sizeof(float) *2) ; 
	
	for(i=0;i<Dim_Out_cam[1]; i++)
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
  		tempPara0 = 1+Para_Xray.kc[0]*r2[i] + Para_Xray.kc[1]*r2[i]*r2[i] +Para_Xray.kc[4]*r2[i]*r2[i]*r2[i];		//0.998 ;46862.875
  		tempPara1 = 2 * Para_Xray.kc[2]*xy[i] + Para_Xray.kc[3]*(r2[i]+2 *cord2[i][0]);   //-0.0000662
  		tempPara2 = 2 * Para_Xray.kc[3]*xy[i]+Para_Xray.kc[2]* (r2[i]+2*cord2[i][1]);	  //0.0000184
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
	
	//transfer coorinates to cord_cam
	cord_cam = cord;

	for(i=0;i<numPix_out_cam;i++) free(cord2[i]);
	free(cord2);
	free(xy);
	free(r2);
	
}

//pinhole correction for x_ray image
float * pinhole_Xray (unsigned short* imgBuf, float *CastRange)	
{ 
	int i,j,k;  
	
	//Calculate the pinhole parameters
	pinhole_Xray_Para_Initial();
	
	unsigned int n_pix = Dim_Xray_Detector[0]*Dim_Xray_Detector[1]; 
	
	//median filtering of the input image
	int x_neib[] = {0, -1, 1, -1,  0,  1, -1,  0,  1};
	int y_neib[] = {0,  0, 0, -1, -1, -1,  1,  1,  1};
	double val_Neib[9], median[1]; 																															
	unsigned int l=Dim_Xray_Detector[0];
	float *Img_tmp0 = malloc(sizeof(float)*n_pix);
	float *pp=Img_tmp0;
	
	for (j=0;j<Dim_Xray_Detector[1];j++)
	for (i=0;i<Dim_Xray_Detector[0];i++,pp++)
	{
	  if (i==0 || i==(Dim_Xray_Detector[0]-1) || j==0 || j==(Dim_Xray_Detector[1]-1)) {
		  *pp = imgBuf[i+j*l];
	  } else {
		for (k=0;k<9;k++) val_Neib[k] = (double) imgBuf[i+x_neib[k]+(j+y_neib[k])*l];
		int error = Median(val_Neib, 9, median);
		*pp = (float) median[0];
	  }
		
	} 
	
	
	//image enhancement
	float minImg = Img_tmp0[0]; 
	float *pp1;
	for (i=0,pp1=Img_tmp0; i<n_pix; i++,pp1++) if(*pp1<minImg) minImg=*pp1;
	for (i=0,pp1=Img_tmp0; i<n_pix; i++,pp1++) *pp1=(float)(-log((double)(*pp1-minImg+1e-8))); 
	
	
	//interpolation  
	n_pix = Dim_Out_xray[0]*Dim_Out_xray[1];
	float* Img_tmp1 = (float*)malloc(sizeof(float)*n_pix);
	for (i=0; i<n_pix; i++)
	Img_tmp1[i]= Interpolation_Float(Img_tmp0, Dim_Xray_Detector[0], Dim_Xray_Detector[1], cord_xray[i][0], cord_xray[i][1]);
	
	
	
	/*//reverse the image value
	float maxImg = Img_tmp1[0];
	for (i=0,pp1=Img_tmp1; i<n_pix; i++,pp1++) if(*pp1>maxImg) maxImg=*pp1;
	for (i=0,pp1=Img_tmp1; i<n_pix; i++,pp1++) *pp1=maxImg-*pp1;  */
	
	
	//Do rebining
	unsigned int Dim0[] = {Dim_Out_xray[0], Dim_Out_xray[1]};
	Dim_Out_xray[0] = round( (float)Dim0[0]/(float)Rebin_Xray_Reg );
    Dim_Out_xray[1] = round( (float)Dim0[1]/(float)Rebin_Xray_Reg );
	n_pix = Dim_Out_xray[0]*Dim_Out_xray[1];
	float* Img_Xray = (float*)malloc(sizeof(float)*n_pix);
	rebin2D_nondivisible(Img_tmp1, Dim0, Img_Xray, Dim_Out_xray); 
	
	
	// cast x-ray pixel value into 0~255
	float tmp, low, high;
	float max_v=Img_Xray[0], min_v=Img_Xray[0];
	for (i=0,pp1=Img_Xray; i<n_pix; i++,pp1++) 
	{
		if (*pp1>max_v) max_v=*pp1; 
		if (*pp1<min_v) min_v=*pp1;
	}
	low  = min_v + CastRange[0]*(max_v-min_v);
	high = min_v + CastRange[1]*(max_v-min_v);
	for (i=0,pp1=Img_Xray; i<n_pix; i++,pp1++)
	{ 
		tmp = 255.0* (*pp1-low)/(high-low);
		if (tmp<0) tmp=0;
		if (tmp>255) tmp=255;
		*pp1 = tmp;
	}  
	
	
	//set global xray dimension
	Dim_Xray[0]=Dim_Out_xray[0];
	Dim_Xray[1]=Dim_Out_xray[1];
	
	//free the intermediate variables
	for(i=0;i<numPix_out_xray;i++) free(cord_xray[i]);
	free(cord_xray);
	free(Img_tmp0);
	free(Img_tmp1); 
	
	return Img_Xray;
	  
}


		//pinhole correction for side photo
float * pinhole_SidePhoto ( unsigned char* imgBuf )		 
{
	int i,j;
	
    //Calculate the pinhole prameters
	pinhole_Cam_Para_Initial(); 
	
	unsigned int n_pix = Dim_Out_cam[0]*Dim_Out_cam[1];
	float* Img_tmp = malloc(sizeof(float)*n_pix); 
	float* Img_Cam = malloc(sizeof(float)*n_pix);
	
	//interpolate  
	unsigned char temp;
	for (i=0; i<numPix_out_cam; i++)
	 Img_tmp[i]= (float) Interpolation_Char(imgBuf, Dim_Cam_Detector[0], Dim_Cam_Detector[1], cord_cam[i][0], cord_cam[i][1]); 

	
	// reverse the image
	for (i=0; i<Dim_Out_cam[0]; i++)
  	for (j=0; j<Dim_Out_cam[1]; j++)
	Img_Cam[j*(int)Dim_Out_cam[0]+i] = Img_tmp[((int)Dim_Out_cam[1]-j-1)*(int)Dim_Out_cam[0]+i];
	
	
	//free the intermediate variables
	for(i=0;i<numPix_out_cam;i++) free(cord_cam[i]);
	free(cord_cam);
	free(Img_tmp);
	
	return Img_Cam;
	
}

// interpolation for unsigned short image type (x-ray image)
unsigned short Interpolation_Short ( unsigned short* imgBuf, unsigned int lWidth, unsigned int lHeight, float x, float y)
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
		} 
	}
}


// interpolation for float image type (x-ray image)
float Interpolation_Float ( float* imgBuf, unsigned int lWidth, unsigned int lHeight, float x, float y)
{
		// four adjacent coordinates
		int i1, i2;
		int j1, j2;
	
		// four adjacent pixel
		float f1,f2,f3,f4;
	
		// two interpolated pixel
		float f12, f34;
	
		// two coords are supposed to be the same if their difference is less then EXP
		float EXP = 0.0001;
	
		//calculate four adjacent coods
		i1 = (int)x;
		i2 = i1+1;
		j1 = (int)y;
		j2 = j1+1;
	
		// if the point is out of the image, return 0
		if ((x<0)||(x > lWidth-1)||(y<0)||(y>lHeight-1))
		{ 
			return 0;
		}								
		else
		{
			if (fabs(x-lWidth+1)<=EXP) 
			{
				// if the point is at the right edge of the image
				if (fabs(y-lHeight+1)<=EXP)
					{
						//the point is at the right down corner of the image, return the original value
						f1 = *((float *)imgBuf+lWidth * j1 + i1);
						return f1;
					}
				else
					{
						f1 = *((float *)imgBuf + lWidth * j1+i1);
						f3 = *((float *)imgBuf + lWidth * j1 + i2);
						return ((float)(f1+(y-j1)*(f3-f1)));
					}
			}
	
		else if (fabs(y-lHeight +1)<=EXP)
 		{  
		//the point is at the lower edge of the image, but not the last point
	 		f1 = *((float *)imgBuf + lWidth * j1 + i1);
	 		f2 = *((float *)imgBuf + lWidth * j2 + i1);		 //*** char to short
	 		return ((float) (f1+(x-i1)*(f2-f1)));
 		}
 		else
 		{
		 f1 = imgBuf[lWidth * j1+i1];
	     f2 = imgBuf[lWidth * j2+i1];
		 f3 = imgBuf[lWidth * j1+i2];
		 f4 = imgBuf[lWidth * j2+i2];
		 
		 f12=(float)(f1+(x-i1)*(f2-f1));	  
		 f34=(float)(f3+(x-i1)*(f4-f3));
		 
		 return ((float)(f12+(y-j1)*(f34-f12)));
		} 
	}
}

//interpolcation for char image type (side photo)
unsigned char Interpolation_Char ( unsigned char* imgBuf, unsigned int lWidth, unsigned int lHeight, float x, float y)
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
			}
		}

}


//rebin 2D image pixels, when the input and output dimensions are not nessesarily divisible, use less than 1 pixel area for the rebining
//INPUTS:  Img0, Dim0[3]: input image and dimension
//         Dim1: output image dimension
//OUTPUT:  Img1: output image

void rebin2D_nondivisible(float *Img0, unsigned int Dim0[2], float *Img1, unsigned int Dim1[2])
{
	
	unsigned int i,j, ii, jj, i0, i1, j0, j1;
	float x0, x1, y0, y1, rx, ry, r;
	
	
	//spacing for rebining
	float Spc[2], s_pix; 
	Spc[0] = (float)Dim0[0]/(float)Dim1[0];
	Spc[1] = (float)Dim0[1]/(float)Dim1[1];  
	s_pix = Spc[0]*Spc[1];
	
	//initialize the output image
	unsigned int n_pix1 = Dim1[0]*Dim1[1];
	float *pp=Img1;
	for (i=0;i<n_pix1;i++,pp++) *pp=0;
	
	pp=Img1;
	for (j=0;j<Dim1[1];j++)
	for (i=0;i<Dim1[0];i++,pp++)
	{																					 
		//the x and y bounds for the current regioin
		x0=i*Spc[0];    x1=(i+1)*Spc[0];  
		y0=j*Spc[1];    y1=(j+1)*Spc[1];  
		if(x1>=Dim0[0]) x1=((float)Dim0[0])-0.01; 
		if(y1>=Dim0[1]) y1=((float)Dim0[1])-0.01;
		i0=floor(x0);   i1=floor(x1); 
		j0=floor(y0);   j1=floor(y1);
		
		//Do rebining
		for (jj=j0; jj<=j1; jj++)
		for (ii=i0; ii<=i1; ii++)
		{
			if (ii<x0)   rx = ii+1-x0;
			else if (ii==i1)  rx = x1-ii;
			else rx=1;
			
			if (jj<y0)   ry = jj+1-y0;
			else if (jj==j1)  ry = y1-jj;
			else ry=1;
			
			r = rx*ry;
			*pp += r*Img0[ii+jj*Dim0[0]];
		}
		*pp = *pp/s_pix;
		
	}
	
}
		
