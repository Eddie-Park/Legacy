

#ifndef __Headers_H__
#define __Headers_H__


struct SSM
{
	float *Verts_Mean;
	float *Verts;
	float *Eigenval;
	float *Eigenvect;
	float *Inv_COV;
	float Energy_Th;
	float Mean_Scale;
	unsigned int *Tri;
	unsigned int *Labs_Verts;
	unsigned int *Labs_Mesh;
	unsigned int *numVerts_Each;
	unsigned int *numTri_Each;
	unsigned int num_Verts;
	unsigned int num_Eigenval;  
	unsigned int num_Eigenvect[2];
	unsigned int num_Mesh;
	unsigned int num_VertsMean;
	unsigned int num_Tri;
} SSM, SSM_O, SSM_I;


struct CSM
{
	float *MeanCoef_Outer; 
	float *MeanCoef_Inner;
	float *invCov_II; 
    float *CovIO_invCovO;
	unsigned int numPC_Outer;
	unsigned int numPC_Inner;
	unsigned int num_indSkin; 
	unsigned int num_indSkel;
	unsigned int num_CoefOuter; 
	unsigned int num_CoefInner;
	unsigned int *ind_Skin;
	unsigned int *ind_Skel;
} CSM;


struct Mesh
{
	float *Verts;
	unsigned int *Tri;
	unsigned int *Labs_Mesh;
	unsigned int *Labs_Verts;
	unsigned int *numVerts_Each;
	unsigned int *numTri_Each;
	unsigned int num_Verts;
	unsigned int num_Mesh; 
	unsigned int num_Tri;
} M_Skin, M_All, buffer;

  
	
 



#endif


