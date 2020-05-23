#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <imgui.h>
#include <ImGuiFileBrowser.h>
#include <init.h>
#include <3dmath.h>
#include <ogl.h>
#include <string>
#include <texture.h>
#include <unordered_map>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

namespace DGE = DragonGameEngine;
namespace fs = std::filesystem;

fs::path exe;

std::string windowTitle = "OpenGL Demo";
int currentWidth = 600;
int currentHeight = 600;

DragonGameEngine::Math::Matrix ProjectionMatrix;

GLuint ProjectionMatrixUniformLocation;

GLuint ShaderIds[3] = { 0 };

bool showGeneratedImageWindow = false;

imgui_addons::ImGuiFileBrowser file_dialog;

// Loaded images
std::unordered_map<std::string, DGE::Texture::Texture> textures;

void exitCallback();
void initCallback();
void renderCallback();
void reshapeCallback(int width, int height);

int main(int argc, char** argv)
{
  exe = argv[0];
  return DGE::init(currentWidth,
    currentHeight,
    windowTitle,
    true,
    reshapeCallback,
    initCallback,
    renderCallback,
    exitCallback);
}

void exitCallback() {}

void initCallback()
{
  glGetError();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  DragonGameEngine::OGL::ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  DragonGameEngine::OGL::ExitOnGLError("ERROR: Could not set OpenGL culling options");

  reshapeCallback(currentWidth, currentHeight);
}

void drawMainMenu()
{
  if(ImGui::BeginMainMenuBar())
  {
    if(ImGui::BeginMenu("Image"))
    {
      ImGui::MenuItem("Display Generated Image", nullptr, &showGeneratedImageWindow);
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }
}

void generateImage()
{
  constexpr unsigned int width = 128;
  constexpr unsigned int height = 128;
  unsigned char *data = new unsigned char[width * height * 4];

  for (int i = 0; i < width * height; ++i)
  {
    data[(i * 4)] = 0;
    data[(i * 4) + 1] = 0;
    data[(i * 4) + 2] = 255;
    data[(i * 4) + 3] = 255;
  }

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  delete data;
  DGE::Texture::Texture t(width, height, tex);
  textures["generated"] = t;
}

void drawGeneratedImageWindow()
{
  bool open = false;
  if(!textures.count("generated")) generateImage();
  auto t = textures["generated"];
  if(ImGui::Begin("Image Window", &showGeneratedImageWindow, ImGuiWindowFlags_MenuBar))
  {
    if(ImGui::BeginMenuBar())
    {
      if(ImGui::BeginMenu("File"))
      {
        if(ImGui::MenuItem("Open")) open = true;
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    if(open) ImGui::OpenPopup("Open File");

    if(file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), "*.*"))
    {
        //std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
        //std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
    }

    ImGui::Image((void*)(intptr_t)t.textureId, ImVec2(static_cast<float>(t.width), static_cast<float>(t.height)));
    ImGui::End();
  }
}

void renderCallback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  drawMainMenu();
  if(showGeneratedImageWindow) drawGeneratedImageWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  ImGuiIO& io = ImGui::GetIO();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
  }
}

void reshapeCallback(int width, int height)
{
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, currentWidth, currentHeight);
  ProjectionMatrix = DragonGameEngine::Math::CreateProjectionMatrix(60,
    static_cast<float>(currentWidth) / currentHeight,
    1.0f,
    100.0f
  );

  glUseProgram(ShaderIds[0]);
  glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, ProjectionMatrix.m);
  glUseProgram(0);
}
