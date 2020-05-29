#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <init.h>
#include <3dmath.h>
#include <ogl.h>
#include <string>

#include "gui.h"

namespace DGE = DragonGameEngine;
namespace fs = std::filesystem;

fs::path exe;

std::string windowTitle = "OpenGL Demo";
int currentWidth = 600;
int currentHeight = 600;

DragonGameEngine::Math::Matrix ProjectionMatrix;

GLuint ProjectionMatrixUniformLocation;

GLuint ShaderIds[3] = { 0 };

void exitCallback();
void initCallback(int width, int height);
void renderCallback();
void reshapeCallback(int width, int height);

int main(int argc, char** argv)
{
  exe = argv[0];
  return DGE::initMaximized(windowTitle,
    true,
    reshapeCallback,
    initCallback,
    renderCallback,
    exitCallback);
}

void exitCallback() {}

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

  reshapeCallback(width, height);
}

void renderCallback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  editor::drawGui();
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
