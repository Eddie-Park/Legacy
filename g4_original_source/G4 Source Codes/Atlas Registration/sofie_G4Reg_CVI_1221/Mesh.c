		
#include "windows.h"
#include <tcpsupp.h>
#include <mmsystem.h>  
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>

#include "Headers.h"
#include "atlasRegistration.h" 
#include "GlobalDecls.h" 
#include "AtlasRegistrationMain.h"


/*
	Function: "Extract_SubMesh"
	Suppose a father mesh is composed of several sub-meshes, with each sub-mesh has its own label
	this method extract several sub-meshes to form a new mesh
	
INPUT: 
	Verts:    		the vertices of the father mesh
    Tri:           the Triection of the father mesh
    Labs_Mesh:      the labels of each sub-mesh in the father mesh
    Labs_Verts:     the labels for each vertex in the father mesh
    numVerts_Each:  the number of verts of each sub mesh in the father mesh
    numTri_Each:   the number of Triection of each sub-mesh in the father mesh
    Labs_Extract:   the labels of the sub-meshes that are to be extracted
	
OUTPUT: 
	Verts_Out;
	Tri_Out;
	Labs_VertsOut;
	numVerts_Each_Out;
	numTri_Each_Out;
	Labs_Extract;
	numVerts;
*/

int Extract_SubMesh(struct SSM *Input_Mesh, unsigned int *Labs_Extract, unsigned int num_Extract, struct Mesh *Output_Mesh)
{
	
	unsigned int *id_Extract;
	unsigned int *id_Extract_temp;
	unsigned int i,j,ie,il;

	if (num_Extract==0)
	{
		Output_Mesh->Verts = (float*)malloc(sizeof(float)*1*3); 
    	Output_Mesh->Tri = (unsigned int*)malloc(sizeof(unsigned int)*1*3);
		for(i=0;i<3;i++)
		{
			Output_Mesh->Verts[i]=-1;
			Output_Mesh->Tri[i]=-1; 
		}   
		return -1;
	}
	
	id_Extract = (int*)malloc(sizeof(int)*num_Extract);
	for(ie=0;ie<num_Extract;ie++)
	{
		for(i=0;i<Input_Mesh->num_Mesh;i++) 
		if (Labs_Extract[ie]==Input_Mesh->Labs_Mesh[i]) id_Extract[ie] = i;
	}
	
    Output_Mesh->numVerts_Each = (unsigned int*)malloc(sizeof(unsigned int)*num_Extract);
	Output_Mesh->numTri_Each = (unsigned int*)malloc(sizeof(unsigned int)*num_Extract);
	Output_Mesh->Labs_Mesh = (unsigned int*)malloc(sizeof(unsigned int)*num_Extract); 
	Output_Mesh->num_Mesh = num_Extract;
	
	Output_Mesh->num_Verts = 0;
	Output_Mesh->num_Tri = 0;
	for(ie=0;ie<num_Extract;ie++)
	{
    Output_Mesh->numVerts_Each[ie] = *(Input_Mesh->numVerts_Each + id_Extract[ie]); 
	Output_Mesh->numTri_Each[ie] = *(Input_Mesh->numTri_Each + id_Extract[ie]);
    Output_Mesh->num_Verts = Output_Mesh->num_Verts + Output_Mesh->numVerts_Each[ie];
	Output_Mesh->num_Tri = Output_Mesh->num_Tri + Output_Mesh->numTri_Each[ie];
	Output_Mesh->Labs_Mesh[ie] = *(Input_Mesh->Labs_Mesh + id_Extract[ie]); 
	}
	
    Output_Mesh->Verts = (float*)malloc(sizeof(float)*Output_Mesh->num_Verts*3); 
    Output_Mesh->Tri = (unsigned int *)malloc(sizeof(unsigned int)*Output_Mesh->num_Tri*3);
	Output_Mesh->Labs_Verts = (unsigned int*)malloc(sizeof(unsigned int)*Output_Mesh->num_Verts);
	
	int iv_start1 =0;
	int iv_start0 =0;
	int ic_start1 =0;
	int ic_start0 =0;
		
    for(ie = 0; ie<num_Extract;ie++)
	{
        //extract vert and Tri
		if(ie==0)
		{
			iv_start1=0;
			ic_start1=0;
		}
		else
		{
			for (i=0;i<ie;i++)
			{
				iv_start1= iv_start1 + Output_Mesh->numVerts_Each[i];
				ic_start1= ic_start1 + Output_Mesh->numTri_Each[i];
			}
		}
		
		if(id_Extract[ie]==0)
		{
			iv_start0 = 0;
			ic_start0 = 0;
		}
		else
		{
			for(i=0;i<id_Extract[ie];i++)
			{
				iv_start0 = iv_start0+ Input_Mesh->numVerts_Each[i]; 
				ic_start0 = ic_start0+ Input_Mesh->numTri_Each[i];
			}
		}
		
		for(i=0;i<Output_Mesh->numVerts_Each[ie];i++)
		{
			for(j=0;j<3;j++)
				Output_Mesh->Verts[(iv_start1+i)*3+j] = Input_Mesh->Verts[(iv_start0+i)*3+j];
				
			Output_Mesh->Labs_Verts[iv_start1+i] = Input_Mesh->Labs_Verts[iv_start0+i]; 
		}
		
			
        for(i=0;i<Output_Mesh->numTri_Each[ie];i++)
			for(j=0;j<3;j++)
        		Output_Mesh->Tri[(ic_start1+i)*3+j] = Input_Mesh->Tri[(ic_start0+i)*3+j] -iv_start0+iv_start1;
        
    } 
	
	free(id_Extract);  
	return 0;
	
	
		
}
   

