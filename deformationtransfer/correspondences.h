#ifndef __DT__CORRES__
#define __DT__CORRES__
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <map>
typedef std::map<int, int> corresDict;
struct Correspondence
{
    int leftVertexIndex, rightVertexIndex;
};
corresDict CorresListToDict(std::vector<Correspondence> &corresList);
std::vector<Correspondence> readCorresFile(std::string filename);
#endif