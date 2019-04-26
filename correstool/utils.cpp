#include "utils.h"
#include <boost/algorithm/string.hpp>

std::vector<std::string> *splitString(std::string s, std::string delimiter)
{
  std::vector<std::string> *retVal = new std::vector<std::string>;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    if (token.compare(delimiter) != 0 && token.length() > 0)
    {
      retVal->push_back(token);
    }

    s.erase(0, pos + delimiter.length());
  }
  return retVal;
}