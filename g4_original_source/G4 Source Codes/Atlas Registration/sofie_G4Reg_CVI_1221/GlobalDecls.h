


extern FILE *logFile;
//extern char studyDataFolder[MAX_FILE_NAME_LENGTH];  //study data folder got from GUI 
extern char *studyDataFolder; 


extern char File_SSM[];
extern char File_CSM[];

extern char XrayFileName[MAX_FILE_NAME_LENGTH]; 
extern char SidePhotoFileName[MAX_FILE_NAME_LENGTH];
extern char OutputFileName[MAX_FILE_NAME_LENGTH];

extern float Weight_Combine[2];
extern unsigned long nBufSize; //raw image size; 

extern struct Para_Img
{
float fc[2];
float cc[2];
float alpha_c;
float kc[5];
float TC[3];
float RC[9];
} Para_Cam,Para_Xray;

extern struct MaskCam
{
	float x_left;
	float x_right;
	float b_ChamberEdge;
	float b_CoverEdge;
}Mask_Cam;

extern float  M_M2P[16];
extern float  M_P2M[16];
extern float  M_init[16];
extern float  M_AT2M[16];
extern float  BndBox_PET[6];

extern float  CastRange_Xray[2];
extern float  Spc_Xray_Detector[2];  
extern float  Spc_Cam_Detector[2];
extern float  Dim_Xray_Detector[2];  
extern float  Dim_Cam_Detector[2];
extern float  Rebin_Xray_Reg; //rebinning factor for making X-ray image for registration
extern float  Spc_Cam_Reg[2];
extern float  Spc_Xray_Reg[2];
extern unsigned int Dim_Xray[2];
extern unsigned int Dim_Cam[2];
extern float fl_Xray;
extern float fl_Cam; 
extern unsigned int Dim_Out_xray[2];
extern unsigned int Dim_Out_cam[2]; 


extern float Spc_Fill[3];
extern unsigned int Dim_Fill[3];
  
extern float  BndBx_Xray_Reg[4];//X-ray image output bounding box region for whole system display
extern float  BndBx_Cam_Reg[4];//camera image output bounding box region for whole system display

extern long numPix_out_cam;
extern float **cord_cam;

extern long numPix_out_xray;
extern float **cord_xray;


extern float Src_Cam[3];
extern float Sz_CamDtr[2];
extern float BndBx_Cam[4];
extern float Sz_CamFOV[2];
extern float Dtctr_Cam[3];
extern float xvec_Cam[3];
extern float yvec_Cam[3];


extern float Spc_Fill[3];
extern unsigned int Dim_Fill[3];
extern float Offset_Fill[3]; 
