#ifndef __DT__adjacent__
#define __DT__adjacent__
#include "triangle.h"
#include "mesh.h"
#include <map>
#include <string>
typedef std::map<int, std::vector<int>> AdjMap;
class AdjacentMatrix
{
private:
  AdjMap map;
  int numAdj = 0;
  static void workerThreadBuildAdj(std::vector<Triangle> triangles, int startIndex, int stopIndex, int threadIndex, AdjacentMatrix *context);

public:
  AdjacentMatrix(Mesh h);
  AdjacentMatrix();
  std::vector<int> GetAdjacentTriangleIndicesForTriangleIndex(int triangelIndex);
  int getNumAdjacences();
  void save(std::string filename);
  void load(std::string filename);
};

#endif