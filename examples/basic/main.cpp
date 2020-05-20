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
//int width = 1280;
//int height = 960;
int currentWidth = 600;
int currentHeight = 600;
//GLuint VertexShaderId;
//GLuint FragmentShaderId;
//GLuint ProgramId;
//GLuint VaoId;
//GLuint VboId;
//GLuint ColorBufferId;
//GLuint BufferId;
//GLuint IndexBufferId;
fs::path exe;

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

/*
void CreateShaders()
{
    GLenum ErrorCheckValue = glGetError();

    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const auto VertexShaderPath = exe.parent_path() / "shaders/vertex.glsl";
    auto VertexShader = DragonGameEngine::Util::getFileContents(VertexShaderPath.string());
    auto VertextShaderPtr = VertexShader.c_str();
    glShaderSource(VertexShaderId, 1, &VertextShaderPtr, NULL);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    auto FragmentShaderPath = exe.parent_path() / "shaders/fragment.glsl";
    auto FragmentShader = DragonGameEngine::Util::getFileContents(FragmentShaderPath.string());
    auto FragmentShaderPtr = FragmentShader.c_str();
    glShaderSource(FragmentShaderId, 1, &FragmentShaderPtr, NULL);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);
    glUseProgram(ProgramId);

    ErrorCheckValue = glGetError();

    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cerr << "ERROR: Could not create the shaders: " << ErrorCheckValue << " \n";
        exit(-1);
    }
}
*/

/*
void CreateVBO()
{
    /*
    GLfloat Vertices[] = {
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.0f,  0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };

    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    *

   /*
    GLfloat Vertices[] = {
        -0.8f,  0.8f, 0.0f, 1.0f,
         0.8f,  0.8f, 0.0f, 1.0f,
        -0.8f, -0.8f, 0.0f, 1.0f,

        -0.8f, -0.8f, 0.0f, 1.0f,
         0.8f,  0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };

    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    *

    GLfloat Vertices[] = {
        -0.8f,  0.8f, 0.0f, 1.0f,
         0.8f,  0.8f, 0.0f, 1.0f,
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };

    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    GLenum ErrorCheckValue = glGetError();

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    ErrorCheckValue = glGetError();

    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cerr << "ERROR: Could not create a VBO: " << ErrorCheckValue << " \n";
        exit(-1);
    }
}
*/

/*
void CreateVBO()
{
    Vertex Vertices[] =
    {
        { { -0.8f, -0.8f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { {  0.0f,  0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { {  0.8f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };


    GLenum ErrorCheckValue = glGetError();
    const size_t BufferSize = sizeof(Vertices);
    const size_t VertexSize = sizeof(Vertices[0]);
    const size_t RgbOffset = sizeof(Vertices[0].XYZW);

    glGenBuffers(1, &VboId);

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    ErrorCheckValue = glGetError();

    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cerr << "ERROR: Could not create a VBO: " << ErrorCheckValue << " \n";
        exit(-1);
    }
}
*/

/*
void CreateVBO()
{
    DGE::Math::Vertex Vertices[] =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        // Top
        { { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Bottom
        { { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Left
        { { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Right
        { { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
    };
    
    GLubyte Indices[] =
    {
        // Top
        0, 1, 3,
        0, 3, 2,
        3, 1, 4,
        3, 4, 2,
        // Bottom
        0, 5, 7,
        0, 7, 6,
        7, 5, 8,
        7, 8, 6,
        // Left
        0, 9, 11,
        0, 11, 10,
        11, 9, 12,
        11, 12, 10,
        // Right
        0, 13, 15,
        0, 15, 14,
        15, 13, 16,
        15, 16, 14
    };

    GLenum ErrorCheckValue = glGetError();
    const size_t BufferSize = sizeof(Vertices);
    const size_t VertexSize = sizeof(Vertices[0]);
    const size_t RgbOffset = sizeof(Vertices[0].Position);

    glGenBuffers(1, &BufferId);

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, BufferId);
    glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}
*/

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

void initCallback()
{
    //CreateShaders();
    //CreateVBO();
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
    reshapeCallback(currentWidth, currentHeight);
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

    DrawCube();
    ImGui::Begin("Demo Window");
    ImGui::Button("Hello");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid *)0);
    framesPerSecondCalculator();
}

void DestroyCube(void)
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

/*
void DestroyShaders()
{
    GLenum ErrorCheckValue = glGetError();

    glUseProgram(0);

    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);

    glDeleteProgram(ProgramId);

    ErrorCheckValue = glGetError();

    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cerr << "ERROR: Could not destroy the shaders: " << ErrorCheckValue << " \n";
        exit(-1);
    }
}
*/

/*
void DestroyVBO()
{
    GLenum ErrorCheckValue = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &BufferId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &IndexBufferId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);

    ErrorCheckValue = glGetError();

    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cerr << "ERROR: Could not destroy the VBO: " << ErrorCheckValue << " \n";
        exit(-1);
    }
}
*/

void exitCallback()
{
    DestroyCube();
    //DestroyShaders();
    //DestroyVBO();
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
