#include <init.h>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace DragonGameEngine
{
  GLFWwindow* window = nullptr;
  DGEreshapefunc reshapeFunc = nullptr;

  void errorCallback(int error, const char* description)
  {
    std::cerr << "Error " << error << ": " << description << "\n";
  }

  void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
  {
    if (reshapeFunc) reshapeFunc(width, height);
  }

  int internalInit(const int width,
    const int height,
    const bool fullscreen,
    const std::string& windowTitle,
    const bool initDearImGui,
    const DGEreshapefunc reshapeFunc,
    const DGEinitfunc initFunc,
    const DGErenderfunc renderFunc,
    const DGEexitfunc exitFunc)
  {
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);

    if (window == nullptr)
    {
      glfwTerminate();
      return -1;
    }

    glfwMakeContextCurrent(window);

    DragonGameEngine::reshapeFunc = reshapeFunc;
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cerr << "Failed to initialize GLAD\n";
      return -1;
    }

    if (initDearImGui)
    {
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

      ImGuiStyle& style = ImGui::GetStyle();
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
          style.WindowRounding = 0.0f;
          style.Colors[ImGuiCol_WindowBg].w = 1.0f;
      }

      if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
      {
        std::cerr << "Failed to initialize ImGui GLFW for OpenGL\n";
        return -1;
      }

      const char *glsl_version = "#version 400";
      
      if (!ImGui_ImplOpenGL3_Init(glsl_version))
      {
        std::cerr << "Failed to initialize ImGui OpenGL 3\n";
        return -1;
      }

      ImGui::StyleColorsDark();
    }

    initFunc(width, height);
    if(fullscreen) glfwMaximizeWindow(window);

    while (!glfwWindowShouldClose(window))
    {
      renderFunc();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    exitFunc();

    if (initDearImGui)
    {
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext();
    }

    glfwTerminate();
    return 0;
  }

  int init(const int width,
    const int height,
    const std::string& windowTitle,
    const bool initDearImGui,
    const DGEreshapefunc reshapeFunc,
    const DGEinitfunc initFunc,
    const DGErenderfunc renderFunc,
    const DGEexitfunc exitFunc)
  {
    return internalInit(width,
      height,
      false,
      windowTitle,
      true,
      reshapeFunc,
      initFunc,
      renderFunc,
      exitFunc);
  }

  int initMaximized(const std::string& windowTitle,
    const bool initDearImGui,
    const DGEreshapefunc reshapeFunc,
    const DGEinitfunc initFunc,
    const DGErenderfunc renderFunc,
    const DGEexitfunc exitFunc)
  {
    return internalInit(640,
      480,
      true,
      windowTitle,
      true,
      reshapeFunc,
      initFunc,
      renderFunc,
      exitFunc);
  }

  void setWindowTitle(const std::string& newTitle)
  {
    glfwSetWindowTitle(window, newTitle.c_str());
  }
}
