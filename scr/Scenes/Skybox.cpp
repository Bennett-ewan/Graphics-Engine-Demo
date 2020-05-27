#include <stdafx.h>

#include <Scenes/Skybox.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshLibrary.h>

#include <Graphics/Shader.h>
#include <Graphics/ShaderSystem.h>

#include <Scenes/Scene.h>
#include <Scenes/SceneManager.h>
#include <Graphics/Camera.h>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

float skyboxVertices[] = {
  // positions          
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f
};

Skybox::Skybox(const std::vector<std::string>& images) : shader(nullptr), offset(0.f, 0.f, 0.f), tex_ID(0), VAO(0), VBO(0), directory("")
{
  init(images);
  directory = std::string(images[0], 0, images[0].find("right") - 1);
}

Skybox::Skybox(const std::string& directory) : shader(nullptr), offset(0.f, 0.f, 0.f), tex_ID(0), VAO(0), VBO(0), directory(directory)
{
  bool has_left = false, has_right = false, has_top = false, has_bottom = false, has_back = false, has_front = false;
  std::vector<std::string> filenames;
  filenames.resize(6);
  for (const auto& entry : std::filesystem::directory_iterator(directory))
  {
    std::string file = entry.path().generic_string();
    if (file.find("back.") != std::string::npos) {
      filenames[5] = file;
      has_back = true;
    }
    else if (file.find("front.") != std::string::npos){
      filenames[4] = file;
      has_front = true;
    }
    else if (file.find("left.") != std::string::npos) {
      filenames[1] = file;
      has_left = true;
    }
    else if (file.find("right.") != std::string::npos) {
      filenames[0] = file;
      has_right = true;
    }
    else if (file.find("top.") != std::string::npos) {
      filenames[2] = file;
      has_top = true;
    }
    else if (file.find("bottom.") != std::string::npos) {
      filenames[3] = file;
      has_bottom = true;
    }
  }
  if (has_bottom && has_top && has_left && has_right && has_front && has_back)
    init(filenames);
  else
    std::cout << "Skybox Error: Files not in correct format in directory: " << directory << std::endl;
}

Skybox::~Skybox()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteTextures(1, &tex_ID);
}


void Skybox::Bind()
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_ID);
}


void Skybox::Draw()
{
  if (!shader)
    return;
  //glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  Graphics::Camera::Set_Uniforms(shader);
  shader->Use();
  shader->Set_Uniform((int)tex_ID, "skybox");
  shader->Set_Uniform(offset, "offset");
  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0 + tex_ID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_ID);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
  //glDepthMask(GL_TRUE);
  shader->Unuse();
}

void Skybox::init(const std::vector<std::string>& images)
{
  glGenTextures(1, &tex_ID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_ID);

  int width, height, num;
  for (unsigned int i = 0; i < images.size(); i++)
  {
    unsigned char* data = stbi_load(images[i].c_str(), &width, &height, &num, 0);
    if (data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
      std::cout << "Failed to load texture for skybox: " << images[i] << std::endl;
    }
    stbi_image_free(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // VAO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
  glBindVertexArray(0);

  Graphics::Shader* core = Graphics::Shaders::Get_Shader("Core");
  core->Use();
  core->Set_Uniform((int)tex_ID, "skybox");
  core->Unuse();

  shader = Graphics::Shaders::Get_Shader("Skybox");
}