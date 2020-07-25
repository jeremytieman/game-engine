#pragma once

#include <fstream>
#include <ostream>
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

  void prettyPrintMemoryBlock(const size_t x, const size_t y, const size_t dataSize, const std::vector<unsigned char>& data, std::ostream& out)
  {
    const auto rowSize = x * dataSize;

    for(size_t i = 0; i < y; ++i)
    {
      const auto yOffset = rowSize * i;

      for(size_t j = 0; j < x; ++j)
      {
        const auto xyOffset = yOffset + (j * dataSize);

        for(size_t k = 0; k < dataSize; ++k)
        {
          out << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(data[xyOffset + k]) << std::nouppercase << std::dec << " ";
        }
      }

      out << "\n";
    }
  }
}