void Combine_MultiMesh(struct SSM *Mesh0, struct SSM *Mesh1, struct Mesh *Mesh_All)
{
  int i;
  
  Mesh_All->Verts = malloc(sizeof(float)*(Mesh0->num_Verts+Mesh1->num_Verts)*3);
  float *iv=Mesh_All->Verts, *iv1=Mesh0->Verts;
  for (i=0;i<3*Mesh0->num_Verts;i++) {*iv=*iv1; iv++; iv1++;}
  iv1=Mesh1->Verts;
  for (i=0;i<3*Mesh1->num_Verts;i++) {*iv=*iv1; iv++; iv1++;}
  
  Mesh_All->Tri = malloc(sizeof(float)*(Mesh0->num_Tri+Mesh1->num_Tri)*3);
  unsigned int *ic=Mesh_All->Tri, *ic1=Mesh0->Tri;
  for (i=0;i<3*Mesh0->num_Tri;i++) {*ic=*ic1; ic++; ic1++;}	
  ic1=Mesh1->Tri;
  for (i=0;i<3*Mesh1->num_Tri;i++) {*ic=*ic1+Mesh0->num_Verts; ic++; ic1++;}
  
  Mesh_All->Labs_Verts = malloc(sizeof(float)*(Mesh0->num_Verts+Mesh1->num_Verts));
  ic=Mesh_All->Labs_Verts, ic1=Mesh0->Labs_Verts;
  for (i=0;i<Mesh0->num_Verts;i++) {*ic=*ic1; ic++; ic1++;}
  ic1=Mesh1->Labs_Verts;
  for (i=0;i<Mesh1->num_Verts;i++) {*ic=*ic1; ic++; ic1++;}
  
  Mesh_All->Labs_Mesh = malloc(sizeof(float)*(Mesh0->num_Mesh+Mesh1->num_Mesh));
  for (i=0;i<Mesh0->num_Mesh;i++) Mesh_All->Labs_Mesh[i]=Mesh0->Labs_Mesh[i];
  for (i=0;i<Mesh1->num_Mesh;i++) Mesh_All->Labs_Mesh[i+Mesh0->num_Mesh]=Mesh1->Labs_Mesh[i];  
  
  Mesh_All->numVerts_Each = malloc(sizeof(float)*(Mesh0->num_Mesh+Mesh1->num_Mesh));
  for (i=0;i<Mesh0->num_Mesh;i++) Mesh_All->numVerts_Each[i]=Mesh0->numVerts_Each[i];
  for (i=0;i<Mesh1->num_Mesh;i++) Mesh_All->numVerts_Each[i+Mesh0->num_Mesh]=Mesh1->numVerts_Each[i];  
  
  Mesh_All->numTri_Each = malloc(sizeof(float)*(Mesh0->num_Mesh+Mesh1->num_Mesh));
  for (i=0;i<Mesh0->num_Mesh;i++) Mesh_All->numTri_Each[i]=Mesh0->numTri_Each[i];
  for (i=0;i<Mesh1->num_Mesh;i++) Mesh_All->numTri_Each[i+Mesh0->num_Mesh]=Mesh1->numTri_Each[i];  
  
  Mesh_All->num_Tri = Mesh0->num_Tri + Mesh1->num_Tri;
  Mesh_All->num_Verts = Mesh0->num_Verts + Mesh1->num_Verts;
  Mesh_All->num_Mesh = Mesh0->num_Mesh + Mesh1->num_Mesh; 
  
}


void free_Mesh(struct Mesh* Mesh)
{
	free(Mesh->Verts);
	free(Mesh->Tri);
	free(Mesh->Labs_Mesh);
	free(Mesh->Labs_Verts);
	free(Mesh->numVerts_Each);
	free(Mesh->numTri_Each);
}


void free_SSM(struct SSM* SSM)
{
	free(SSM->Verts);
	free(SSM->Tri);
	free(SSM->Labs_Mesh);
	free(SSM->Labs_Verts);
	free(SSM->numVerts_Each);
	free(SSM->numTri_Each);
	free(SSM->Verts_Mean);
	free(SSM->Eigenval);
	free(SSM->Eigenvect);
	free(SSM->Inv_COV);
}


