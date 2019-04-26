#include "triangleMapping.h"
#include "triangle.h"
#include <fstream>

pcl::PointCloud<pcl::PointXYZ>::Ptr triangleCenterPoints;
static glm::vec3 findCentroid(std::vector<glm::vec3> &t);
pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr buildTriangleSearchTree(Mesh *target)
{
    triangleCenterPoints = pcl::PointCloud<pcl::PointXYZ>::Ptr(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr newTree = pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr(new pcl::KdTreeFLANN<pcl::PointXYZ>);
    std::vector<Triangle> triangles = target->getTriangles();
    std::vector<glm::vec3> vertices = target->getVertices();

    for (int i = 0; i < triangles.size(); i++)
    {

        glm::vec3 centerpoint;
        std::vector<int> vertexIndices = triangles.at(i).getGlobalVertexIndices();
        std::vector<glm::vec3> triangleVertices;
        for (int j : vertexIndices)
        {
            triangleVertices.push_back(vertices.at(j));
        }
        centerpoint = findCentroid(triangleVertices);
        triangleCenterPoints->push_back(pcl::PointXYZ(centerpoint.x, centerpoint.y, centerpoint.z));
    }
    newTree->setInputCloud(triangleCenterPoints);
    return newTree;
}
int getTargetTriangleIndex(pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr tree, glm::vec3 searchPoint)
{

    pcl::PointXYZ pclSearchPoint;
    pclSearchPoint.x = searchPoint.x;
    pclSearchPoint.y = searchPoint.y;
    pclSearchPoint.z = searchPoint.z;
    int K = 1;
    std::vector<int> pointIdx;
    std::vector<float> pointDist;
    if (tree->nearestKSearch(pclSearchPoint, K, pointIdx, pointDist) > 0)
    {
        return pointIdx.at(0);
    }
    return -1;
}

glm::vec3 findCentroid(std::vector<glm::vec3> &t)
{
    //center of a triangle is 1/3 * (u+v+w)
    return (float)(1.0 / 3.0) * (t.at(0) + t.at(1) + t.at(2));
}

std::vector<Correspondence> createCorrespondenceList(Mesh *source, Mesh *target)
{
    std::vector<Correspondence> corresList;
    pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr tree = buildTriangleSearchTree(target);
    std::vector<glm::vec3> sourceVertices = source->getVertices();
    for (Triangle t : source->getTriangles())
    {
        Correspondence c;
        c.leftVertexIndex = t.getIndex();
        std::vector<int> vertexIdx = t.getGlobalVertexIndices();
        std::vector<glm::vec3> triangleVertices;
        for (int i : vertexIdx)
        {
            triangleVertices.push_back(sourceVertices.at(i));
        }
        glm::vec3 sourceCentroid = findCentroid(triangleVertices);
        int targetIndex = getTargetTriangleIndex(tree, sourceCentroid);
        c.rightVertexIndex = targetIndex;
        corresList.push_back(c);
    }
    return corresList;
}
void saveCorresFile(std::vector<Correspondence> corresList, std::string filename)
{
    std::ofstream filestream(filename);
    if (filestream.is_open())
    {
        filestream << "#left model, right model" << std::endl;
        for (auto c : corresList)
        {
            filestream << c.leftVertexIndex << ";" << c.rightVertexIndex << std::endl;
        }
        filestream.close();
    }
    else
    {
        std::cout << "cannot open filestream" << std::endl;
    }
}
