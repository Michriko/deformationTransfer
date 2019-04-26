#ifndef __DT_MESH__
#define __DT_MESH__
#include <string>
#include "triangle.h"
#include <vector>

class Mesh
{
private:
  int size;
  std::vector<Triangle> triangles;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  void parseLine(
      std::vector<glm::vec3> &vertices,
      std::vector<glm::vec3> &normals);

public:
  Mesh(std::string filename);
  void printContent(void);
  void saveFile(std::string fileName);
  std::vector<Triangle> &getTriangles();
  std::vector<glm::vec3> getVertices();
  std::vector<glm::vec3> getNormals();
  void writeObjFile(std::string filename);
  float minX, minY, minZ = std::numeric_limits<float>::max();
  float maxX, maxY, maxZ = std::numeric_limits<float>::min();
  void setVertex(int index, glm::vec3 vertex);
  void makeTrianglesConsistentWithVertices();
};
#endif
