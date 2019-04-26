#ifndef __DT__CHOLMOD_BUILDER__
#define __DT__CHOLMOD_BUILDER__
#include "mesh.h"
#include "correspondences.h"
#include "cholmod.h"

//defines sind wie im github repo (wäre aber auch nicht großartig anders möglich)
#define cholmod_modifyvec(vec, i, val) \
    ((vec->dtype == 0) ? (((double *)((vec)->x))[i] = (double)(val)) : (((float *)((vec)->x))[i] = (float)(val)))
#define cholmod_getVarAt(vec, i) \
    ((vec->dtype == 0) ? ((double *)(vec->x))[i] : ((float *)(vec->x))[i])

//ab hier wieder eigener code
extern cholmod_common common, *cm;
void startCholmod();
void buildCoefficientMatrix(Mesh *target, corresDict corresMap, cholmod_triplet *A);
void allocateMemory(Mesh *target, corresDict corresmap, cholmod_triplet **A, cholmod_dense **C);
void buildRhsVector(Mesh *sourceDeformed, Mesh *targetRef, std::vector<glm::mat3x3> &invertedSurfaceMatrix_ref, corresDict corresmap, cholmod_dense *C);
#endif