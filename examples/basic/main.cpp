#include <filesystem>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <3dmath.h>
#include <init.h>
#include <ogl.h>
#include <util.h>
#include <iostream>
#include <string>
#include <sstream>

namespace DGE = DragonGameEngine;
namespace fs = std::filesystem;

unsigned long frameCount = 0UL;
GLuint ProjectionMatrixUniformLocation;
GLuint ViewMatrixUniformLocation;
GLuint ModelMatrixUniformLocation;
GLuint BufferIds[3] = { 0 };
GLuint ShaderIds[3] = { 0 };
DragonGameEngine::Math::Matrix ProjectionMatrix;
DragonGameEngine::Math::Matrix ViewMatrix;
DragonGameEngine::Math::Matrix ModelMatrix;
float CubeRotation = 0;
clock_t LastTime = 0;
double startTime = 0.0;
std::string windowTitle = "OpenGL Demo";
int currentWidth = 600;
int currentHeight = 600;
fs::path exe;

bool showRenderWindow = false;

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

void CreateCube()
{
  const DGE::Math::Vertex VERTICES[8] =
  {
    { { -.5f, -.5f,  .5f, 1 }, { 0, 0, 1, 1 } },
    { { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },
    { {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
    { {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
    { { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
    { { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
    { {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
    { {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }
  };

  const GLuint INDICES[36] =
  {
    0, 2, 1,
    0, 3, 2,
    4, 3, 0,
    4, 7, 3,
    4, 1, 5,
    4, 0, 1,
    3, 6, 2,
    3, 7, 6,
    1, 6, 5,
    1, 2, 6,
    7, 5, 6,
    7, 4, 5
  };

  ShaderIds[0] = glCreateProgram();
  DGE::OGL::ExitOnGLError("ERROR: Could not create the shader program");
  
  const auto FragmentShaderPath = exe.parent_path() / "shaders/SimpleShader.fragment.glsl";
  const auto VertexShaderPath = exe.parent_path() / "shaders/SimpleShader.vertex.glsl";
  ShaderIds[1] = DGE::OGL::LoadShader(FragmentShaderPath.string(), GL_FRAGMENT_SHADER);
  ShaderIds[2] = DGE::OGL::LoadShader(VertexShaderPath.string(), GL_VERTEX_SHADER);
  glAttachShader(ShaderIds[0], ShaderIds[1]);
  glAttachShader(ShaderIds[0], ShaderIds[2]);

  glLinkProgram(ShaderIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not link the shader program");

  ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
  ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
  ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
  DGE::OGL::ExitOnGLError("ERROR: Could not get the shader uniform locations");

  glGenVertexArrays(1, &BufferIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not generate the VAO");
  glBindVertexArray(BufferIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not bind the VAO");

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  DGE::OGL::ExitOnGLError("ERROR: Could not enable vertex attributes");

  glGenBuffers(2, &BufferIds[1]);
  DGE::OGL::ExitOnGLError("ERROR: Could not generate the buffer objects");

  glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
  DGE::OGL::ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
  DGE::OGL::ExitOnGLError("ERROR: Could not set VAO attributes");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
  DGE::OGL::ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

  glBindVertexArray(0);
}

void initCallback(int width, int height)
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

  ModelMatrix = DragonGameEngine::Math::IDENTITY_MATRIX;
  ProjectionMatrix = DragonGameEngine::Math::IDENTITY_MATRIX;
  ViewMatrix = DragonGameEngine::Math::IDENTITY_MATRIX;
  TranslateMatrix(ViewMatrix, 0, 0, -2);

  CreateCube();
  reshapeCallback(width, height);
}

bool createMainMenu()
{
  if(!ImGui::BeginMainMenuBar()) return false;
  if(!ImGui::BeginMenu("File")) return false;
  ImGui::MenuItem("Render Window", nullptr, &showRenderWindow);
  ImGui::MenuItem("Memory Editor", nullptr, true);
  ImGui::EndMenu();
  ImGui::EndMenuBar();
  return true;
}

bool createRenderWindow()
{
  if(!showRenderWindow) return false;
  if(!ImGui::Begin("Render Window", &showRenderWindow)) return false;
  ImGui::Text("Hello, World!");
  ImGui::End();
  return true;
}

void DrawCube()
{
  float CubeAngle;
  clock_t Now = clock();
  if (LastTime == 0) LastTime = Now;

  CubeRotation += 45.0f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
  CubeAngle = DGE::Math::DegreesToRadians(CubeRotation);
  LastTime = Now;

  ModelMatrix = DGE::Math::IDENTITY_MATRIX;
  DGE::Math::RotateAboutY(ModelMatrix, CubeAngle);
  DGE::Math::RotateAboutX(ModelMatrix, CubeAngle);

  glUseProgram(ShaderIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not use the shader program");

  glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
  glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, ViewMatrix.m);
  DGE::OGL::ExitOnGLError("ERROR: Could not set the shader uniforms");

  glBindVertexArray(BufferIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
  DGE::OGL::ExitOnGLError("ERROR: Could not draw the cube");

  glBindVertexArray(0);
  glUseProgram(0);
}

void framesPerSecondCalculator()
{
  if (0.0 == startTime)
  {
    startTime = glfwGetTime();
    return;
  }

  auto curTime = glfwGetTime();
  if ((curTime - startTime) < 0.25) return;
  std::stringstream newTitle(windowTitle);
  newTitle << ": " << frameCount * 4 << " Frames Per Second @ " << currentWidth << " x " << currentHeight;
  DGE::setWindowTitle(newTitle.str());
  frameCount = 0;
  startTime = curTime;
}

void mainCallback()
{
  ++frameCount;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  createMainMenu();
  createRenderWindow();
  DrawCube();

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

  framesPerSecondCalculator();
}

void DestroyCube()
{
  glDetachShader(ShaderIds[0], ShaderIds[1]);
  glDetachShader(ShaderIds[0], ShaderIds[2]);
  glDeleteShader(ShaderIds[1]);
  glDeleteShader(ShaderIds[2]);
  glDeleteProgram(ShaderIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not destroy the shaders");

  glDeleteBuffers(2, &BufferIds[1]);
  glDeleteVertexArrays(1, &BufferIds[0]);
  DGE::OGL::ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void exitCallback()
{
  DestroyCube();
}

int main(int argc, char** argv)
{
  exe = argv[0];
  return DGE::init(currentWidth,
    currentHeight,
    windowTitle,
    true,
    reshapeCallback,
    initCallback,
    mainCallback,
    exitCallback);
}
