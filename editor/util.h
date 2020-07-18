#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "util.h"

namespace editor::util
{
  namespace fs = std::filesystem;

  // Allows enum classes to be used as keys for unordered_map
  struct EnumClassHash
  {
      template <typename T>
      std::size_t operator()(T t) const
      {
          return static_cast<std::size_t>(t);
      }
  };

  bool loadFile(const std::string& absoluteFilePath, std::vector<unsigned char>& v)
  {
    fs::path p = absoluteFilePath;
    auto status = fs::status(p);
    if(!fs::exists(status)) return false;
    if(!fs::is_regular_file(status)) return false;
    if(fs::is_empty(p)) return false;
    auto size = fs::file_size(p);
    std::ifstream ifs(p, std::ios::binary);
    if(!ifs.is_open()) return false;
    v.clear();
    v.resize(size);
    ifs.read(reinterpret_cast<char*>(v.data()), size);
    return true;
  }
}
