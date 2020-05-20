#include <util.h>

#include <fstream>
#include <string>
#include <cerrno>

namespace DragonGameEngine::Util
{
    std::string getFileContents(const std::string& fileName)
    {
        std::ifstream in(fileName, std::ios::in | std::ios::binary);
        if (!in) throw (errno);
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
}
