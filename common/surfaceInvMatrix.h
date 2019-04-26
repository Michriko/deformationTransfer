#ifndef __DT__SURFACEINV__
#define __DT__SURFACEINV__
#include <glm/glm.hpp>
#include "triangle.h"
#include "mesh.h"
#include <vector>
glm::mat3x3 calculateSurfaceMatrix(Mesh *m, Triangle &t);
std::vector<glm::mat3x3> calcSurfaceInvList(Mesh *m);
#endif