void free_CSM(struct CSM* CSM)
{
	free(CSM->MeanCoef_Outer); 
	free(CSM->MeanCoef_Inner);
	free(CSM->invCov_II); 
    free(CSM->CovIO_invCovO);
	free(CSM->ind_Skin);
	free(CSM->ind_Skel);
}


/*
 FUNCTION:  Fill_Mesh
			Fill a closed trianglular mesh, the user should guarantee that
            the mesh input is spatially closed
 INPUT:     Dim: the size of the volume to be filled
            Mesh: the mesh to be filled
			step: the filling step
 OUTPUT:    Volume: The filled bw volume with the size of Dim   
*/
void Fill_Mesh(struct Mesh *Mesh, unsigned int Label, unsigned int Dim[3], float step, unsigned char *Volume)
{
	float grow_ratio;
	float total_Vlm_Sub=0;
	float total_Volume=0;
	unsigned int i,j,k; 
	int x_min,x_max,y_min,y_max,z_min,z_max;
	
	unsigned int num_vox = Dim[0]*Dim[1]*Dim[2];
	unsigned int num_vox_slc = Dim[0]*Dim[1]; 
	unsigned char *pp;
	
	
	//Get the bounding box of the mesh
	x_min=Mesh->Verts[0];
	x_max=Mesh->Verts[0];
	y_min=Mesh->Verts[1];
	y_max=Mesh->Verts[1];
	z_min=Mesh->Verts[2];
	z_max=Mesh->Verts[2];
	
	for(i=0;i<Mesh->num_Verts;i++)
	{
		if (Mesh->Verts[i*3]<x_min)   x_min=Mesh->Verts[i*3];
		if (Mesh->Verts[i*3]>x_max)   x_max=Mesh->Verts[i*3];
		if (Mesh->Verts[i*3+1]<y_min) y_min=Mesh->Verts[i*3+1];
		if (Mesh->Verts[i*3+1]>y_max) y_max=Mesh->Verts[i*3+1];
		if (Mesh->Verts[i*3+2]<z_min) z_min=Mesh->Verts[i*3+2];
		if (Mesh->Verts[i*3+2]>z_max) z_max=Mesh->Verts[i*3+2];
	}
	
	x_min = x_min-2;
	x_max = x_max+2;
	y_min = y_min-2;
	y_max = y_max+2;
	z_min = z_min-2;
	z_max = z_max+2;
	
	if(x_min<0) x_min = 0;
	if(x_max>(Dim[0]-1)) x_max = Dim[0]-1;
	if(y_min<0) y_min = 0;
	if(y_max>(Dim[1]-1)) y_max=Dim[1]-1;
	if(z_min<0) z_min = 0;
	if(z_max>(Dim[2]-1)) z_max = Dim[2]-1;

    if((x_min>=x_max)||(y_min>=y_max)||(z_min>=z_max)) return;

	
	//fill the mesh surfacce only
	step=0.2;
	unsigned char *Vlm_Surf = calloc(num_vox, sizeof(unsigned char));
	Fill_Mesh_Only(Dim, Mesh->Verts, Mesh->Tri, Mesh->num_Tri, step, Vlm_Surf);
	
    //Get the subvolume
	unsigned int Dim_Sub[3] = {x_max-x_min+1, y_max-y_min+1, z_max-z_min+1}; 
	unsigned int num_vox1 = Dim_Sub[0]*Dim_Sub[1]*Dim_Sub[2];
	unsigned char *Vlm_Sub = calloc(num_vox1, sizeof(unsigned char));
	
	unsigned char max_Vlm_Sub=0, min_Vlm_Sub=1;
	pp=Vlm_Sub;
	for(k=0;k<Dim_Sub[2];k++)
	for(j=0;j<Dim_Sub[1];j++)
	for(i=0;i<Dim_Sub[0];i++,pp++)
	{
	  *pp = Vlm_Surf[i+x_min+(j+y_min)*Dim[0]+(k+z_min)*num_vox_slc];
	  if (*pp>max_Vlm_Sub) max_Vlm_Sub=*pp;
	  if (*pp<min_Vlm_Sub) min_Vlm_Sub=*pp;
	}
	
	
	if(max_Vlm_Sub==0 || min_Vlm_Sub==1) 
	{
		free(Vlm_Sub); 
	    free(Vlm_Surf);
		return;
	}

	
	//close possible holes 
	close_3x3cube(Vlm_Sub, Dim_Sub);
	
	//Do region griowing based on the filled surface
	unsigned int seed[3]={0};
	unsigned char *BW_RG = calloc(num_vox1, sizeof(unsigned char));
	Region_Growing_3D(Vlm_Sub, BW_RG, Dim_Sub, seed, 0.0, 0.5, 6);       
	unsigned char *pp1=BW_RG;
    pp=Vlm_Sub; for(i=0;i<num_vox1;i++,pp++,pp1++) if(*pp==0 && *pp1==0) *pp=1; 
	free(BW_RG);
	
	
	/*unsigned long nBytesWritten;  
	HANDLE vFile = CreateFile("Filled_CT.raw",GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL); 
	if (vFile != INVALID_HANDLE_VALUE)
	{
	   WriteFile(vFile, Vlm_Sub, sizeof(unsigned char)*num_vox1, &nBytesWritten, NULL); 
	   CloseHandle(vFile); 
	}*/
	
	//Check if the volume is filled correctly, if not, reduce the filling step and fill again
	pp=Vlm_Sub; for(i=0;i<num_vox1;i++,pp++) if(*pp==1) total_Vlm_Sub++;
    pp=Vlm_Surf;  for(i=0;i<num_vox; i++,pp++) if(*pp==1) total_Volume++;
	grow_ratio = (total_Vlm_Sub-total_Volume)/total_Volume;
	 
	if(grow_ratio>0.1)
	{
	  pp=Vlm_Sub;
	  for(k=0;k<Dim_Sub[2];k++)    
	  for(j=0;j<Dim_Sub[1];j++)
	  for(i=0;i<Dim_Sub[0];i++,pp++)
		 if (*pp==1) Volume[i+x_min+(j+y_min)*Dim[0]+(k+z_min)*num_vox_slc] = Label;
	 
	}
	else if (step>0.1)
	{																																						
	  step=step/2;
	  Fill_Mesh(Mesh, Label, Dim, step, Volume); 
	}
	 
	 
	free(Vlm_Sub); 
	free(Vlm_Surf);
     
}


