#ifndef __dt__cholmodbuilder__
#define __dt__cholmodbuilder__
#include "mesh.h"
#include "adjacent.h"
#include "correspondences.h"
#include "cholmod.h"
#include <map>

extern cholmod_common common, *cm;
extern std::map<int, int> vertexVarMap;
cholmod_dense *solveLeastSquares(cholmod_sparse *A, cholmod_dense *c);

void build_linearProblem_Phase1(Mesh *source,
                                Mesh *target,
                                AdjacentMatrix *adj,
                                std::vector<Correspondence> &CorrespondenceList,
                                cholmod_triplet **M, cholmod_dense **C, //outputParams
                                double weight_smooth,
                                double weight_identity);
void build_linearProblem_Phase2(Mesh *source,
                                Mesh *target,
                                AdjacentMatrix *adj,
                                std::vector<Correspondence> &CorrespondenceList,
                                cholmod_triplet **M, cholmod_dense **C, //outputParams
                                double weight_smooth,
                                double weight_identity,
                                double weight_closest);
int getCorresIndex(std::vector<Correspondence> corresList, int vertexIndex);
int getVertexVarIndex(int vertexIndex, int dim, bool isPhantomVertex, int num_free);
#endif