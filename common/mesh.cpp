#include "mesh.h"
#include <iostream>
#include <fstream>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iomanip>
using namespace std;
Mesh::Mesh(string filename) : size(0)
{
  ifstream fileStream(filename);
  if (fileStream.is_open())
  {
    string line;
    while (getline(fileStream, line))
    {
      if (line.length() <= 0 || line.at(0) == '#')
      {
        continue;
      }
      vector<string> lineargs;
      using boost::is_any_of;
      boost::algorithm::split(lineargs, line, is_any_of("\t "), boost::token_compress_on);
      assert(lineargs.size() >= 2);
      string token = lineargs.at(0);
      if (token.compare("v") == 0)
      {
        //expecting exactly 3 coordinates
        assert(lineargs.size() >= 4);
        float x = std::stof(lineargs.at(1), 0);
        float y = std::stof(lineargs.at(2), 0);
        float z = std::stof(lineargs.at(3), 0);
        glm::vec3 v = glm::vec3(x, y, z);
        vertices.push_back(v);
        minX = std::min(x, minX);
        minY = std::min(y, minY);
        minZ = std::min(z, minZ);
        maxX = std::max(x, maxX);
        maxY = std::max(y, maxY);
        maxZ = std::max(z, maxZ);
      }
      else if (token.compare("vn") == 0)
      {
        //expecting exactly 3 coordinates
        assert(lineargs.size() >= 4);
        float x = std::stof(lineargs.at(1), 0);
        float y = std::stof(lineargs.at(2), 0);
        float z = std::stof(lineargs.at(3), 0);
        glm::vec3 n = glm::vec3(x, y, z);
        normals.push_back(n);
      }
      else if (token.compare("f") == 0)
      {
        //expacting 3 indexes in form of vertexindex//normalindex  vertexindex//normalindex vertexindex//normalindex
        assert(lineargs.size() >= 4);
        vector<string> indices;
        boost::algorithm::split(indices, lineargs.at(1), is_any_of("//"), boost::token_compress_on);
        assert(indices.size() == 2);
        int index_vertex_1 = stoi(indices.at(0));
        int index_normal_1 = stoi(indices.at(1));
        glm::vec3 v1 = vertices.at(index_vertex_1 - 1);
        glm::vec3 n1 = normals.at(index_normal_1 - 1);
        boost::algorithm::split(indices, lineargs.at(2), is_any_of("//"), boost::token_compress_on);
        assert(indices.size() == 2);
        int index_vertex_2 = stoi(indices.at(0));
        int index_normal_2 = stoi(indices.at(1));
        glm::vec3 v2 = vertices.at(index_vertex_2 - 1);
        glm::vec3 n2 = normals.at(index_normal_2 - 1);
        boost::algorithm::split(indices, lineargs.at(3), is_any_of("//"), boost::token_compress_on);
        assert(indices.size() == 2);
        int index_vertex_3 = stoi(indices.at(0));
        int index_normal_3 = stoi(indices.at(1));
        glm::vec3 v3 = vertices.at(index_vertex_3 - 1);
        glm::vec3 n3 = normals.at(index_normal_3 - 1);

        Triangle t(v1, v2, v3, n1, n2, n3, triangles.size(), index_vertex_1 - 1, index_vertex_2 - 1, index_vertex_3 - 1, index_normal_1 - 1, index_normal_2 - 1, index_normal_3 - 1);
        this->triangles.push_back(t);
      }
      else
      {
        cout << "unknown line:" << endl;
        cout << token << endl;
      }
    }
    fileStream.close();
  }

  else
    cout << "Unable to open file" << filename;
}
std::vector<glm::vec3> Mesh::getVertices()
{
  return this->vertices;
}
std::vector<Triangle> &Mesh::getTriangles(void)
{
  return this->triangles;
}
void Mesh::writeObjFile(string filename)
{
  ofstream filestream(filename);
  if (filestream.is_open())
  {
    //first write all vertices
    //v   x.xxxxxxx    y.yyyyyyy   z.zzzzzzz
    for (auto v : vertices)
    {
      string x, y, z;
      stringstream ssx, ssy, ssz;
      ssx << std::fixed << std::setprecision(8) << v.x;
      ssy << std::fixed << std::setprecision(8) << v.y;
      ssz << std::fixed << std::setprecision(8) << v.z;

      x = ssx.str();
      y = ssy.str();
      z = ssz.str();

      filestream << "v    " << x << "        " << y << "        " << z << endl;
    }

    //write all normals
    //vn   x.xxxxxxx    y.yyyyyyy   z.zzzzzzz
    for (auto vn : normals)
    {
      string x, y, z;
      stringstream ssx, ssy, ssz;
      ssx << std::fixed << std::setprecision(8) << vn.x;
      ssy << std::fixed << std::setprecision(8) << vn.y;
      ssz << std::fixed << std::setprecision(8) << vn.z;

      x = ssx.str();
      y = ssy.str();
      z = ssz.str();

      filestream << "vn    " << x << "        " << y << "        " << z << endl;
    }
    //write all Triangles
    //   f   v1//v1n    v2//v2n     v3//v3n
    for (auto t : triangles)
    {
      vector<int> indices = t.getGlobalVertexIndices();
      vector<int> normals = t.getGlobalNormalIndices();

      filestream << "f " << indices[0] + 1 << "//" << normals[0] + 1 << " " << indices[1] + 1 << "//" << normals[1] + 1 << " " << indices[2] + 1 << "//" << normals[2] + 1 << endl;
    }
    filestream.close();
  }
  else
  {
    cout << "cannot write file " << filename << endl;
  }
}

void Mesh::setVertex(int index, glm::vec3 coords)
{

  this->vertices.at(index).x = coords.x;
  this->vertices.at(index).y = coords.y;
  this->vertices.at(index).z = coords.z;
}
void Mesh::makeTrianglesConsistentWithVertices()
{
  for (Triangle t : this->triangles)
  {
    std::vector<int> vidx = t.getGlobalVertexIndices();
    for (int i = 0; i < vidx.size(); i++)
    {
      int vid = vidx.at(i);
      glm::vec3 newCoords = this->vertices.at(vid);
      t.setCoords(i, newCoords);
    }
  }
}
std::vector<glm::vec3> Mesh::getNormals()
{
  return this->normals;
}