void Fill_Mesh_Only(unsigned int Dim[3],  float *Verts,  unsigned int *Tri,  unsigned int num_Tri, float step,  unsigned char *VOLUME)
{
	
	float vt0[3], vt1[3], vt2[3], vte[3];
	float d12, d0e, n, nn, x, y, z;
	
	//fill the triangles one by one
	for (long i=0; i<num_Tri; i++ ) {

		for (int m=0; m<3; m++){
            vt0[m] = Verts[m+Tri[i*3]*3];
		    vt1[m] = Verts[m+Tri[1+i*3]*3];
		    vt2[m] = Verts[m+Tri[2+i*3]*3];
		}

		d12 = sqrt( (vt1[0]-vt2[0])*(vt1[0]-vt2[0])+(vt1[1]-vt2[1])*(vt1[1]-vt2[1])+(vt1[2]-vt2[2])*(vt1[2]-vt2[2]));
        n = ceil(d12 / step)+1;
        
		float *vtes = malloc(sizeof(float)*3*n); 
		for (int m=0; m<3; m++){
			for (int in=0; in<n; in++){
                vtes[m+in*3] = (float)in*(vt2[m]-vt1[m])/n+vt1[m];
			}
		}

		for (int m=0; m<n; m++) {

			for (int id=0; id<3; id++) {vte[id]=vtes[id+m*3];}
			d0e = sqrt( (vt0[0]-vte[0])*(vt0[0]-vte[0])+(vt0[1]-vte[1])*(vt0[1]-vte[1])+(vt0[2]-vte[2])*(vt0[2]-vte[2]));
            nn = ceil(d0e / step)+1;
            float *vtms = malloc(sizeof(float)*3*nn);
			for (int mm=0; mm<3; mm++){
			for (int in=0; in<nn; in++){
                vtms[mm+in*3] = (float)in*(vte[mm]-vt0[mm])/nn+vt0[mm];
			}}
            for (int mm=0; mm<nn; mm++){
				x = ((vtms[mm*3]-floor(vtms[mm*3]))>=0.5)?ceil(vtms[mm*3]):floor(vtms[mm*3]);
				y = ((vtms[1+mm*3]-floor(vtms[1+mm*3]))>=0.5)?ceil(vtms[1+mm*3]):floor(vtms[1+mm*3]);
				z = ((vtms[2+mm*3]-floor(vtms[2+mm*3]))>=0.5)?ceil(vtms[2+mm*3]):floor(vtms[2+mm*3]);
				if ( (x>=0) && (x<Dim[0]) && (y>=0) && (y<Dim[1]) && (z>=0) && (z<Dim[2])) {
					VOLUME[(int)x+(int)y*Dim[0]+(int)z*Dim[0]*Dim[1]]=1; }
			}			
		    free(vtms); //be careful, to avoid memory leakage, every dynamically created variable should be deleted before it is created again in another loop

		}
		free(vtes);

	}
	
}



/*
Fill multiple meshes into labeled solid volume, the multiple mesh is a father mesh which is 
composed of several sub-meshes
INPUTS: Verts:         The vertices of the multiple mesh, 
                       the coodinates of the vertices should represented by the volume image data coodinate system
        Tri:          The Triectivity of the multiple mesh
        Dim_Vlm:       The dimension of the output volume image //3 vector
        Labs_Fill:     The sequence of labels for filling, in case the sub-meshes has overlapping, 
                       the meshes filled latter will overwrite the overlapping parts of the meshes filled earlier
        Labs_Mesh:     The object labels of the each sub-mesh 
        numVerts_Each: The number of vertices of each sub-mesh
        numTri_Each:  The number of Triectivities of each sub-mesh
OUTPUT: the Label volume // size of dim_vlm 
*/

