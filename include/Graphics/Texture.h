#pragma once
class Model;

namespace Graphics
{
  class Material;

  class Texture
  {
  public:
    Texture(const std::string& filename);
    ~Texture();

    void Bind() const;
    void Unbind() const;


    GLuint Get_ID() const { return texture_id; }

    const std::string& Filename() const { return filename_; }
    const std::string& Name() const { return name_; }

    const std::string& Type() const { return type; }
    void Type(const std::string& type) { this->type = type; }

  private:
    GLuint texture_id;
    std::string type;
    std::string path;

    std::string name_;
    std::string filename_;

    friend class Model;
    friend class Material;
    friend class Mesh;
  };
}