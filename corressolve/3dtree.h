#ifndef __DT__TREE__
#define __DT__TREE__
#include "mesh.h"
#include "glm/glm.hpp"
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

class ThreeDTree
{
private:
  pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr kdtree;
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;
  Mesh *target;

public:
  ThreeDTree(Mesh *m);
  int findNearestNeighbourIndex(glm::vec3 sourcePoint, glm::vec3 sourceNormal);
};

#endif