#include "3dtree.h"
#include <glm/gtc/constants.hpp>
ThreeDTree::ThreeDTree(Mesh *m)
{
    target = m;
    cloud = pcl::PointCloud<pcl::PointXYZ>::Ptr(new pcl::PointCloud<pcl::PointXYZ>);
    kdtree = pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr(new pcl::KdTreeFLANN<pcl::PointXYZ>);
    std::vector<glm::vec3> vertices = m->getVertices();
    for (int i = 0; i < vertices.size(); i++)
    {
        glm::vec3 v = vertices.at(i);
        cloud->push_back(pcl::PointXYZ(v.x, v.y, v.z));
    }

    kdtree->setInputCloud(cloud);
}
int ThreeDTree::findNearestNeighbourIndex(glm::vec3 sourcePoint, glm::vec3 sourceNormal)
{
    pcl::PointXYZ searchPoint;
    searchPoint.x = sourcePoint.x;
    searchPoint.y = sourcePoint.y;
    searchPoint.z = sourcePoint.z;
    int K = 1;
    std::vector<int> pointIdxNKNSearch;
    std::vector<float> pointNKNSquaredDistance;
    bool foundValidPoint = false;
    std::vector<glm::vec3> normales = target->getNormals();
    int bestHitIndex = -1;
    float dist = std::numeric_limits<float>::max();
    while (!foundValidPoint)
    {
        if (kdtree->nearestKSearch(searchPoint, K, pointIdxNKNSearch, pointNKNSquaredDistance) > 0)
        {
            for (int i = 0; i < pointIdxNKNSearch.size(); i++)
            {
                int id = pointIdxNKNSearch.at(i);
                glm::vec3 targetNormal = normales.at(id);
                float angle = glm::acos((glm::dot(targetNormal, sourceNormal)) / (glm::length(targetNormal) * glm::length(sourceNormal)));
                if (angle < glm::half_pi<float>())
                {
                    foundValidPoint = true;
                    if (dist > pointNKNSquaredDistance.at(i))
                    {
                        dist = pointNKNSquaredDistance.at(i);
                        bestHitIndex = id;
                    }
                }
            }
        }
        K *= 2;
    }
    return bestHitIndex;
}
