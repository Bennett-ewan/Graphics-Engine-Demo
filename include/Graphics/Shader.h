#pragma once


namespace Graphics
{


  class Shader
  {
  public:
    Shader(const std::string& vertex, const std::string& fragment, const std::string& geometry = "");
    ~Shader();

    const std::string& Name() const { return name; }
    void Name(const std::string& name) { this->name = name; }

    GLuint Get_ID() const { return id; }
    void Use() const;
    void Unuse() const;
    void Set_Uniform(const glm::vec2& vec, const GLchar* name);
    void Set_Uniform(const glm::vec3& vec, const GLchar* name);
    void Set_Uniform(const glm::vec4& vec, const GLchar* name);
    void Set_Uniform(const glm::mat4& mtx, const GLchar* name);
    void Set_Uniform(const float& value, const GLchar* name);
    void Set_Uniform(const int& value, const GLchar* name);
  private:
    std::string name;
    GLuint id;

    std::string load_shader_file(const std::string& file);
    GLuint load_shader(GLenum type, const std::string& file);
    void link_program(GLuint vertex, GLuint fragment, GLuint geometry = (GLuint)-1);
  };

  
}