unsigned char * Fill_MultiMesh(struct Mesh *Mesh, unsigned int *Labs_Fill, unsigned int Dim_Vlm[3], unsigned int num_Fill)
{
	
	struct Mesh *out_buffer=&buffer;
	unsigned int id_Fill;
    int iv_start;
	int ic_start;
	int i,j,k; 
	int io,ii,il;
	
	unsigned int num_vox = Dim_Vlm[0]*Dim_Vlm[1]*Dim_Vlm[2];
	unsigned char *Vlm_Lab = calloc(num_vox, sizeof(unsigned char)); 
	
	
    /*make sure the skin is filled first */ 
	unsigned int *Labs_Fill1 = sort(Labs_Fill,num_Fill);
	
	
    for (io=0;io<num_Fill;io++)
	{
		id_Fill=-1;
		for (il=0;il<Mesh->num_Mesh;il++) if(Mesh->Labs_Mesh[il]==Labs_Fill1[io])  id_Fill=il;
		 
		if(id_Fill!=-1)
		{
			iv_start=0;
			ic_start=0;
			for(i=0;i<id_Fill;i++)
			{
			  iv_start=iv_start + Mesh->numVerts_Each[i];
			  ic_start=ic_start + Mesh->numTri_Each[i];
			}
			
			out_buffer->Verts = malloc(sizeof(float)*Mesh->numVerts_Each[id_Fill]*3);
			out_buffer->Tri = malloc(sizeof(unsigned int)*Mesh->numTri_Each[id_Fill]*3);
			out_buffer->num_Verts = Mesh->numVerts_Each[id_Fill]; 
			out_buffer->num_Tri = Mesh->numTri_Each[id_Fill]; 
			
			for(i=0;i<Mesh->numVerts_Each[id_Fill];i++) for(j=0;j<3;j++)
			out_buffer->Verts[i*3+j]= Mesh->Verts[(iv_start+i)*3+j];
			
			for(i=0;i<Mesh->numTri_Each[id_Fill];i++) for(j=0;j<3;j++)
			out_buffer->Tri[i*3+j]= Mesh->Tri[(ic_start+i)*3+j]-iv_start;
			
			Fill_Mesh(out_buffer, Labs_Fill1[io], Dim_Vlm, 0.4, Vlm_Lab);
			
			free_Mesh(out_buffer);
			
		}
	}
	
	free(Labs_Fill1);
	
	
	return Vlm_Lab;   // if Vlm_Lab=0, error
}


unsigned int *sort(unsigned int*t, unsigned int num)
{
	unsigned int i,j;
	unsigned int temp;
	unsigned int *p = calloc(num, sizeof(unsigned int));
	
	for (i=0;i<num;i++) p[i]=t[i]; 
	
	for(i=0;i<(num-1);i++)
		for(j=i+1;j<num;j++)
		{
			if(*(p+i)>*(p+j))
			{
				temp=*(p+i);
				*(p+i)=*(p+j);
				*(p+j)=temp;
			}
		}
	return p;
}


unsigned int round(float a)
{
	unsigned int b, tmp;
	tmp = floor(a);
	b = ( (a-tmp)<0.5 )?tmp:(tmp+1);
	
	return b;
}

