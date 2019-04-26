#ifndef __DT_TRIANGLE__
#define __DT_TRIANGLE__
#include <glm/glm.hpp>
#include <vector>

class Triangle
{
private:
  int index;
  glm::vec3 point1;
  glm::vec3 point2;
  glm::vec3 point3;
  glm::vec3 normal1;
  glm::vec3 normal2;
  glm::vec3 normal3;
  std::vector<int> globalVertexIndex;
  std::vector<int> globalNormalIndex;

public:
  Triangle(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, int index);
  Triangle(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 normal1, glm::vec3 normal2, glm::vec3 normal3, int triangleIndex, int globalVertexIndex1, int globalVertexIndex2, int globalVertexIndex3, int indexglobalNormal1, int indexglobalNormal2, int indexglobalNormal3);
  void setNormal(glm::vec3 normal);
  std::vector<glm::vec3> getPoints();
  std::vector<glm::vec3> getNormals();
  std::vector<int> getGlobalVertexIndices();
  int getGlobalVertexIndex(int localVertexIndex);
  std::vector<int> getGlobalNormalIndices();
  int getIndex();
  void setCoords(int localIndex, glm::vec3 coords);
};
#endif