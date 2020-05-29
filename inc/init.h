#pragma once

#include <string>

namespace DragonGameEngine
{
    typedef void (* DGEinitfunc)(int width, int height);
    typedef void (* DGErenderfunc)();
    typedef void (* DGEexitfunc)();
    typedef void (* DGEreshapefunc)(int width, int height);

    int init(const int width,
      const int height,
      const std::string& windowTitle,
      const bool initDearImGui,
      const DGEreshapefunc reshapeFunc,
      const DGEinitfunc initFunc,
      const DGErenderfunc renderFunc,
      const DGEexitfunc exitFunc);

    int initMaximized(const std::string& windowTitle,
      const bool initDearImGui,
      const DGEreshapefunc reshapeFunc,
      const DGEinitfunc initFunc,
      const DGErenderfunc renderFunc,
      const DGEexitfunc exitFunc);

    void setWindowTitle(const std::string& newTitle);
}
