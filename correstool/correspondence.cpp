#include "correspondence.h"
#include "GL/freeglut.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <triangle.h>
using namespace std;

void logCorresList(std::vector<TriangleVertexIndex> selection)
{
  for (auto const t : selection)
  {
    std::cout << t.vertex << "_" << t.triangle << std::endl;
  }
}
CorrespondenceList::CorrespondenceList()
{
}
void CorrespondenceList::upsertTriangleLeft(int triangleIndex)
{
  TriangleVertexIndex tvi;
  int corresIndex = findElementByTriangleIndex(leftSelection, triangleIndex);
  if (corresIndex < 0)
  {
    tvi.triangle = triangleIndex;
    tvi.vertex = 0;
    leftSelection.push_back(tvi);
  }
  else
  {
    tvi = leftSelection.at(corresIndex);
    tvi.vertex = (++tvi.vertex) % 3;
    leftSelection.at(corresIndex).vertex = tvi.vertex;
  }
}
void CorrespondenceList::upsertTriangleRight(int triangleIndex)
{
  TriangleVertexIndex tvi;
  int corresIndex = findElementByTriangleIndex(rightSelection, triangleIndex);
  if (corresIndex < 0)
  {
    tvi.triangle = triangleIndex;
    tvi.vertex = 0;
    rightSelection.push_back(tvi);
  }
  else
  {
    tvi = rightSelection.at(corresIndex);
    tvi.vertex = (++tvi.vertex) % 3;
    rightSelection.at(corresIndex).vertex = tvi.vertex;
  }
}
int CorrespondenceList::getNumCorrespondences()
{
  return min(leftSelection.size(), rightSelection.size());
}
std::vector<TriangleVertexIndex> CorrespondenceList::getLeftPoints()
{
  return leftSelection;
}
std::vector<TriangleVertexIndex> CorrespondenceList::getRightPoints()
{
  return rightSelection;
}
void CorrespondenceList::saveCorresFile(std::string filename, Mesh left, Mesh right)
{
  std::vector<Triangle> leftTriangles = left.getTriangles();
  std::vector<Triangle> rightTrignales = right.getTriangles();
  ofstream filestream(filename);
  if (filestream.is_open())
  {
    filestream << "#left model, right model" << endl;
    for (int i = 0; i < this->getNumCorrespondences(); i++)
    {
      Triangle left = leftTriangles.at(leftSelection.at(i).triangle);
      Triangle right = rightTrignales.at(rightSelection.at(i).triangle);

      int leftVertexIndex = left.getGlobalVertexIndex(leftSelection.at(i).vertex);
      int rightVertexIndex = right.getGlobalVertexIndex(rightSelection.at(i).vertex);
      filestream << leftVertexIndex << ";" << rightVertexIndex << endl;
    }
    filestream.close();
  }
  else
  {
    cout << "cannot open filestream" << endl;
  }
}

int CorrespondenceList::findElementByTriangleIndex(std::vector<TriangleVertexIndex> items, int triangleIndex)
{
  for (int i = 0; i < items.size(); i++)
  {
    if (items.at(i).triangle == triangleIndex)
    {
      return i;
    }
  }
  return -1;
}