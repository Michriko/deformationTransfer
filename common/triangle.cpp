#include "triangle.h"
#include <glm/glm.hpp>

Triangle::Triangle(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, int index) : point1(point1), point2(point2), point3(point3), index(index)
{
}
Triangle::Triangle(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 normal1, glm::vec3 normal2,
                   glm::vec3 normal3, int triangleIndex, int globalVertexIndex1, int globalVertexIndex2, int globalVertexIndex3, int globalNormalIndex1, int globalNormalIndex2, int globalNormalIndex3) : Triangle(point1, point2, point3, index)
{
  this->normal1 = normal1;
  this->normal2 = normal2;
  this->normal3 = normal3;
  this->index = triangleIndex;
  this->globalVertexIndex = {globalVertexIndex1, globalVertexIndex2, globalVertexIndex3};
  this->globalNormalIndex = {globalNormalIndex1, globalNormalIndex2, globalNormalIndex3};
}
std::vector<glm::vec3> Triangle::getPoints(void)
{
  std::vector<glm::vec3> retVal;
  retVal.push_back(this->point1);
  retVal.push_back(this->point2);
  retVal.push_back(this->point3);
  return retVal;
}
std::vector<glm::vec3> Triangle::getNormals(void)
{
  std::vector<glm::vec3> retVal;
  retVal.push_back(this->normal1);
  retVal.push_back(this->normal2);
  retVal.push_back(this->normal3);
  return retVal;
}
std::vector<int> Triangle::getGlobalVertexIndices()
{
  return this->globalVertexIndex;
}
int Triangle::getGlobalVertexIndex(int localVertexIndex)
{
  assert(localVertexIndex >= 0);
  assert(localVertexIndex < 3);
  return globalVertexIndex[localVertexIndex];
}
int Triangle::getIndex()
{
  return this->index;
}
std::vector<int> Triangle::getGlobalNormalIndices()
{
  return this->globalNormalIndex;
}
void Triangle::setCoords(int localIndex, glm::vec3 coords)
{

  switch (localIndex)
  {
  case 0:
    this->point1.x = coords.x;
    this->point1.y = coords.y;
    this->point1.z = coords.z;
    break;
  case 1:
    this->point2.x = coords.x;
    this->point2.y = coords.y;
    this->point2.z = coords.z;
    break;
  case 2:
    this->point3.x = coords.x;
    this->point3.y = coords.y;
    this->point3.z = coords.z;
    break;

  default:
    break;
  }
}
