#ifndef __DT__CORRES__
#define __DT__CORRES__
#include <vector>
#include <glm/glm.hpp>
#include <string>
struct Correspondence
{
    int leftVertexIndex, rightVertexIndex;
};
std::vector<Correspondence> readCorresFile(std::string filename);
bool isFreeVertex(int vertexIndex);
#endif