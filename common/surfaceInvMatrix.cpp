#include "surfaceInvMatrix.h"
#include "math.h"
glm::mat3x3 calculateSurfaceMatrix(Mesh *m, Triangle &t)
{
    glm::vec3 v2_1, v3_1, v4; //v2-1, v3-1, v4
    std::vector<int> vIdx = t.getGlobalVertexIndices();
    std::vector<glm::vec3> originalVertices = m->getVertices();
    std::vector<glm::vec3> currentVertices;
    for (int idx : vIdx)
    {
        currentVertices.push_back(originalVertices.at(idx));
    }
    v2_1 = currentVertices.at(1) - currentVertices.at(0);
    v3_1 = currentVertices.at(2) - currentVertices.at(0);
    //calculate crossproduct
    //v4.x = (v2_1.y * v3_1.z) - (v2_1.z * v3_1.y);
    //v4.y = (v2_1.z * v3_1.x) - (v2_1.x * v3_1.z);
    //v4.z = (v2_1.x * v3_1.y) - (v2_1.y * v3_1.x);
    v4 = glm::cross(v2_1, v3_1);
    float normTerm = 1.0 / sqrt(glm::length(v4));
    v4 = v4 * normTerm;

    glm::mat3x3 surfaceInvMatrix;
    surfaceInvMatrix[0] = v2_1;
    surfaceInvMatrix[1] = v3_1;
    surfaceInvMatrix[2] = v4;
    return surfaceInvMatrix;
}

std::vector<glm::mat3x3> calcSurfaceInvList(Mesh *m)
{
    std::vector<glm::mat3x3> InvList;
    std::vector<Triangle> triangles = m->getTriangles();
    for (Triangle t : triangles)
    {
        glm::mat3x3 surfaceMatrix = calculateSurfaceMatrix(m, t);
        InvList.push_back(glm::inverse(surfaceMatrix));
    }
    return InvList;
}