/*
ISODATA thresholding to classify the image into several classes

INPUT:  Img: the image to be thresholded, in arbitary dimention
		Dim: dimention of the input image
        n:   number of classes to be classified, minimumly 2
OUTPUT: Img_Lab: the label image    //char
        Th: the decided thresholds.(initialized as all 0 vector)  //float   n-1 vector
*/
void ISODATA_Thresholding(float *Img_In, int Dim[2],int n,float *Img_Lab,float *Th)
{
  
	int It_Max = 500;
	int i,ic,it,j;
	float Up,Low;
	float interval;
	float Max_abs_Th;
	int Sum,Num;
	
	Up=Img_In[0];
	Low=Img_In[0];
	for(i=0;i<Dim[0]*Dim[1];i++)
	{
		if(Img_In[i]<Low)
			Low=Img_In[i];
		if(Img_In[i]>Up)
			Up=Img_In[i];
	}
	
	//calculate the initial class means
   interval = (Up-Low)/n;
   float *mu = (float *)malloc(sizeof(float)*n);
   
   for (ic=0;ic<n;ic++)
	   mu[ic]=Low+(ic+0.5)*interval;
   
  //Iterations
   float *Th0= (float *)malloc(sizeof(float)*(n-1));
   for(i=0;i<n-1;i++)
	   Th0[i]=Low+(i+1)*interval;
	   
   for(it=0;it<It_Max;it++)
   {
	   if(it==0)
	   {   for(i=0;i<n-1;i++)
			   Th[i]=Th0[i];		
	   }
	   
	   Max_abs_Th=0;
	   for(i=0;i<n-1;i++)
	   {
		   if(Th[i]<=Th0[i])
		   {
			   if((Th0[i]-Th[i])/Th0[i]>Max_abs_Th)
				  Max_abs_Th=(Th0[i]-Th[i])/Th0[i];
		   }
		   else
		   {
			   if((Th[i]-Th0[i])/Th0[i]>Max_abs_Th)
				   Max_abs_Th=(Th[i]-Th0[i])/Th0[i];
		   }
	   }
	   
	   if(it>0 && Max_abs_Th<0.01)
		   break;
	   else
	   {
		   for(ic=0;ic<n;ic++)
		   {
			   if(ic==0)
			   {
				   Sum=0;
				   Num=0;
				   for(i=0;i<Dim[0]*Dim[1];i++)
				   {
					   if(Img_In[i]<Th[0])
					   {Sum=Sum+Img_In[i];
					   Num++;}
				   }
				   if (Num!=0) mu[ic]=Sum/Num;
			   }
			   else if(ic==n-1)
			   {
				   Sum=0;
				   Num=0;
				   for(i=0;i<Dim[0]*Dim[1];i++)
				   {
					   if(Img_In[i]>=Th[n-2])
					   {Sum=Sum+Img_In[i];
					   Num++;}
				   }
				   if (Num!=0) mu[ic]=Sum/Num;
			   }
			   else
			   {
				   Sum=0;
				   Num=0;
				   for(i=0;i<Dim[0]*Dim[1];i++)
				   {
					   if(Img_In[i]>=Th[ic-1] && Img_In[i]<Th[ic])
					   {Sum=Sum+Img_In[i];
					   Num++;}
				   }
				   if (Num!=0) mu[ic]=Sum/Num;
			   }
		   }
	   }
	   for(ic=1;ic<n;ic++)
	   {
		   for(i=0;i<n-1;i++) {Th0[i]=Th[i];}  
		   Th[ic-1]=0.5*(mu[ic-1]+mu[ic]);
		   
	   }
   }
						   
   
//Get label image
   for(ic=1;ic<n;ic++)
   {
	   if(ic==n-1)
	   {
		   for(i=0;i<Dim[0]*Dim[1];i++)
		   {
			   if(Img_In[i]>=Th[n-2])
				   Img_Lab[i]=n-1;
		   }
	   }
	   else
	   {
		    for(i=0;i<Dim[0]*Dim[1];i++)
		   {
			   if(Img_In[i]>=Th[ic-1] && Img_In[i]<Th[ic])
				   Img_Lab[i]=ic;
		   }
	   }
   }
   
   free(mu);
   free(Th0);
   
}


//nNeib: the number of neighborhood, either 6 or 26
void Region_Growing_3D(unsigned char *Vlm_In, unsigned char *Vlm_Out, unsigned int Dim[3],unsigned int seed[3], float Value_min, float Value_max, unsigned int nNeib)
{
	int i,j,k, X, Y, Z; 
	unsigned int s, l, v, id, ind;
	unsigned char Region_Label = 1;
	
	//the offset for searching neighborhood, 6-connection be the first 6th element, fllowed by the other 20 connections
	int xd[] = {-1, 1,  0, 0,  0, 0, -1,  0,  1, -1,  1, -1,  0,  1, -1,  1, -1, 1, -1,  0,  1, -1, 1, -1, 0, 1};
	int yd[] = { 0, 0, -1, 1,  0, 0, -1, -1, -1,  0,  0,  1,  1,  1, -1, -1,  1, 1, -1, -1, -1,  0, 0,  1, 1, 1};
	int zd[] = { 0, 0,  0, 0, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0, 0,  1,  1,  1,  1, 1,  1, 1, 1};
	
	l = Dim[0];
	s = Dim[0]*Dim[1];
	v = Dim[0]*Dim[1]*Dim[2];
	
	struct regionStack	  //the stack that stores the growed indices
	{
		unsigned short *x;
		unsigned short *y;
		unsigned short *z;
		int top;
	} buf;
	
	struct regionStack *buffer=&buf;
	buffer->x = (unsigned short*)calloc(v,sizeof(unsigned short));
	buffer->y = (unsigned short*)calloc(v,sizeof(unsigned short));    
	buffer->z = (unsigned short*)calloc(v,sizeof(unsigned short));    
	buffer->top = -1;
	
	*(Vlm_Out+seed[0]+seed[1]*l+seed[2]*s)=Region_Label;
	
	for(id=0;id<nNeib;id++)
	{
		i=seed[0]+xd[id];
		j=seed[1]+yd[id];
		k=seed[2]+zd[id];
		ind = i+j*l+k*s;
		
		if(!(i<0||j<0||k<0||i>=Dim[0]||j>=Dim[1]||k>=Dim[2]))
		{   
			if(Vlm_In[ind]>=Value_min && Vlm_In[ind]<=Value_max)
			{
				Vlm_Out[ind]=Region_Label;
				buffer->top++;
				buffer->x[buffer->top]=i;
				buffer->y[buffer->top]=j; 
				buffer->z[buffer->top]=k; 
				
			}
		}
	}
	
	while(buffer->top != -1)
	{
		X=buffer->x[buffer->top];
		Y=buffer->y[buffer->top];
		Z=buffer->z[buffer->top];
		buffer->top--;
		
		for(id=0;id<nNeib;id++)
		{
			i=X+xd[id];
		    j=Y+yd[id];
		    k=Z+zd[id];
			ind = i+j*l+k*s; 
			
			if(!(i<0||j<0||k<0||i>=Dim[0]||j>=Dim[1]||k>=Dim[2]) && !(i==X && j==Y && k==Z))
			{
				if (Vlm_Out[ind]!=Region_Label)
				{
				    if(Vlm_In[ind]>=Value_min && Vlm_In[ind]<=Value_max)
					{
						Vlm_Out[ind]=Region_Label;
						buffer->top++;
						buffer->x[buffer->top]=i;
						buffer->y[buffer->top]=j; 
						buffer->z[buffer->top]=k;
					}
				}
			}
		}
	}
 
	free(buffer->x);   
	free(buffer->y); 
	free(buffer->z); 
	
}
	


