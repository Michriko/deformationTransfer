#include "adjacent.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>

AdjacentMatrix::AdjacentMatrix() {}

std::vector<int> AdjacentMatrix::GetAdjacentTriangleIndicesForTriangleIndex(int sourceIndex)
{
    return map[sourceIndex];
}
void AdjacentMatrix::save(std::string filename)
{
    std::ofstream filestream(filename);
    if (filestream.is_open())
    {
        for (auto const &[key, value] : map)
        {
            filestream << key;
            for (auto const adjEntry : value)
            {
                filestream << ";";
                filestream << adjEntry;
            }
            filestream << std::endl;
        }
        filestream.close();
    }
}
void AdjacentMatrix::load(std::string filename)
{
    std::ifstream filestream(filename);
    if (filestream.is_open())
    {
        std::string line;
        while (std::getline(filestream, line))
        {
            if (line.length() <= 0 || line.at(0) == '#')
            {
                continue;
            }
            std::vector<std::string> lineargs;
            using boost::is_any_of;
            boost::algorithm::split(lineargs, line, is_any_of("\t ;"), boost::token_compress_on);
            assert(lineargs.size() >= 1);

            //assert(lineargs.size() <= 4);
            int key = std::stoi(lineargs.at(0));
            std::vector<int> *value = new std::vector<int>();
            //take only first  three adjacent triangles
            for (int i = 1; i < std::min(4, (int)lineargs.size()); i++)
            {
                value->push_back(std::stoi(lineargs.at(i)));
                numAdj++;
            }
            map[key] = *value;
        }
    }
    else
    {
        std::cout << "cannot open file " << filename << std::endl;
    }
}
int AdjacentMatrix::getNumAdjacences()
{
    return numAdj;
}