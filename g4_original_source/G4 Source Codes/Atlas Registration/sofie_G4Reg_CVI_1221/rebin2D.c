#include <ansi_c.h>
void rebin2D(float *Img, unsigned char Dim[2], float* Img_Out, unsigned char Dim_Out[2]) ;

/*
Img: input image buffer
Dim[2]:dimension of input image
Img_Out: output image buffer
Dim_Out[2]:dimension of output image
*/

void rebin2D(float *Img, unsigned char Dim[2], float* Img_Out, unsigned char Dim_Out[2])
{
	//down sample rate
	int n[2],Num;
	float temp;
	temp=(float)Dim[0]/(float)Dim_Out[0];
	if(temp>floor(temp))
		n[0]=Dim[0]/(Dim_Out[0]-1);
	else
		n[0]=temp;
	
	temp=Dim[1]/Dim_Out[1];
	if(temp>floor(temp))
		n[1]=floor(Dim[1]/(Dim_Out[1]-1)); 
	else
		n[1]=temp;
	
	//extra 
	int ex,ey;
	ex=n[0]-(n[0]*Dim_Out[0]-Dim[0]);
	ey=n[1]-(n[1]*Dim_Out[1]-Dim[1]);
	
	int i,j,k0,k1;
	float Sum=0;
	for(j=0;j<Dim_Out[1];j++)
		for(i=0;i<Dim_Out[0];i++)
		{
			//the point is at the rightest side, but not at the bottom
			if(i==(Dim_Out[0]-1)&&j!=(Dim_Out[1]-1))
			{
				for(k0=0;k0<ex;k0++)
					for(k1=0;k1<n[1];k1++)
						Sum=Sum+Img[i*n[0]+k0+(j*n[1]+k1)*Dim[0]];
				Img_Out[i+j*Dim_Out[0]]=Sum/(ex*n[1]);
			}
				
			//the point is at the bottom, but not at the rightest side
			else if(i!=(Dim_Out[0]-1)&&j!=(Dim_Out[1]-1))
			{
				for(k0=0;k0<n[0];k0++)
					for(k1=0;k1<ey;k1++)
						Sum=Sum+Img[i*n[0]+k0+(j*n[1]+k1)*Dim[0]];
				Img_Out[i+j*Dim_Out[0]]=Sum/(n[0]*ey);
			}
			
			//the point is at the bottom and the rightest	
			else if(i==(Dim_Out[0]-1)&&j==(Dim_Out[1]-1))
			{
				for(k0=0;k0<ex;k0++)
					for(k1=0;k1<ey;k1++)
						Sum=Sum+Img[i*n[0]+k0+(j*n[1]+k1)*Dim[0]];
				Img_Out[i+j*Dim_Out[0]]=Sum/(ex*ey);	
			}
			
			//the point is not at the bottom nor rightest	
			else
			{
				for(k0=0;k0<n[0];k0++)
					for(k1=0;k1<n[1];k1++)
						Sum=Sum+Img[i*n[0]+k0+(j*n[1]+k1)*Dim[0]];
				Img_Out[i+j*Dim_Out[0]]=Sum/(n[0]*n[1]);
			} }
	
	
}
		