/*
  Function Interpolation
  Input: imgBuf: input image
  lWidth: dimension of x
  lHeight:dimention of y
  x,y: coordinate of the point to be interpolated
  Output: interpolated value of (x,y)
*/
float Interpolation ( float* imgBuf, int lWidth, int lHeight, float x, float y)
{
		// four adjacent coordinates
		int i1, i2;
		int j1, j2;
	
		// four adjacent pixel
		float f1,f2,f3,f4;
	
		// two interpolated pixel
		float f12, f34;
	
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
		} }}
			
   
/*
Resample 2D images
Img0, Spc0, Dim0: original data, 3 elements vector
Spc1: spacing of the output data
*/

float *Resample_Img(float *Image_In, float Spc0[2],unsigned int Dim0[2], float Spc1[2],unsigned int Dim1[2])
{
	int i,j;
	
	Dim1[0]=floor(Dim0[0]*Spc0[0]/Spc1[0]+0.5);
	Spc1[0]=Spc0[0]*Dim0[0]/Dim1[0];
	
	Dim1[1]=floor(Dim0[1]*Spc0[1]/Spc1[1]+0.5);
	Spc1[1]=Spc0[1]*Dim0[1]/Dim1[1];
	
	float* Image_Out = (float*)malloc(sizeof(float)*Dim1[0]*Dim1[1]);  
	
	for(i=0;i<Dim1[0];i++)
		for(j=0;j<Dim1[1];j++)
			Image_Out[i+j*Dim1[0]]=Interpolation (Image_In, Dim0[0], Dim0[1], i*Spc1[0]/Spc0[0], j*Spc1[1]/Spc0[1]); 
	
	return Image_Out;
}
	


/*
Compose rotation matrix from x,y,z angles, in rat
INPUT:  Angles[3];
		RotMat[4][4]; //empty matrix  
*/
void Compose_RotMat(float angles_In[3],float RotMat[4][4])
{
	float RxT[3][3];
	float RyT[3][3];
	float RzT[3][3];

	RxT[0][0] = 1;
	RxT[0][1] = 0;
	RxT[0][2] = 0;
	RxT[1][0] = 0;
	RxT[1][1] = cos(angles_In[0]);
	RxT[1][2] = -sin(angles_In[0]);
	RxT[2][0] = 0;
	RxT[2][1] = sin(angles_In[0]);
	RxT[2][2] = cos(angles_In[0]);
	
	RyT[0][0] = cos(angles_In[1]);
	RyT[0][1] = 0;
	RyT[0][2] = sin(angles_In[1]);
	RyT[1][0] = 0;
	RyT[1][1] = 1;
	RyT[1][2] = 0;
	RyT[2][0] = -sin(angles_In[1]);
	RyT[2][1] = 0;
	RyT[2][2] = cos(angles_In[1]);
	
	RzT[0][0] = cos(angles_In[2]);
	RzT[0][1] = -sin(angles_In[2]);
	RzT[0][2] = 0;
	RzT[1][0] = sin(angles_In[2]);
	RzT[1][1] = cos(angles_In[2]);
	RzT[1][2] = 0;
	RzT[2][0] = 0;
	RzT[2][1] = 0;
	RzT[2][2] = 1;
	
	identityMatrix_float(&RotMat[0][0],4);
	
	float temp_array1[3][3]={0};
	float temp_array2[3][3]={0};
	arrayMult(&RzT[0][0],&RyT[0][0],&temp_array1[0][0],3,3, 3);
	arrayMult(&temp_array1[0][0],&RxT[0][0],&temp_array2[0][0],3,3,3);
	arrayTrans(&temp_array2[0][0], &temp_array1[0][0], 3,3);
	
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			RotMat[i][j]=temp_array1[i][j];

}



void identityMatrix_float(float *matrix_In,int size)
{
	int i,j;
	
	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
		{
			if(i==j)
				matrix_In[i*size+j]=1;
			else
				matrix_In[i*size+j]=0;
		}
}


