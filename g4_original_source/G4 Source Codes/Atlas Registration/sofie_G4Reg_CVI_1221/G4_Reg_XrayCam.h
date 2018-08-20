#ifndef _G4_REG_XRAYCAM_H_
#define _G4_REG_XRAYCAM_H_

extern "C" __declspec(dllexport)  
float G4_Init_XrayCam(float *t, float *SSM_meanVerts,  float SSM_meanScale, float *MAT2W, float *M_M2P, float *BndBox_PET, unsigned int num_Verts, 
					  float *ImgXray, float *Src_Xray, unsigned int *DimXray, float *Dtctr_Xray, float *xvec_Xray, float *yvec_Xray, float *Sz_XrayFOV,
				      unsigned int *ind_Skin, unsigned int *TRI_Skin, unsigned int numVerts_Skin, unsigned int numTRI_Skin);

extern "C" __declspec(dllexport)  
float G4_CSM_XrayCam(float *t, 
				     float *SSM_meanVerts,  float *SSM_eigenvect, float SSM_meanScale, unsigned int *SSM_LabsVerts, float *inv_SSMCov, float SSM_TH, 
				     float *WightCombine, unsigned int numPC, float *MAT2W, float *M_M2P, float *BndBox_PET, unsigned int *TRI, unsigned int num_TRI, unsigned int num_Verts, 
				     float *ImgXray, float *Src_Xray, unsigned int *DimXray, float *Dtctr_Xray, float *xvec_Xray, float *yvec_Xray, float *Sz_XrayFOV, 
                     float *ImgCam,  float *Src_Cam,  unsigned int *DimCam,  float *xyz_00_Cam, float *xvec_Cam,  float *yvec_Cam,  float *SpcCam, 
				     unsigned int *ind_Skin, unsigned int *ind_Mask, unsigned int num_Mask, unsigned int *TRI_Skin, unsigned int numVerts_Skin, unsigned int numTRI_Skin, 
				     unsigned int *Label_Prj, float *Intensity_Fill);



#endif