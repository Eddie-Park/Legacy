
FILE *logFile; 

//char studyDataFolder[MAX_FILE_NAME_LENGTH];  //study data folder got from GUI 
char *studyDataFolder;

char File_SSM[] = "SSM_Org-2-21-1_Sbj1to118_v6749_LegsStreched.atl";
char File_CSM[] = "CSM-Trunk_OrgOut-2-21-1_v6749_OrgIn-9-18-16-190-191_v4527.atl";

char XrayFileName[MAX_FILE_NAME_LENGTH]; 
char SidePhotoFileName[MAX_FILE_NAME_LENGTH];
char OutputFileName[MAX_FILE_NAME_LENGTH];

struct Image *Image_In;
struct Image *Image_Out;

float Weight_Combine[] = {0.5,0.5};
unsigned long nBufSize; //raw image size; 

struct Para_Img
{
float fc[2];
float cc[2];
float alpha_c;
float kc[5];
float TC[3];
float RC[9];
} Para_Cam,Para_Xray;


struct Para_Img Para_Cam = {
	{1238.19033, 1239.76332},
	{333.98139, 145.17983},
	0,
    {-0.30095, 1.19466, -0.00630, 0.00437, 0.00000},
    {-11.043358, 15.679278, 222.855773},
    {0.035793, 0.999357, -0.001968, 
    0.627637, -0.024011, -0.778136,
     -0.777683, 0.026617, -0.628093}
};

struct Para_Img Para_Xray = {
	 {2782.30827, 2782.73554}, 
	 {493.10699, 789.14269}, 
	 0,
     {-0.05938, -1.16495,-0.00404, 0.00327, 0.00000},
     {24.544389, 7.166838, 218.765128},
     {-0.037437, -0.999127, 0.018561,
      -0.728177, 0.014555, -0.685235,
     0.684366, -0.039169, -0.728086}
};


struct maskCamStruct
{
	float x_left;
	float x_right;
	float b_ChamberEdge;
	float b_CoverEdge;
}Mask_Cam;   //****


float  M_M2P[16] = { 0.735342,     0.677274,  0.0239268, 0.000000, 
					-0.000524469, -0.0347372, 0.999397,  0.000000,
            		 0.677696,    -0.734910, -0.0251888, 0.000000, 
            		10.4136, 	  11.4541,   28.3315,    1.000000 };
float  M_P2M[16];
float  M_AT2M[16];  
float M_init[16] = {-1.0,  0.0,  0.0, 0.0, 
					 0.0, -1.0,  0.0, 0.0, 
					 0.0,  0.0,  1.0, 0.0,
             		21.5, 21.0, 50.0, 1.0};

float BndBox_PET[6] = {0.913750, 40.2050, 3.19812, 31.0675, -91.3750, 99.5731};

float  BndBx_Xray_Reg[4]  = {-0.481445, 0.499023, -0.246000, 0.153000};//X-ray image output bounding box region for whole system display
float  BndBx_Cam_Reg[4]   = {-0.434375, 0.423438, 0.0875000, 0.320833};//camera image output bounding box region for whole system display


float Spc_Fill[3] = {0.456875/2, 0.456875/2, 0.456875/2};
unsigned int Dim_Fill[3] = {96*2, 96*2, 208*2};


float  CastRange_Xray[2] = {0.0, 1.0}; //{0.25,0.9};
float  Spc_Xray_Detector[2] = {0.096, 0.096};  
float  Spc_Cam_Detector[2]  = {0.006, 0.006};
float  Dim_Xray_Detector[2] = {1024, 1000};  
float  Dim_Cam_Detector[2]  = {640, 480};
float  Rebin_Xray_Reg = 4; 
float  Spc_Cam_Reg[2]  = {0.012, 0.012};
float  Spc_Xray_Reg[2];
unsigned int Dim_Xray[2];
unsigned int Dim_Cam[2];
float fl_Xray;
float fl_Cam;
unsigned int Dim_Out_xray[2];
unsigned int Dim_Out_cam[2]; 


long numPix_out_cam;
float **cord_cam;

long numPix_out_xray;
float **cord_xray;

float Src_Cam[3];
float Sz_CamDtr[2];
float BndBx_Cam[4];
float Sz_CamFOV[2];
float Dtctr_Cam[3];
float xvec_Cam[3];
float yvec_Cam[3];
float M_AT2W[4][4];

float Spc_Fill[3];
unsigned int Dim_Fill[3];
float Offset_Fill[3]={0};

