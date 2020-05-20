#include <ogl.h>

#include <glad/glad.h>
#include <iostream>
#include <string>

#include <util.h>

namespace DGE = DragonGameEngine;

namespace DragonGameEngine::OGL
{
    void ExitOnGLError(const std::string& error_message)
    {
        const GLenum ErrorValue = glGetError();

        if (ErrorValue != GL_NO_ERROR)
        {
            std::cerr << error_message << ": " << ErrorValue << " \n";
            exit(-1);
        }
    }

    GLuint LoadShader(const std::string& fileName, GLenum shaderType)
    {
        auto glslSource = DGE::Util::getFileContents(fileName);
        auto shaderId = glCreateShader(shaderType);
        if (0 == shaderId) std::cerr << "ERROR: Could not create a shader.\n";
        auto sourcePtr = glslSource.c_str();
        glShaderSource(shaderId, 1, &sourcePtr, NULL);
        glCompileShader(shaderId);
        ExitOnGLError("Could not compile a shader");
        return shaderId;
    }
}
