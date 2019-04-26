#ifndef __DT__TRANSFORMER__
#define __DT__TRANSFORMER__
#include "mesh.h"
#include "correspondences.h"
#include <vector>
#include "cholmod.h"
class Transformer
{
private:
  Mesh *source, *target;
  std::vector<Correspondence> CorresList;
  std::vector<glm::mat3x3> sourceInvSurfaces;
  corresDict corresMap;
  cholmod_sparse *At, *AtA;
  cholmod_dense *C, *c, *x;
  void *umfpackNumericObj;
  void applyDeformation();

public:
  Transformer(Mesh *sourceRef, Mesh *targetRef, std::vector<Correspondence> CorresList);
  ~Transformer();
  void applyToTargetMesh(Mesh *sourceDeformed);
};

#endif