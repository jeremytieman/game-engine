#pragma once

#include <string>

#include <glad/glad.h>

namespace DragonGameEngine::OGL
{
    void ExitOnGLError(const std::string& error_message);
    GLuint LoadShader(const std::string& fileName, GLenum shaderType);
}
