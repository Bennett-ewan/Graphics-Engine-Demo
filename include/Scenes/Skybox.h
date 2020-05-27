#pragma once

//forward declarations
namespace Graphics { class Shader; }

class Skybox
{
public:
  Skybox(const std::vector<std::string>& images);
  Skybox(const std::string& directory);
  ~Skybox();

  void Draw();
  void Bind();

  const glm::vec3 Offset() const { return offset; }
  void Offset(const glm::vec3& offset) { this->offset = offset; }

  const std::string& Directory() const { return directory; }

private:
  unsigned tex_ID;
  GLuint VAO;
  GLuint VBO;
  Graphics::Shader* shader;
  glm::vec3 offset;

  std::string directory;

  void init(const std::vector<std::string>& images);
};