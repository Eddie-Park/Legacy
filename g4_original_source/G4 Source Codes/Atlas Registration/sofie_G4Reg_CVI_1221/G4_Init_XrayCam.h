#ifndef _G4_INIT_XRAYCAM_H_
#define _G4_INIT_XRAYCAM_H_

extern "C" __declspec(dllexport)  
float G4_Init_XrayCam(float *t, float *SSM_meanVerts,  float SSM_meanScale, float *MAT2W, unsigned int num_Verts, 
					  float *ImgXray, float *Src_Xray, unsigned int *DimXray, float *Dtctr_Xray, float *xvec_Xray, float *yvec_Xray, float *Sz_XrayFOV,
				      unsigned int *ind_Skin, unsigned int *TRI_Skin, unsigned int numVerts_Skin, unsigned int numTRI_Skin);

#endif