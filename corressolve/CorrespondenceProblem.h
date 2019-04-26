#ifndef __DT_CORRESPROBLEM__
#define __DT_CORRESPROBLEM__
#include "mesh.h"
#include "correspondences.h"
#include "adjacent.h"
#include "cholmod_builder.h"
#include "cholmod.h"

class CorrespondenceProblem
{
private:
  Mesh *source, *target;
  std::vector<Correspondence> userMapping;
  AdjacentMatrix *adj;
  void start_cholmod();
  void solvePhase1();
  void solvePhase2();
  void solveFinalize();
  void resolveTriangleCorrespondences();
  void applyDeformation(cholmod_dense *x);

public:
  CorrespondenceProblem(Mesh *source, Mesh *target, std::vector<Correspondence> userMapping, AdjacentMatrix *adj);
  void Solve();
  float weight_smooth = 1.00;
  float weight_identity = 0.001;
  float weight_closest_start = 1.0;
  float weight_closest_step = 10.0;
  float weight_closest_end = 10000;
};

#endif