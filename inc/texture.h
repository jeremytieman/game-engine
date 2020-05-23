#pragma once

#include <glad/glad.h>

namespace DragonGameEngine::Texture
{
    class Texture
    {
      public:
        Texture() {}
        Texture(const unsigned int width, const unsigned int height, const GLuint textureId) :
          width(width), height(height), textureId(textureId) {}
        unsigned int width;
        unsigned int height;
        GLuint textureId;
    };
}
