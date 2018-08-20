#include "windows.h"
#include <analysis.h>
#include <tcpsupp.h>
#include <mmsystem.h>  
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>

#include "Headers.h"
#include "atlasRegistration.h" 
#include "GlobalDefs.h"
#include "AtlasRegistrationMain.h"

#include "G4_Reg_XrayCam.h" 


static int atlaspanel;

static int mainInitialization(void) ;
static int Run_G4_Reg_CSMAtl(void);
int Load_Atlas(struct SSM *Ssm);
int Load_CSM(struct CSM *Csm, struct SSM *Ssm_o, struct SSM *Ssm_i);   
float* JointReg_CSM_Xray0Cam90(struct SSM *Ssm, float* Img_Xray, float *Img_Cam);
struct Mesh *Reg_TrunkCSM_2_WholebodySSM(struct SSM *Ssm, struct CSM *Csm, struct SSM *Ssm_o, struct SSM *Ssm_i);


int main (int argc, char *argv[])
{
	
	if (InitCVIRTE (0, argv, 0) == 0)
	return -1;	/* out of memory */
	mainInitialization();
	Run_G4_Reg_CSMAtl();
	
	DisplayPanel (atlaspanel);
	RunUserInterface ();
	DiscardPanel (atlaspanel);
//	mainTermination();
	return 0;
}


static int mainInitialization()
{
	
	DWORD errcode;
	if ((atlaspanel = LoadPanel (0, "AtlasRegistrationMain.uir", AtlasPanel)) < 0)
		return -1;
	

	return 0;
}	


