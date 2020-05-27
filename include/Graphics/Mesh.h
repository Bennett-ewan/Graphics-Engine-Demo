#pragma once

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

namespace Shapes { class Primitive; }

namespace Graphics
{
  class Texture;
  class Shader;

  struct Vertex
  {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
  };

  class Mesh
  {
  public:
    Mesh();
    Mesh(Shapes::Primitive* primitive);
    Mesh(const std::vector<Vertex>& vert, const std::vector<GLuint>& ind);
    Mesh(const Mesh& other);
    ~Mesh();

    void Init_VAO();

    void Update();
    void Draw(Shader* shader);

    void Write(std::ofstream& file);
    void Read(std::fstream& file);

    const std::vector<Texture*>& Textures() const { return textures_; }
    void Add_Texture(Texture* texture) { textures_.push_back(texture); }
    Texture* Get_Texture(unsigned index) { if (index < textures_.size()) return textures_[index]; }
    void Set_Texture(Texture* texture, const unsigned& index) { if (index < textures_.size()) textures_[index] = texture; }
    void Remove_Texture(const unsigned& index) {
      std::vector<Texture*>::iterator it = textures_.begin() + index;
      textures_.erase(it);
    }
    void Remove_Texture(Texture* texture) {
      for (std::vector<Texture*>::iterator it = textures_.begin(); it != textures_.end(); ++it)
        if (texture == *it)
          textures_.erase(it);
    }
    const glm::vec2& Texture_Scale() const { return scale_; }
    void Texture_Scale(const glm::vec2& scale) { scale_ = scale; }

    const glm::vec3& Pos_Offset() const { return offset_; }
    void Pos_Offset(const glm::vec3& offset) { offset_ = offset; }

    const std::string& Name() const { return name_; }
    void Name(const std::string& name) { name_ = name; }

    const std::vector<Vertex>& Get_Vertices() const { return vertices_; }
    const std::vector<unsigned>& Get_Indices() const { return indices_; }

    const GLuint& Get_VAO() const { return VAO; }

    const Mesh& operator=(const Mesh& other);

  private:
    const std::vector<Vertex>& vertices_;
    const std::vector<unsigned>&  indices_;
    std::vector<Texture*> textures_;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    

    glm::vec2 scale_;
    glm::vec3 offset_; 

    std::string name_;
    
    friend class Model;

  };

  
}
