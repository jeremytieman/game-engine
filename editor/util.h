#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace editor
{
  bool loadFile(const std::string& absoluteFilePath, std::vector<char>& v)
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
    ifs.read(v.data(), size);
    return true;
  }
}