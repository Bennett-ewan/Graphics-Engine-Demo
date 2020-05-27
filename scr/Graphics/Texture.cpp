#include <stdafx.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Graphics/Texture.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>

namespace Graphics
{
  Texture::Texture(const std::string& filename) : texture_id(), type("texture_diffuse"), filename_(filename)
  {
    int i = static_cast<int>(filename.size());
    while (filename[i] != '/' && filename[i] != '\\')
    {
      --i;
    }
    name_ = std::string(filename, i + 1);

    stbi_set_flip_vertically_on_load(false);
    int width, height, num;

    
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    std::string file = filename;
    int pos;
    if ((pos = file.find(".tif")) != std::string::npos)
    {
      file.replace(pos + 1, 3, "png");
    }

    unsigned char* img = stbi_load(file.c_str(), &width, &height, &num, 4);
    if (!img)
    {
      std::cout << "Cannot load texture: " << filename << std::endl;
      return;
    }
    else
    {
      glTexImage2D(GL_TEXTURE_2D, 0, (GLint)GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
      //glTexImage2D(GL_TEXTURE_2D, 0, (GLint)GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 10);
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(img);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Graphics::Shaders::Get_Shader("Core")->Use();
    //Graphics::Shaders::Get_Shader("Core")->Set_Uniform((int)texture_id, "texture0");

  }

  Texture::~Texture()
  {
    glDeleteTextures(1, &texture_id);
  }


  void Texture::Bind() const
  {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
  }

  void Texture::Unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

