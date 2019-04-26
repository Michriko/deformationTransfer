
#include "correspondences.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
using namespace std;

std::vector<Correspondence> readCorresFile(std::string filename)
{
    std::vector<Correspondence> corresList;
    ifstream filestream(filename);
    if (filestream.is_open())
    {
        string line;
        while (getline(filestream, line))
        {
            if (line.length() <= 0 || line.at(0) == '#')
            {
                continue;
            }
            vector<string> lineargs;
            using boost::is_any_of;
            boost::algorithm::split(lineargs, line, is_any_of("\t ;"), boost::token_compress_on);
            assert(lineargs.size() <= 2);
            int leftIndex = std::stoi(lineargs.at(0));
            int rightIndex = std::stoi(lineargs.at(1));
            Correspondence c = {leftIndex, rightIndex};
            corresList.push_back(c);
        }
    }
    else
    {
        std::cout << "cannot open corresFile" << std::endl;
    }
    return corresList;
}