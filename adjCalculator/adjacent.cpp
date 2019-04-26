#include "adjacent.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>
int numberOfSameVertices(Triangle t1, Triangle t2)
{
    std::vector<int> v1 = t1.getGlobalVertexIndices();
    std::vector<int> v2 = t2.getGlobalVertexIndices();
    int hits = 0;
    for (int i : v1)
    {
        if (std::find(v2.begin(), v2.end(), i) != v2.end())
        {
            hits++;
        }
    }
    return hits;
}
/*This step can be improved by building an octtree for finding nearby triangles. At this time adjacentdetection time complexity is  O(n^2) 
*/
AdjacentMatrix::AdjacentMatrix(Mesh h)
{
    std::vector<Triangle> triangles = h.getTriangles();
    int numTriangles = triangles.size();
    unsigned int numWorkerThreads = std::max(std::thread::hardware_concurrency(), (unsigned int)6);
    std::cout << "Use " << numWorkerThreads << " Threads for adjacent detection" << std::endl;
    int BatchPerThread = numTriangles / numWorkerThreads;

    std::vector<std::thread> workerThreads;
    for (int i = 0; i < numTriangles; i += BatchPerThread)
    {
        int startIndex = i;
        int stopIndex = std::min(i + BatchPerThread, numTriangles);
        workerThreads.push_back(std::thread(workerThreadBuildAdj, triangles, startIndex, stopIndex, i, this));
    }
    for (std::thread &t : workerThreads)
    {
        t.join();
    }
    std::cout << "finished" << std::endl;
    std::cout << "total adj size = " << map.size() << std::endl;
}

void AdjacentMatrix::workerThreadBuildAdj(std::vector<Triangle> triangles, int startIndex, int stopindex, int threadIndex, AdjacentMatrix *context)
{
    for (int i = startIndex; i < stopindex; i++)
    {
        Triangle t = triangles.at(i);
        std::vector<int> *adjacentTriangles = new std::vector<int>();
        for (Triangle s : triangles)
        {
            if (s.getIndex() != t.getIndex() && numberOfSameVertices(s, t) >= 2)
            {
                adjacentTriangles->push_back(s.getIndex());
            }
        }
        context->map[t.getIndex()] = *adjacentTriangles;
        if ((i - startIndex) % 100 == 0)
        {
            std::cout << "processed " << i - startIndex << "of" << stopindex - startIndex << "Triangles    Thread:" << threadIndex / (stopindex - startIndex) << std::endl;
        }
    }
}
