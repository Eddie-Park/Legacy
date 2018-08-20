


#ifndef atlasRegistration_H
#define atlasRegistration_H

#define MAX_IMAGE_LENGTH      2048000
#define MAX_FILE_NAME_LENGTH 262


int Extract_SubMesh(struct SSM *Input_Mesh, unsigned int *Labs_Extract, unsigned int num_Extract, struct Mesh *Output_Mesh);
void Fill_Mesh (struct Mesh *Mesh, unsigned int Label, unsigned int Dim[3], float step, unsigned char *Volume);  
void Fill_Mesh_Only(unsigned int Dim[3],  float *Verts,  unsigned int *Tri,  unsigned int num_Tri, float Step,  unsigned char *VOLUME); 
unsigned char * Fill_MultiMesh(struct Mesh *Mesh, unsigned int *Labs_Fill, unsigned int Dim_Vlm[3], unsigned int num_Fill);
void Simple_Transform_Mesh(float *Verts, float *Center, float *Shifts, float Scale, float *Verts1, unsigned int n_Verts) ;
void Combine_MultiMesh(struct SSM *Mesh0, struct SSM *Mesh1, struct Mesh *Mesh_All);
void free_Mesh(struct Mesh* Mesh);
void free_SSM(struct SSM* SSM);
void free_CSM(struct CSM* CSM);

void ISODATA_Thresholding(float *Img_In, int Dim[2],int n,float *Img_Lab,float *Th);
void Region_Growing_3D(unsigned char *Vlm_In, unsigned char *Vlm_Out, unsigned int Dim[3],unsigned int seed[3], float Value_min, float Value_max, unsigned int nNeib); 
float Interpolation ( float* imgBuf, int lWidth, int lHeight, float x, float y);
float * Resample_Img(float *Image_In, float Spc0[2],unsigned int Dim0[2], float Spc1[2],unsigned int Dim1[2]);
void close_3x3cube(unsigned char *BW, unsigned int Dim[3]);  

void Compose_RotMat(float angles_In[3],float RotMat[4][4]);
void identityMatrix_float(float *matrix_In,int size);
void arrayMult(float *array1,float *array2, float *array_Out,unsigned int m, unsigned int n,unsigned int t);
void arrayTrans(float *array_In, float *array_Out, unsigned int m, unsigned int n);
void Matrix_Multiply(float *M1, unsigned int m1, unsigned int n1, float *M2, unsigned int m2, float *M3);

unsigned int *sort(unsigned int*t, unsigned int num);
unsigned int round(float a);

/* image correction related prototypes*/
float * pinhole_Xray ( unsigned short *imgBuf, float *CastRange );
float * pinhole_SidePhoto ( unsigned char *imgBuf );
unsigned short Interpolation_Short ( unsigned short* imgBuf, unsigned int lWidth, unsigned int lHeight, float x, float y);
unsigned char Interpolation_Char ( unsigned char* imgBuf, unsigned int lWidth, unsigned int lHeight, float x, float y);
float Interpolation_Float ( float* imgBuf, unsigned int lWidth, unsigned int lHeight, float x, float y);
void rebin2D_nondivisible(float *Img0, unsigned int Dim0[2], float *Img1, unsigned int Dim1[2]);
void pinhole_Xray_Para_Initial(void);
void pinhole_Cam_Para_Initial(void);




#endif 