/*
array multiplication
array1: size m*n
array2: size n*t
output array: size m*t
*/
void arrayMult(float *array1,float *array2, float *array_Out,unsigned int m,unsigned int n,unsigned int t)
{
	int i,j,k;
	float temp;
	float *temp_Out=(float*)malloc(sizeof(float)*m*t);
	
	for(i=0;i<m;i++)
		for(j=0;j<t;j++)
		{
			temp=0;
			for(k=0;k<n;k++)
				temp=array1[i*n+k]*array2[k*t+j] + temp;
			temp_Out[i*t+j] = temp;
		}
	for(i=0;i<m*t;i++)
		array_Out[i]=temp_Out[i];
	free(temp_Out);
}


/*
transpose array
input array size: m*n
output array size: n*m
*/
void arrayTrans(float *array_In, float *array_Out, unsigned int m, unsigned int n)
{
	int i,j;
	
	float temp_array[n][m];
	
	for(i=0;i<n;i++)
		for(j=0;j<m;j++)
			temp_array[i][j]=array_In[j*n+i];
	
	for(i=0;i<n;i++)
		for(j=0;j<m;j++)
			array_Out[i*m+j] = temp_array[i][j];
}


/*Simple transformation for the mesh (3 DOF of translation, 1 DOF of scaling, no rotation)
;INPUT:
;      Verts:        the 3-by-n vertices array of the mesh
;      Center:       the origine for the transformation
;      Shifts:       the shift vector
;      Scale:        the scaling parameters
;
;OUTPUT:
;      Verts1: the deformed vertices
*/


void Simple_Transform_Mesh(float *Verts, float *Center, float *Shifts, float Scale, float *Verts1, unsigned int n_Verts)
{
     unsigned int i,j;
	 
     //Centerlization
     for (j=0;j<n_Verts;j++) for(i=0;i<3;i++) Verts1[i+j*3]=Verts[i+j*3]-Center[i];
	
	 //Scaling
	 for (j=0;j<n_Verts*3;j++) Verts1[j]=Verts1[j]*Scale;

     //Translation
	 for (j=0;j<n_Verts;j++) for(i=0;i<3;i++) Verts1[i+j*3]=Verts1[i+j*3]+Shifts[i];

	 //Decenterlization
	 for (j=0;j<n_Verts;j++) for(i=0;i<3;i++) Verts1[i+j*3]=Verts1[i+j*3]+Center[i];
       
}



void Matrix_Multiply(float *M1, unsigned int m1, unsigned int n1, float *M2, unsigned int m2, float *M3)
{
	float tmp;
	float *Ma;
	unsigned int i, j, k, i2, i3=0;
	for(j=0; j<n1; j++){
		for(i=0; i<m2; i++){
			Ma=M1+j*m1;	    
            tmp=0;
			i2=i;
			for (k=0; k<m1; k++){
                if( (*Ma!=0) && (M2[i2]!=0)) tmp+=*Ma*M2[i2];
				Ma++;
				i2+=m2;
			}
			M3[i3++]=tmp;
		}
	}
}

   
void close_3x3cube(unsigned char *BW, unsigned int Dim[3])
{
	
	int i,j,k,m; 
	int s, l, v, id, ind;
	
	l = Dim[0];
	s = Dim[0]*Dim[1];
	v = Dim[0]*Dim[1]*Dim[2];
	
	
	//the offset for searching neighborhood, 6-connection be the first 6th element, fllowed by the other 20 connections
	int offset[] = {-1,1,-l-1,-l,-l+1,l-1,l,l+1,-l-1-s,-l-s,-l+1-s,-1-s,-s,1-s,l-1-s,l-s,l+1-s,-l-1+s,-l+s,-l+1+s,-1+s,s,1+s,l-1+s,l+s,l+1+s};
	
	
	
	//dilate
	unsigned char *BW1 = calloc(sizeof(unsigned char), v);
	for (k=1;k<(Dim[2]-1);k++)
	for (j=1;j<(Dim[1]-1);j++)
	for (i=1;i<(Dim[0]-1);i++)
	{
	  id=i+j*l+k*s;
	  if (BW[id]==1)
	  {
		  BW1[id]=1;
	      for (m=0;m<26;m++)
		  {
			  ind=id+offset[m];
			  if (BW1[ind]!=1) BW1[ind]=1; 										 
		  }
	  }
	}
	
	
	unsigned char *pp=BW, *pp1=BW1;
	for (i=0;i<v;i++,pp++,pp1++) if(*pp1==1 && *pp==0) *pp=1;
	
	//erode
	for (k=1;k<(Dim[2]-1);k++)
	for (j=1;j<(Dim[1]-1);j++)
	for (i=1;i<(Dim[0]-1);i++)
	{
	  id=i+j*l+k*s;
	  if (BW1[id]==1)
	  {
		  for (m=0;m<26;m++)
		  {
			  ind=id+offset[m];
			  if (BW1[ind]==0) {BW[id]=0; break;} 										 
		  }
	  }
	}
	
	free(BW1);
	
	
}
