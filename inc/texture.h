#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

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

  std::unordered_map<size_t, Texture> textures;

  size_t createTexture(const int width, const int height, const unsigned char* const data)
  {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // TODO: Add error checking here
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    const Texture t(width, height, tex);
    // TODO: Come up with a better storage ID that won't overflow - UUIDs?
    const auto id = textures.size();
    textures[id] = t;
    return id;
  }

  Texture& getTexture(const size_t id)
  {
    return textures[id];
  }

  void freeTexture(const size_t id)
  {
    const auto t = textures[id];
    textures.erase(id);
    glDeleteTextures(1, &t.textureId);
  }
}