static int Run_G4_Reg_CSMAtl()
{
	
	unsigned int EntrySize = 2000;
	unsigned long nBytes;
	char tempStr[30];
	long numVerts_H;
	unsigned long nBytesWritten;
	int i,j;
	
	nBufSize = MAX_IMAGE_LENGTH; 
	
	unsigned short* xrayImgBuf = (unsigned short*)malloc(nBufSize);  //xray image buffer 
	unsigned char* sidePhotoBuf=(unsigned char*)malloc(nBufSize);    //side photo buffer  
	
	Spc_Xray_Reg[0]=Spc_Xray_Detector[0]*Rebin_Xray_Reg;
	Spc_Xray_Reg[1]=Spc_Xray_Detector[1]*Rebin_Xray_Reg;
	

  	//Output Files
  	//File_FillCT = 'FillCT.mhd';
 	 //File_RegRslt = 'RegMesh.atl';
	
	//run parameter initialization
	pinhole_Xray_Para_Initial();
    pinhole_Cam_Para_Initial();

	// read raw xray image and pinhole
	HANDLE xFile = CreateFile ("xrayImage.raw",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if(xFile == INVALID_HANDLE_VALUE)
	{
		MessagePopup ("", "Error opening xray image");
		return 0;
	}  
	if(ReadFile(xFile, xrayImgBuf, nBufSize-1, &nBytes, NULL)==0)
	{
		MessagePopup ("", "Error reading xray image");
		return 0;
	}
	pinhole_Xray(xrayImgBuf);
	
	  
	// cast x-ray pixel value into 0~255
	float CastRange[]={0.4, 0.9};
	float tmp, low, high;
	unsigned short max_v=0, min_v;
	for (i=0;i<Dim_Out_xray[0]*Dim_Out_xray[1];i++){
		if (xrayImgBuf[i]>max_v) max_v=xrayImgBuf[i];
	}
	min_v = max_v;
	for (i=0;i<Dim_Out_xray[0]*Dim_Out_xray[1];i++){
		if (xrayImgBuf[i]<min_v) min_v=xrayImgBuf[i];
	}
	low = min_v + CastRange[0]*(max_v-min_v);
	high = min_v + CastRange[1]*(max_v-min_v);
	for (i=0;i<Dim_Out_xray[0]*Dim_Out_xray[1];i++){ 
		tmp = 255.0* (xrayImgBuf[i]-low)/(high-low);
		if (tmp<0) tmp=0;
		if (tmp>255) tmp=255;
		xrayImgBuf[i] = floor(tmp);
	}
	
	
	
	//read raw side photo and pinhole 
	
	HANDLE sFile = CreateFile ("sidePhoto.raw",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if(sFile == INVALID_HANDLE_VALUE)
	{
		MessagePopup ("", "Error opening side photo");
		return 0;
	}  
	if(ReadFile(sFile, sidePhotoBuf, nBufSize-1, &nBytes, NULL)==0)
	{
		MessagePopup ("", "Error reading side photo");
		return 0;
	}
	//for debug only

	pinhole_SidePhoto(sidePhotoBuf);
	
	//Convert the images to float type
	float* Img_Xray = (float*)malloc(sizeof(float)*Dim_Out_xray[0]*Dim_Out_xray[1]);     
	float* Img_Cam=(float*)malloc(sizeof(float)*Dim_Out_cam[0]*Dim_Out_cam[1]);        
	for(i=0;i<Dim_Out_xray[0]*Dim_Out_xray[1];i++) Img_Xray[i]=(float)xrayImgBuf[i];
	for(i=0;i<Dim_Out_cam[0]*Dim_Out_cam[1];i++)   Img_Cam[i]=(float)sidePhotoBuf[i];
	free(xrayImgBuf);
	free(sidePhotoBuf);
	
	/*
	//save xray and side photo for debug mode
	HANDLE hFile = CreateFile("xrayPinhole.raw", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE rFile = CreateFile("sidePhotoPinhole.raw",GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL); 
	if (hFile != INVALID_HANDLE_VALUE && rFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, Img_Xray, sizeof(float)*Dim_Out_xray[0]*Dim_Out_xray[1], &nBytesWritten, NULL); 
		CloseHandle(hFile); 
		WriteFile(rFile, Img_Cam,sizeof(float)*Dim_Out_cam[0]*Dim_Out_cam[1], &nBytesWritten, NULL);
		CloseHandle(rFile); 
	}  
	 */
	
	

	//Load atlas
	struct SSM *Ssm = &SSM;
	Load_Atlas(Ssm);
	
	
	//initialize Atlas
	float T[3]={-11,7.5,-13};
	float R[3] = {atan(1)*4, 0, 0}; 
	float O[3]={0.0};
	
	for(i=0;i<Ssm->num_VertsMean;i++) Ssm->Verts[i]=Ssm->Verts_Mean[i]*Ssm->Mean_Scale;  
	
	float numVerts_Skin=0.0;
	for(i=0;i<Ssm->num_Verts;i++)
	{
		if (Ssm->Labs_Verts[i]==1)
		{
			O[0]+=Ssm->Verts[i*3];
			O[1]+=Ssm->Verts[i*3+1];
			O[2]+=Ssm->Verts[i*3+2];
			numVerts_Skin++;
		}
	}
	for(i=0;i<3;i++) O[i]=O[i]/numVerts_Skin;
	
	float Mt_CT[4][4];
	float Mc_CT[4][4];
	identityMatrix_float(&Mt_CT[0][0],4);
	identityMatrix_float(&Mc_CT[0][0],4);

	Mt_CT[3][0]=T[0];
	Mt_CT[3][1]=T[1];
	Mt_CT[3][2]=T[2];

	//centerize in the CT coordinate system 
	Mc_CT[3][0]=-O[0];
	Mc_CT[3][1]=-O[1];
	Mc_CT[3][2]=-O[2];

	float Mr_CT[4][4],Mr_CT2X[4][4];
	Compose_RotMat(R,Mr_CT);
	float R2[3]={atan(1)*2,0,atan(1)*2};
	Compose_RotMat(R2,Mr_CT2X);	// rotation from CT coordinate system to X-ray pinhole coordinate system

	float Mr_X2W[4][4];
	identityMatrix_float(&Mr_X2W[0][0],4); 
	for(i=0;i<=2;i++)
		for(j=0;j<=2;j++)
			Mr_X2W[i][j]=Para_Xray.RC[i*3+j];  //rotation from X-ray pinhole coordinate system to world coordinate system 

    float temp1[4][4],temp2[4][4];
	arrayMult(&Mc_CT[0][0],&Mr_CT[0][0],&temp1[0][0],4,4,4);
	arrayMult(&temp1[0][0],&Mt_CT[0][0],&temp2[0][0],4,4,4);
	arrayMult(&temp2[0][0],&Mr_CT2X[0][0],&temp1[0][0],4,4,4);
	arrayMult(&temp1[0][0],&Mr_X2W[0][0],&M_AT2W[0][0],4,4,4); 

	float *Verts_tmp = (float*)malloc(sizeof(float)*Ssm->num_Verts*4);
	for(i=0;i<Ssm->num_Verts;i++)
	{
	  Verts_tmp[i*4]  = Ssm->Verts[i*3];
	  Verts_tmp[i*4+1]= Ssm->Verts[i*3+1];  
	  Verts_tmp[i*4+2]= Ssm->Verts[i*3+2];
	  Verts_tmp[i*4+3]= 1;
	}
	arrayMult(Verts_tmp,&M_AT2W[0][0],Verts_tmp,Ssm->num_Verts,4,4); 
	for(i=0;i<Ssm->num_Verts;i++)
	{
	  Ssm->Verts[i*3] = Verts_tmp[i*4];
	  Ssm->Verts[i*3+1] = Verts_tmp[i*4+1];  
	  Ssm->Verts[i*3+2] = Verts_tmp[i*4+2];
	}
	
		
	//structure_modify_multiple, SSM_H, {Verts:Verts_mean, Conn:SSM_H.Conn_mean};	 ??
	
	//Get the transformation from MARS to CT, so as to enable low-contrast organ CSM registration and mesh filling
	float Mr_W2X[4][4],Mr_X2CT[4][4],Mt_invCT[4][4],Md_CT[4][4],Mr_invCT[4][4],M_W2AT[4][4];
	arrayTrans(&Mr_X2W[0][0], &Mr_W2X[0][0],4,4);
	arrayTrans(&Mr_CT2X[0][0], &Mr_X2CT[0][0],4,4);
	identityMatrix_float(&Mt_invCT[0][0],4);
    for(i=0;i<3;i++)
		Mt_invCT[3][i]=-T[i];
	identityMatrix_float(&Md_CT[0][0],4);
	for(i=0;i<3;i++)
		Md_CT[3][i]=O[i]; //de-centerize center in the CT coordinate system 
    arrayTrans(&Mr_CT[0][0], &Mr_invCT[0][0],4,4);
  
	arrayMult(&Mr_W2X[0][0],&Mr_X2CT[0][0],&temp1[0][0],4,4,4);
	arrayMult(&temp1[0][0],&Mt_invCT[0][0],&temp2[0][0],4,4,4);
	arrayMult(&temp2[0][0],&Mr_invCT[0][0],&temp1[0][0],4,4,4);
	arrayMult(&temp1[0][0],&Md_CT[0][0],&M_W2AT[0][0],4,4,4); 
	

	//Run registration
	Ssm->Verts = JointReg_CSM_Xray0Cam90(Ssm, Img_Xray, Img_Cam);
	
	
	//estimate internal organs
	struct CSM *Csm = &CSM;
	struct SSM *Ssm_o=&SSM_O, *Ssm_i=&SSM_I; 
	Load_CSM(Csm, Ssm_o, Ssm_i);
	struct Mesh *Mesh_All = Reg_TrunkCSM_2_WholebodySSM(Ssm, Csm, Ssm_o, Ssm_i);

	
	//transform the registered mesh back to atlas space
	float *Verts_tmp1 = (float*)malloc(sizeof(float)*Mesh_All->num_Verts*4);
    for(i=0;i<Mesh_All->num_Verts;i++)
	{
	  Verts_tmp1[i*4]  = Mesh_All->Verts[i*3];
	  Verts_tmp1[i*4+1]= Mesh_All->Verts[i*3+1];  
	  Verts_tmp1[i*4+2]= Mesh_All->Verts[i*3+2];
	  Verts_tmp1[i*4+3]= 1;
	}
	arrayMult(Verts_tmp1,&M_W2AT[0][0],Verts_tmp1,Mesh_All->num_Verts,4,4);   
	for(i=0;i<Ssm->num_Verts;i++)
	{
	  Mesh_All->Verts[i*3] = Verts_tmp1[i*4];
	  Mesh_All->Verts[i*3+1] = Verts_tmp1[i*4+1];  
	  Mesh_All->Verts[i*3+2] = Verts_tmp1[i*4+2];
	}
	

    //Generate filled CT
	float Spc_Fill[3] = {0.2, 0.2, 0.4};
    unsigned int Dim_Fill[3] = {256, 256, 248};
    float Shifts[3]; for (i=0;i<3;i++) Shifts[i]=Dim_Fill[i]*Spc_Fill[i]/2;
	unsigned int num_vox = Dim_Fill[0]*Dim_Fill[1]*Dim_Fill[2];
	
	for (i=0;i<Mesh_All->num_Verts;i++) for (j=0;j<3;j++) Mesh_All->Verts[j+i*3]=(Mesh_All->Verts[j+i*3]+Shifts[j])/Spc_Fill[j];
	
	unsigned char *Vlm_Lab = Fill_MultiMesh(Mesh_All, Mesh_All->Labs_Mesh, Dim_Fill, Mesh_All->num_Mesh);
																											   
	unsigned char *Vlm_CT = calloc(num_vox,sizeof(unsigned char));
	unsigned char *pp0=Vlm_Lab, *pp1=Vlm_CT;
    for (i=0;i<num_vox;i++,pp0++,pp1++)
	{
		if (*pp0==1)    *pp1=20;
	    if (*pp0==21)   *pp1=5;
	    if (*pp0==2)    *pp1=100;   
	    if (*pp0==9)    *pp1=30;   
	    if (*pp0==18)   *pp1=40;   
	    if (*pp0==16)   *pp1=35;   
	    if (*pp0==190)  *pp1=31;   
	    if (*pp0==191)  *pp1=32;   
	}

	
	HANDLE vFile = CreateFile("Filled_CT.raw",GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL); 
	if (vFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(vFile, Vlm_CT, sizeof(unsigned char)*num_vox, &nBytesWritten, NULL); 
		CloseHandle(vFile); 
	}  
	 
	
	
	free(Img_Xray);
	free(Img_Cam);
	free_SSM(Ssm);
	free_SSM(Ssm_o); 
	free_SSM(Ssm_i); 
	free_CSM(Csm);
	free(Verts_tmp); 
	free(Verts_tmp1);
	free(Vlm_Lab);
	free(Vlm_CT);
	free_Mesh(Mesh_All);
		
	
	return 1;
	
}

	
int Load_Atlas(struct SSM* Ssm)
{
	
	char tempStr[30];
	long arraySize[2];
	unsigned int i;
	
	FILE *aFile=fopen("SSM_Org-2-21-1_Sbj1to118_v6749.atl", "r");
		
	if (aFile!=NULL)
	{
	
     while(fscanf(aFile, "%s",tempStr) != EOF)
	 {
		
		if(!strcmp(tempStr,"VERTS_MEAN"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm->num_VertsMean=arraySize[0];
			Ssm->Verts_Mean=(float *)malloc(sizeof(float)*arraySize[0]);
			Ssm->Verts=(float *)malloc(sizeof(float)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%f",&Ssm->Verts_Mean[i]);
		}
		if(!strcmp(tempStr,"TRI"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Ssm->Tri=(unsigned int *)malloc(sizeof(unsigned int)*arraySize[0]*arraySize[1]);
			Ssm->num_Tri=arraySize[1];
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%ld",&Ssm->Tri[i]);
		}
		if(!strcmp(tempStr,"EIGENVAL"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm->Eigenval=(float *)malloc(sizeof(float)*arraySize[0]);
			Ssm->num_Eigenval=arraySize[0];
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%f",&Ssm->Eigenval[i]);
		} 
		if(!strcmp(tempStr,"EIGENVECT"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Ssm->num_Eigenvect[0]=arraySize[0];
			Ssm->num_Eigenvect[1]=arraySize[1];
			Ssm->Eigenvect=(float*)malloc(sizeof(float)*arraySize[0]*arraySize[1]);
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%f",&Ssm->Eigenvect[i]);
		}
		if(!strcmp(tempStr,"INV_COV"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Ssm->Inv_COV=(float*)malloc(sizeof(float)*arraySize[0]*arraySize[1]);
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%f",&Ssm->Inv_COV[i]);
		}
		if(!strcmp(tempStr,"ENERGY_TH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%f",&Ssm->Energy_Th);
		}
		if(!strcmp(tempStr,"NUM_VERTS"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&Ssm->num_Verts);
		}
		if(!strcmp(tempStr,"MEANSCALE"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%f",&Ssm->Mean_Scale);
		}
		if(!strcmp(tempStr,"LABS_VERTS"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm->Labs_Verts=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm->Labs_Verts[i]);
		}
		if(!strcmp(tempStr,"LABS_MESH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm->num_Mesh=arraySize[0];
			Ssm->Labs_Mesh=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm->Labs_Mesh[i]);
		}
		if(!strcmp(tempStr,"NUMVERTS_EACH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm->numVerts_Each=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm->numVerts_Each[i]);
		}
		if(!strcmp(tempStr,"NUMTRI_EACH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm->numTri_Each=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm->numTri_Each[i]);
		}
		
	 }

	}
	
	fclose(aFile);
	return 1;
	
}



int Load_CSM(struct CSM *Csm, struct SSM *Ssm_o, struct SSM *Ssm_i)
{
	
	char tempStr[30];
	long arraySize[2];
	int i,ae;
	FILE *aFile=fopen("CSM-Trunk_OrgOut-2-21-1_v6749_OrgIn-9-18-16-190-191_v4527.atl", "r");
		
	if (aFile!=NULL)
	{
	 while(fscanf(aFile, "%s",tempStr) != EOF)    
	 { 
		
		if(!strcmp(tempStr,"IND_SKIN"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Csm->num_indSkin=arraySize[0]; 
			Csm->ind_Skin=(unsigned int *)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ld",&Csm->ind_Skin[i]);
		}
		if(!strcmp(tempStr,"IND_SKEL"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Csm->num_indSkel=arraySize[0]; 
			Csm->ind_Skel=(unsigned int *)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ld",&Csm->ind_Skel[i]);
		}
		if(!strcmp(tempStr,"MEANCOEF_OUTER"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Csm->num_CoefInner=arraySize[0]; 
			Csm->MeanCoef_Outer=(float *)malloc(sizeof(float)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%f",&Csm->MeanCoef_Outer[i]);
		}
		if(!strcmp(tempStr,"MEANCOEF_INNER"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Csm->num_CoefInner=arraySize[0]; 
			Csm->MeanCoef_Inner=(float *)malloc(sizeof(float)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%f",&Csm->MeanCoef_Inner[i]);
		}
		if(!strcmp(tempStr,"INV_COV_II"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Csm->invCov_II=(float *)malloc(sizeof(float)*arraySize[0]*arraySize[1]);
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%f",&Csm->invCov_II[i]);
		}
		if(!strcmp(tempStr,"COVIO_INVCOVO"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Csm->CovIO_invCovO=(float *)malloc(sizeof(float)*arraySize[0]*arraySize[1]);
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%f",&Csm->CovIO_invCovO[i]);
		}
		if(!strcmp(tempStr,"NUMPC_OUTER"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
		    fscanf(aFile,"%ld",&Csm->numPC_Outer);
		}
		if(!strcmp(tempStr,"NUMPC_INNER"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
		    fscanf(aFile,"%ld",&Csm->numPC_Inner);
		}
		if(!strcmp(tempStr,"SSM_O-VERTS_MEAN"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm_o->Verts_Mean=(float *)malloc(sizeof(float)*arraySize[0]);
			Ssm_o->Verts=(float *)malloc(sizeof(float)*arraySize[0]);
			Ssm_o->num_VertsMean=arraySize[0];
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%f",&Ssm_o->Verts_Mean[i]);
		}
		if(!strcmp(tempStr,"SSM_O-EIGENVECT"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Ssm_o->Eigenvect=(float *)malloc(sizeof(float)*arraySize[0]*arraySize[1]);
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%f",&Ssm_o->Eigenvect[i]);
		}
		if(!strcmp(tempStr,"SSM_O-NUM_VERTS"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
		    fscanf(aFile,"%ld",&Ssm_o->num_Verts);
		}
		if(!strcmp(tempStr,"SSM_I-VERTS_MEAN"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm_i->Verts_Mean=(float *)malloc(sizeof(float)*arraySize[0]);
			Ssm_i->Verts=(float *)malloc(sizeof(float)*arraySize[0]);   
			Ssm_i->num_VertsMean=arraySize[0];     
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%f",&Ssm_i->Verts_Mean[i]);
		}
		if(!strcmp(tempStr,"SSM_I-TRI"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Ssm_i->Tri=(unsigned int *)malloc(sizeof(unsigned int)*arraySize[0]*arraySize[1]);
			Ssm_i->num_Tri=arraySize[1]; 
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%ld",&Ssm_i->Tri[i]);
		}
		if(!strcmp(tempStr,"SSM_I-EIGENVECT"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&arraySize[1]);
			Ssm_i->num_Eigenvect[0]=arraySize[0];
			Ssm_i->num_Eigenvect[1]=arraySize[1];
			Ssm_i->Eigenvect=(float*)malloc(sizeof(float)*arraySize[0]*arraySize[1]);
			for(i=0;i<arraySize[0]*arraySize[1];i++)
			fscanf(aFile,"%f",&Ssm_i->Eigenvect[i]);
		}
		if(!strcmp(tempStr,"SSM_I-NUM_VERTS"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			fscanf(aFile,"%ud",&Ssm_i->num_Verts);
		}
		if(!strcmp(tempStr,"SSM_I-LABS_VERTS"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm_i->Labs_Verts=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm_i->Labs_Verts[i]);
		}
		if(!strcmp(tempStr,"SSM_I-LABS_MESH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm_i->num_Mesh=arraySize[0];
			Ssm_i->Labs_Mesh=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm_i->Labs_Mesh[i]);
		}
		if(!strcmp(tempStr,"SSM_I-NUMVERTS_EACH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm_i->numVerts_Each=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm_i->numVerts_Each[i]);
		}
		if(!strcmp(tempStr,"SSM_I-NUMTRI_EACH"))
		{
			fscanf(aFile,"%s",tempStr);
			fscanf(aFile,"%ud",&arraySize[0]);
			Ssm_i->numTri_Each=(unsigned int*)malloc(sizeof(unsigned int)*arraySize[0]);
			for(i=0;i<arraySize[0];i++)
			fscanf(aFile,"%ud",&Ssm_i->numTri_Each[i]);
		}
		
	 } //while !EOF

	} //if (aFile!=NULL) 
	
	
	fclose(aFile);
	return 1;
	
}


struct Mesh *Reg_TrunkCSM_2_WholebodySSM(struct SSM *Ssm, struct CSM *Csm, struct SSM *Ssm_o, struct SSM *Ssm_i)  ;    

static int mainTermination()
{
	

return 0;
}


int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
	}
	return 0;
}


/*
Register the whole body statistical atlas to top-view X-ray, side-view camera photo, and/or surface scan data
INPUTS:  SSM:            the statistical shape model (SSM) of high contrast organs
         M_AT2W:         the transformation matrix between atlas (CT) space and MARS space, it is needed to transfer the atlas from SSM space into the world space
         Img_Xray:       the 2D image of top-view X-ray projection, it is the pinhole projection image which is the result of  CamImg_2_PinholeImg.pro
                         Img_Xray should be cast into [0,255] beforhand
         Mesh_Surf:      the mesh of surface scan data, can be optional
         Wight_Combine:  Weights of combinating Xray, Photo and SurfScan; the first element for Xray, second for photo, the 3rd for Surface Scan
                         if all elements are <=0, will be automatically corrected to [0.5, 0.5, 0.]
         Calib_Xray:     the calibration parameters of X-ray system
         BndBox_Xray:    the 2D bounding box region of the X-ray in the pinhole camera coordinate system plane Z=fl, with format [x_min,x_max,y_min,y_max]
         Spc_Xray:       the physical spacing of X-ray image pixels, in the pinhole camera coordinate system plane
         Img_Cam, Calib_Cam, BndBox_Cam, Spc_Cam: similar as those of X-ray
OUTPUT:  Verts_Dfm: the registered mesh vertices of the altas
*/
float* JointReg_CSM_Xray0Cam90(struct SSM *Ssm, float* Img_Xray, float *Img_Cam)
{
	
	unsigned int num_verts,num_Tri;
	unsigned long nBufSize = MAX_IMAGE_LENGTH;
	unsigned int DimXray[2],DimCam[2];
	float SpcXray[2],SpcCam[2];
	float Ssm_Th;
	struct maskCamStruct MaskCam;
	int iR;
	unsigned int i,j;
	
	//Extract skin mesh
	unsigned int Labs_Extract[] = {1};
	unsigned int num_Extract = 1;
	struct Mesh *Mesh_Skin=&M_Skin;
	short errcode=Extract_SubMesh(Ssm, Labs_Extract, num_Extract, Mesh_Skin);
	if(errcode==-1) MessagePopup("","Extract_SubMesh error.");
	
	unsigned int num_IndSkin=0;
	for(i=0;i<Ssm->num_Verts;i++) if(Ssm->Labs_Verts[i]==1) num_IndSkin++;
	unsigned int ind_Skin[num_IndSkin];
	unsigned int ii=0;
	for(i=0;i<Ssm->num_Verts;i++)
		if(Ssm->Labs_Verts[i]==1)				
		{	ind_Skin[ii]=i;
			ii++;
		}
	
	
	//Automatic segmentation of the camera image
	float *Bw_Cam = (float *)malloc(sizeof(float)*Dim_Cam[0]*Dim_Cam[1]);
	float Th0[1];
	
	ISODATA_Thresholding(Img_Cam,Dim_Cam,2,Bw_Cam,Th0);
	
	for(i=0;i<Dim_Cam[0]*Dim_Cam[1];i++)
	{
		Bw_Cam[i]=1-Bw_Cam[i];
		if(Bw_Cam[i]<0) Bw_Cam[i]=0;
	}
	
	
	/*//save file for debug
	unsigned long nBytesWritten;
	HANDLE tFile = CreateFile("BW_Cam.raw", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	if (tFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(tFile, Bw_Cam, sizeof(float)*Dim_Cam[0]*Dim_Cam[1], &nBytesWritten, NULL); 
		CloseHandle(tFile); 
	}*/  
	
	
	//Multi-resolution registration parameters
	int Sample_Rate[3]={4,2,1};
	int num_Resolution=3;
	
	//Xray projection parameters
	unsigned int Label_Prj[3]={1,21,2};	  //the organ labels to be projected
	float Intensity_Fill[3]={7000.0, 3000.0, 20000.0}; //the intensity of the projected organs
	
	//number of SSM principle components to optimize
	float energy_Th=0.95;
	float energy[Ssm->num_Eigenval];
	
	float sum_Eig=0;
	for(i=0;i<Ssm->num_Eigenval;i++)
		sum_Eig=sum_Eig+Ssm->Eigenval[i];
	
	float sum_Eig_temp=0;
	for(i=0;i<Ssm->num_Eigenval;i++)
	{
		sum_Eig_temp=sum_Eig_temp+Ssm->Eigenval[i];
		energy[i]= sum_Eig_temp/sum_Eig;
	}
	unsigned int numPC;
	
	for(i=0;i<Ssm->num_Eigenval;i++)			   //??
	{
		if(energy[i]>energy_Th)
		{
			numPC=i+1;
			break;
		}
	}
	
	Ssm_Th = 2.0*2.0;
	
	//parmeters for X-ray projection
	float Src_Xray[3];
	float Sz_XrayDtr[2];
	float BndBx_Xray[4];
	float Sz_XrayFOV[2];
	float Dtctr_Xray[3];
	float xvec_Xray[3],yvec_Xray[3];
	if(Weight_Combine[0]>0)
	{ 
		
		//the 3D coordinate of X-ray source in world coordinate system
		arrayMult(Para_Xray.TC,Para_Xray.RC, Src_Xray,1,3,3);
		for(i=0;i<3;i++)
			Src_Xray[i]=-Src_Xray[i];
		
		//the focal length of X-ray system
		fl_Xray = (Para_Xray.fc[0]*Spc_Xray_Detector[0]+Para_Xray.fc[1]*Spc_Xray_Detector[1])/2;
		
		Sz_XrayDtr[0]=Dim_Xray_Detector[0]*Spc_Xray_Detector[0];
		Sz_XrayDtr[1]=Dim_Xray_Detector[1]*Spc_Xray_Detector[1];
		
		BndBx_Xray[0]=BndBx_Xray_Reg[0]*Sz_XrayDtr[0];
		BndBx_Xray[1]=BndBx_Xray_Reg[1]*Sz_XrayDtr[0];
		BndBx_Xray[2]=BndBx_Xray_Reg[2]*Sz_XrayDtr[1];
		BndBx_Xray[3]=BndBx_Xray_Reg[3]*Sz_XrayDtr[1];
		
		Sz_XrayFOV[0]=BndBx_Xray[1]-BndBx_Xray[0];
		Sz_XrayFOV[1]=BndBx_Xray[3]-BndBx_Xray[2];
		
		//the 3D coordinate of X-ray detector center in world coordinate system
		Dtctr_Xray[0]=BndBx_Xray[0]+Sz_XrayFOV[0]/2;
		Dtctr_Xray[1]=BndBx_Xray[2]+Sz_XrayFOV[1]/2;
		Dtctr_Xray[2]=fl_Xray;
		
		//from pinhole coordinate system to world coordinate system
		for(i=0;i<3;i++)
			Dtctr_Xray[i]=Dtctr_Xray[i]-Para_Xray.TC[i];
		arrayMult(Dtctr_Xray,Para_Xray.RC, Dtctr_Xray,1,3,3);
		
		for(i=0;i<3;i++)
		{
			xvec_Xray[i]=Para_Xray.RC[i]; //the x-axis of pinhole coordinate system in world coordinate system
			yvec_Xray[i]=Para_Xray.RC[3+i]; //the y-axis of pinhole coordinate system in world coordinate system
		}
	}
	
	//parmeters for Camera projection
	if(Weight_Combine[1]>0)
	{

		arrayMult(Para_Cam.TC,Para_Cam.RC, Src_Cam,1,3,3);
		for(i=0;i<3;i++)
			Src_Cam[i]=-Src_Cam[i];
		
		fl_Cam = (Para_Cam.fc[0]*Spc_Cam_Detector[0]+Para_Cam.fc[1]*Spc_Cam_Detector[1])/2;
		
		Sz_CamDtr[0]=Dim_Cam_Detector[0]*Spc_Cam_Detector[0];
		Sz_CamDtr[1]=Dim_Cam_Detector[1]*Spc_Cam_Detector[1];
		
		BndBx_Cam[0]=BndBx_Cam_Reg[0]*Sz_CamDtr[0];
		BndBx_Cam[1]=BndBx_Cam_Reg[1]*Sz_CamDtr[0];
		BndBx_Cam[2]=BndBx_Cam_Reg[2]*Sz_CamDtr[1];
		BndBx_Cam[3]=BndBx_Cam_Reg[3]*Sz_CamDtr[1];
		
		Sz_CamFOV[0]=BndBx_Cam[1]-BndBx_Cam[0];
		Sz_CamFOV[1]=BndBx_Cam[3]-BndBx_Cam[2];
		
		//the 3D coordinate of camera detector center in world coordinate system
		Dtctr_Cam[0]=BndBx_Cam[0]+Sz_CamFOV[0]/2;
		Dtctr_Cam[1]=BndBx_Cam[2]+Sz_CamFOV[1]/2;
		Dtctr_Cam[2]=fl_Cam;
		
		//from pinhole coordinate system to world coordinate system
		for(i=0;i<3;i++)
			Dtctr_Cam[i]=Dtctr_Cam[i]-Para_Cam.TC[i];
		arrayMult(Dtctr_Cam,Para_Cam.RC, Dtctr_Cam,1,3,3);
		
		for(i=0;i<3;i++)
		{
			xvec_Cam[i]=Para_Cam.RC[i];  //the x-axis of pinhole coordinate system in world coordinate system
			yvec_Cam[i]=Para_Cam.RC[3+i];  //the y-axis of pinhole coordinate system in world coordinate system
		}
	}

	float * ImgXray;	
	float *ImgCam;	
	
	//Initial registration
	SpcXray[0] = Spc_Xray_Reg[0]*Sample_Rate[0];
	SpcXray[1] = Spc_Xray_Reg[1]*Sample_Rate[0];
	
    ImgXray = Resample_Img(Img_Xray, Spc_Xray_Reg, Dim_Xray, SpcXray, DimXray);
	
	float Th1[9];
	float* BW_Xray=(float*)malloc(sizeof(float)*DimXray[0]*DimXray[1]);   //ISODATA THREASHOLDING x-ray image 
	ISODATA_Thresholding(ImgXray, DimXray,10,BW_Xray,Th1);
	
	for(i=0;i<DimXray[0]*DimXray[1];i++)
	{
		if(BW_Xray[i]<0) BW_Xray[i]=0;
		ImgXray[i]=BW_Xray[i];
	}
	
	/*
	//save file for debug 
	tFile = CreateFile("BW_Xray.raw", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	if (tFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(tFile, BW_Xray, sizeof(float)*DimXray[0]*DimXray[1], &nBytesWritten, NULL); 
				CloseHandle(tFile); 
			}  
	*/
	
	

	
	
	//convert M_AT2W to 1D array
	float *M_AT2W_1D = malloc(sizeof(float)*16);
	for (i=0;i<4;i++) for (j=0;j<4;j++) M_AT2W_1D[j+i*4]=M_AT2W[i][j];
	
	//initial T value
	float T0[]={0,0,0,1};
	
	//call function from dll					
	/*float f0; 
	f0=G4_Init_XrayCam(T0, Ssm->Verts_Mean,  Ssm->Mean_Scale, M_AT2W_1D, Ssm->num_Verts,ImgXray, 
					   Src_Xray, DimXray, Dtctr_Xray, xvec_Xray, yvec_Xray, Sz_XrayFOV, ind_Skin,
					   Mesh_Skin->Tri, Mesh_Skin->num_Verts, Mesh_Skin->num_Tri);*/
	
	
	//temperary for debug
	T0[0]=-0.0247422; T0[1]=5.35291; T0[2]=2.88360; T0[3]=1.48516;
	
	
	float Sim[4];
	for(i=0;i<4;i++) Sim[i]=T0[i];
	
	

		
	//Multi-resolution deformable registration	

	float *coef = malloc(sizeof(float)*numPC);
	float  *T1 = malloc(sizeof(float)*(numPC+4));
	unsigned int num_Mask;
	unsigned int *ind_Mask;	
	float xyz_00_Cam[3];
	for(iR=0;iR<num_Resolution;iR++)
	{
	
		if(Sample_Rate[iR]==1)
		{
		
			DimXray[0]=Dim_Xray[0];
			DimXray[1]=Dim_Xray[1];
			SpcXray[0]=Spc_Xray_Reg[0];	  
			SpcXray[1]=Spc_Xray_Reg[1];
		
			DimCam[0]=Dim_Cam[0];
			DimCam[1]=Dim_Cam[1];
			SpcCam[0]=Spc_Cam_Reg[0];
			SpcCam[1]=Spc_Cam_Reg[1];
	
			ImgXray= (float*)malloc(sizeof(float)*DimXray[0]*DimXray[1]);    //xray image buffer
			for(i=0;i<DimXray[0]*DimXray[1];i++) ImgXray[i]=Img_Xray[i];  
	
			ImgCam = (float*)malloc(sizeof(float)*DimCam[0]*DimCam[1]);    //side photo buffer
			for(i=0;i<Dim_Cam[0]*Dim_Cam[1];i++) ImgCam[i]=Bw_Cam[i];
		
			MaskCam.x_left=Mask_Cam.x_left;
			MaskCam.x_right=Mask_Cam.x_right;
			MaskCam.b_ChamberEdge=Mask_Cam.b_ChamberEdge;
			MaskCam.b_CoverEdge=Mask_Cam.b_CoverEdge;
		}
		else
		{
			SpcXray[0]=Spc_Xray_Reg[0]*Sample_Rate[iR];
			SpcXray[1]=Spc_Xray_Reg[1]*Sample_Rate[iR];
			ImgXray = Resample_Img(Img_Xray, Spc_Xray_Reg, Dim_Xray, SpcXray, DimXray);
	
			SpcCam[0]=Spc_Cam_Reg[0]*Sample_Rate[iR];
			SpcCam[1]=Spc_Cam_Reg[1]*Sample_Rate[iR];
			ImgCam = Resample_Img(Bw_Cam, Spc_Cam_Reg, Dim_Cam, SpcCam,DimCam);
	
			MaskCam.x_left = floor(Mask_Cam.x_left/Sample_Rate[iR]);
		    MaskCam.x_right = floor(Mask_Cam.x_right/Sample_Rate[iR]);
		    MaskCam.b_ChamberEdge = floor(Mask_Cam.b_ChamberEdge/Sample_Rate[iR]);
		    MaskCam.b_CoverEdge = floor(Mask_Cam.b_CoverEdge/Sample_Rate[iR]);
		}

		for(i=0;i<3;i++) xyz_00_Cam[i]=Dtctr_Cam[i]- (Sz_CamFOV[0]/2-SpcCam[0]/2)*xvec_Cam[i]- (Sz_CamFOV[1]/2-SpcCam[1]/2)*yvec_Cam[i];

		/*
		//write temporary file for debug 
		tFile = CreateFile("ImgXray.raw", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	    if (tFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(tFile, ImgXray, sizeof(float)*DimXray[0]*DimXray[1], &nBytesWritten, NULL); 
			CloseHandle(tFile); 
		}  
		tFile = CreateFile("ImgCam.raw", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	    if (tFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(tFile, ImgCam, sizeof(float)*DimCam[0]*DimCam[1], &nBytesWritten, NULL); 
			CloseHandle(tFile); 
		}  
		*/
	
		//calculate mask indices
		num_Mask=0; 
		for(i=0;i<DimCam[0];i++)
			for(j=0;j<DimCam[1];j++)
		{
			if(i>MaskCam.x_left && i<MaskCam.x_right && j<MaskCam.b_ChamberEdge && j>MaskCam.b_CoverEdge)
				num_Mask++;
		}

		ind_Mask = malloc(sizeof(unsigned int)*num_Mask);  
		ii=0;
		for(j=0;j<DimCam[1];j++) 
			for(i=0;i<DimCam[0];i++) 
		{
			if(i>MaskCam.x_left && i<MaskCam.x_right && j<MaskCam.b_ChamberEdge && j>MaskCam.b_CoverEdge)
			{
				ind_Mask[ii]=i+j*DimCam[0];
				ii++;
			}
		}
	


	   //Do registration

	   for(i=0;i<numPC+4;i++)
	   {
		   if(i<numPC) T1[i]=0;
		   else T1[i]=Sim[i-numPC];
	   }
	   
	   /*float f1;
	   f1=G4_CSM_XrayCam(T1,Ssm->Verts_Mean, Ssm->Eigenvect, Ssm->Mean_Scale, Ssm->Labs_Verts, Ssm->Inv_COV, Ssm_Th, 
					     Weight_Combine, numPC, M_AT2W_1D, Ssm->Tri, Ssm->num_Tri, Ssm->num_Verts, 
					     ImgXray, Src_Xray, DimXray, Dtctr_Xray, xvec_Xray, yvec_Xray, Sz_XrayFOV, 
	                     ImgCam,  Src_Cam,  DimCam,  xyz_00_Cam, xvec_Cam, yvec_Cam,  SpcCam, 
					     ind_Skin, ind_Mask, num_Mask, Mesh_Skin->Tri, Mesh_Skin->num_Verts, Mesh_Skin->num_Tri, 
					     Label_Prj, Intensity_Fill); */
	  
	   
	   for(i=0;i<numPC;i++) coef[i]=T1[i];
	   for(i=numPC;i<numPC+4;i++) Sim[i-numPC]=T1[i];
	   
		   
	}


	//temperary for debug
	coef[0]=0.194372; coef[1]=-0.157383; coef[2]=0.148579; coef[3]=0.133965; coef[4]=-0.0119295; coef[5]=0.0121915;
	coef[6]=-0.00105738; coef[7]=-0.0602622; coef[8]=0.256047; coef[9]=-0.0275624; coef[10]=0.000391623;
	Sim[0]=-0.0783198; Sim[1]=9.40400; Sim[2]=0.191873; Sim[3]=0.896570;


	//Reconstruct the resultant mesh
	float *Verts_Dfm  = (float *)calloc(3*Ssm->num_Verts, sizeof(float));
	float *Verts_tmp1 = (float *)calloc(4*Ssm->num_Verts, sizeof(float));
	float *Verts_tmp2 = (float *)calloc(4*Ssm->num_Verts, sizeof(float));
	float *Verts_tmp3 = (float *)calloc(3*Ssm->num_Verts, sizeof(float));

	for (j=0;j<numPC;j++) for (i=0;i<3*Ssm->num_Verts;i++) Verts_Dfm[i]+=coef[j]*Ssm->Eigenvect[i+j*3*Ssm->num_Verts];
	for (i=0;i<3*Ssm->num_Verts;i++) Verts_Dfm[i]+=Ssm->Verts_Mean[i];
	for (i=0;i<3*Ssm->num_Verts;i++) Verts_Dfm[i]=Verts_Dfm[i]*Ssm->Mean_Scale; 
	for (i=0;i<4;i++) for (j=0;j<Ssm->num_Verts;j++)
	  if (i==3) {Verts_tmp1[i+j*4]=1;}
	  else {Verts_tmp1[i+j*4]=Verts_Dfm[i+j*3];}
	arrayMult(Verts_tmp1,&M_AT2W[0][0], Verts_tmp2, Ssm->num_Verts,4,4);
	for (i=0;i<3;i++) for (j=0;j<Ssm->num_Verts;j++) Verts_Dfm[i+j*3]=Verts_tmp2[i+j*4];
	float center[3]={0};
	for (i=0;i<3;i++) for (j=0;j<Ssm->num_Verts;j++) center[i]+=Verts_Dfm[i+j*3];
	for (i=0;i<3;i++) center[i]=center[i]/Ssm->num_Verts;
	float shift[3]={Sim[0], Sim[1], Sim[2]};
	Simple_Transform_Mesh(Verts_Dfm, center, shift, Sim[3], Verts_tmp3, Ssm->num_Verts);
	for (i=0;i<3;i++) for (j=0;j<Ssm->num_Verts;j++) Verts_Dfm[i+j*3]=Verts_tmp3[i+j*3];


	//free the temporary variables
	free_Mesh(Mesh_Skin);
	free(Bw_Cam);
	free(M_AT2W_1D);
	free(ImgXray);	
	free(BW_Xray);
	free(ImgCam);
	free(ind_Mask);
	free(coef);
	free(T1);
	free(Verts_tmp1);
	free(Verts_tmp2);
	free(Verts_tmp3);

	return Verts_Dfm;

}


struct Mesh *Reg_TrunkCSM_2_WholebodySSM(struct SSM *Ssm, struct CSM *Csm, struct SSM *Ssm_o, struct SSM *Ssm_i)
{
	
  unsigned int i,j,ii;	
	
  float P_TH = 0.5; 
  float d_max = sqrt(-2*log(P_TH)); 

  //Get Vy of the outer shape
  float *Verts_Skin  = malloc(sizeof(float)*Ssm->numVerts_Each[2]*3);
  float *Verts_Skel  = malloc(sizeof(float)*Ssm->numVerts_Each[0]*3);
  float *Verts_Lungs = malloc(sizeof(float)*Ssm->numVerts_Each[1]*3);
  float *Vy  = malloc(sizeof(float)*Ssm_o->num_Verts*3); 
  int i_skin=0, i_skel=0, i_lung=0;
  for (i=0;i<Ssm->num_Verts;i++)
  {
	  if (Ssm->Labs_Verts[i]==1)  { for(j=0;j<3;j++) Verts_Skin[3*i_skin+j]=Ssm->Verts[i*3+j]; i_skin++;  }
	  if (Ssm->Labs_Verts[i]==2)  { for(j=0;j<3;j++) Verts_Skel[3*i_skel+j]=Ssm->Verts[i*3+j]; i_skel++;  } 
	  if (Ssm->Labs_Verts[i]==21) { for(j=0;j<3;j++) Verts_Lungs[3*i_lung+j]=Ssm->Verts[i*3+j]; i_lung++; } 
  }
  
  ii=0;
  for (i=0;i<Csm->num_indSkel;i++) {
	  for(j=0;j<3;j++) Vy[3*ii+j]=Verts_Skel[Csm->ind_Skel[i]*3+j]; 
	  ii++;
  }
  for (i=0;i<Ssm->numVerts_Each[1];i++) {
	  for(j=0;j<3;j++) Vy[3*ii+j]=Verts_Lungs[i*3+j]; 
	  ii++; 
  }
  for (i=0;i<Csm->num_indSkin;i++) {
	  for(j=0;j<3;j++)  Vy[3*ii+j]=Verts_Skin[Csm->ind_Skin[i]*3+j]; 
	  ii++; 
  } 
  
  free(Verts_Skin);
  free(Verts_Skel);
  free(Verts_Lungs);
  
  //Get Vx
  float *Vx  = malloc(sizeof(float)*Ssm_o->num_Verts*3);       
  for (i=0;i<Ssm_o->num_Verts*3;i++) Vx[i]=Ssm_o->Verts_Mean[i];
  
  
  //normalize Vy into SSM space
  float ox[3]={0}, oy[3]={0}, sx=0, sy=0, scl_y2x;
  for (i=0;i<Ssm_o->num_Verts;i++) for (j=0;j<3;j++) {ox[j]+=Vx[i*3+j]; oy[j]+=Vy[i*3+j];}
  for (j=0;j<3;j++) {ox[j]=ox[j]/Ssm_o->num_Verts; oy[j]=oy[j]/Ssm_o->num_Verts;}	
  for (i=0;i<Ssm_o->num_Verts;i++) for (j=0;j<3;j++) {Vx[i*3+j]=Vx[i*3+j]-ox[j]; Vy[i*3+j]=Vy[i*3+j]-oy[j];}
  for (i=0;i<Ssm_o->num_Verts*3;i++) {sx+=Vx[i]*Vx[i]; sy+=Vy[i]*Vy[i];}
  sx = sqrt(sx); 
  sy = sqrt(sy); 
  scl_y2x = sy/sx;
  for (i=0;i<Ssm_o->num_Verts*3;i++) {Vy[i]=Vy[i]/scl_y2x;}
  float *VyT = malloc(sizeof(float)*Ssm_o->num_Verts*3);
  arrayTrans(Vy, VyT, Ssm_o->num_Verts, 3); 
  
  float M[9], U[9], V[9], VT[9];
  double Md[9], Ud[9], Vd[9], Wd[3];
  Matrix_Multiply(VyT, Ssm_o->num_Verts, 3, Vx, 3, M); 
  for (i=0;i<9;i++) Md[i]=M[i];
  int error = SVDEx(Md, 3, 3, 0, Ud, Wd, Vd);
  for (i=0;i<9;i++) {U[i]=Ud[i]; V[i]=Vd[i];}
  
  float ROT_y2x[9];												
  arrayTrans(V, VT, 3, 3); 
  Matrix_Multiply(U, 3, 3, VT, 3, ROT_y2x);
  Matrix_Multiply(Vy, 3, Ssm_o->num_Verts, ROT_y2x, 3, VyT);
  for (i=0;i<Ssm_o->num_Verts;i++) for (j=0;j<3;j++) Vy[i*3+j]=VyT[i*3+j]+ox[j];
  free(VyT);
  
  //deform model 
  float *a = malloc(sizeof(float) * Ssm_o->num_VertsMean * Csm->numPC_Outer);
  for (i=0; i<Ssm_o->num_VertsMean*Csm->numPC_Outer; i++) a[i]=Ssm_o->Eigenvect[i];
  for (i=0; i<Ssm_o->num_VertsMean; i++) Vy[i]=Vy[i]-Ssm_o->Verts_Mean[i];
  float *coef_outer = malloc(sizeof(float)*Csm->numPC_Outer);
  Matrix_Multiply(a, Ssm_o->num_VertsMean, Csm->numPC_Outer, Vy, 1, coef_outer);
  free(a);
  
    
  //Fit inner organs with conditional gaussian model
  float *b = malloc(sizeof(float)*Csm->numPC_Inner);        
  float *coef_inner =  malloc(sizeof(float)*Csm->numPC_Inner);    
  for (i=0;i<Csm->numPC_Outer;i++) coef_outer[i]=coef_outer[i]-Csm->MeanCoef_Outer[i];
  Matrix_Multiply(Csm->CovIO_invCovO, Csm->numPC_Outer, Csm->numPC_Inner, coef_outer, 1, b); 
  for (i=0;i<Csm->numPC_Inner;i++) coef_inner[i] = Csm->MeanCoef_Inner[i];
  for (i=0;i<Csm->numPC_Inner;i++) coef_inner[i] += b[i];
  
  //Constrain the organ deformation from too large
  float *d_Inner = malloc(sizeof(float)); 
  Matrix_Multiply(coef_inner, Csm->numPC_Inner, 1, Csm->invCov_II, Csm->numPC_Inner, b);
  Matrix_Multiply(b, Csm->numPC_Inner, 1, coef_inner, 1, d_Inner);
  *d_Inner = sqrt(*d_Inner);
  if (*d_Inner>d_max) for (i=0;i<Csm->numPC_Inner;i++) coef_inner[i] = coef_inner[i]*d_max/ *d_Inner;
  free(b);
  free(d_Inner);
  
  //Construct the internal mesh verts
  float *Verts_tmp = (float *)calloc(3*Ssm_i->num_Verts, sizeof(float));
  for (i=0;i<Ssm_i->num_VertsMean;i++) Ssm_i->Verts[i]=Ssm_i->Verts_Mean[i];																															 
  for (j=0;j<Csm->numPC_Inner;j++) for (i=0;i<Ssm_i->num_VertsMean;i++) Ssm_i->Verts[i]+=coef_inner[j]*Ssm_i->Eigenvect[i+j*Ssm_i->num_VertsMean];
  for (i=0;i<3;i++) for (j=0;j<Ssm_i->num_Verts;j++) Ssm_i->Verts[i+j*3]-=ox[i];  
  float ROT_y2x_T[9];	
  arrayTrans(ROT_y2x, ROT_y2x_T, 3, 3);
  Matrix_Multiply(Ssm_i->Verts, 3, Ssm_i->num_Verts, ROT_y2x_T, 3, Verts_tmp);   
  for (i=0;i<3*Ssm_i->num_Verts;i++) Verts_tmp[i]*=scl_y2x;
  for (i=0;i<3;i++) for (j=0;j<Ssm_i->num_Verts;j++) Ssm_i->Verts[i+j*3]=Verts_tmp[i+j*3]+oy[i];
  
  
  //Construct the resultant model mesh
  struct Mesh *Mesh_All = &M_All;
  Combine_MultiMesh(Ssm_i, Ssm, Mesh_All);  
  
  
  free(Vy);
  free(Vx);
  free(coef_outer);
  free(coef_inner);
  free(Verts_tmp);  
  
  return Mesh_All;
  
}
