#ifndef Utility_h
#define Utility_h

#include <string>
#include <sstream>
#include <vector>

//this file contains functions that are not specific to each subsystem
//i.e. string parsing functions and other utility based methods

namespace Util {
  std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }

  std::vector<std::string> split(const std::string &s, char delim) {
      std::vector<std::string> elems;
      split(s, delim, elems);
      return elems;
  }
}

#endif
