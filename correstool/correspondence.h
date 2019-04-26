#ifndef __DT__CORRES__
#define __DT__CORRES__
#include <vector>
#include <glm/glm.hpp>
#include "mesh.h"

struct TriangleVertexIndex
{
  int vertex, triangle;
};
struct Correspondence
{
  TriangleVertexIndex left, right;
};

class CorrespondenceList
{
private:
  std::vector<TriangleVertexIndex> leftSelection;
  std::vector<TriangleVertexIndex> rightSelection;
  int findElementByTriangleIndex(std::vector<TriangleVertexIndex> items, int element);

public:
  CorrespondenceList();
  void upsertTriangleLeft(int triangleIndex);
  void upsertTriangleRight(int triangleIndex);
  Correspondence getCorrespondence(int position);
  int getNumCorrespondences();
  std::vector<TriangleVertexIndex> getLeftPoints();
  std::vector<TriangleVertexIndex> getRightPoints();
  void saveCorresFile(std::string filename, Mesh left, Mesh right);
};

#endif