#ifndef __DT__TRIANGLEMAPPING__
#define __DT__TRIANGLEMAPPING__
#include "mesh.h"
#include "glm/glm.hpp"
#include "correspondences.h"
#include <vector>
#include <string>
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr buildTriangleSearchTree(Mesh *target);
int getTargetTriangleIndex(pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr tree, glm::vec3 searchPoint);
std::vector<Correspondence> createCorrespondenceList(Mesh *source, Mesh *target);
void saveCorresFile(std::vector<Correspondence> corresList, std::string filename);
#endif