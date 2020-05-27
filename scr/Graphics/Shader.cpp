#include <stdafx.h>

#include <Graphics/Shader.h>

namespace Graphics
{
  Shader::Shader(const std::string& vertex, const std::string& fragment, const std::string& geometry)
  {
    GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, vertex);
    GLuint frag_shader = load_shader(GL_FRAGMENT_SHADER, fragment);
    if (geometry != "")
    {
      GLuint geometry_shader = load_shader(GL_GEOMETRY_SHADER, geometry);
      link_program(vertex_shader, frag_shader, geometry_shader);
    }
    else
      link_program(vertex_shader, frag_shader);
  }


  Shader::~Shader()
  {
    glDeleteProgram(id);
  }


  void Shader::Use() const
  {
    glUseProgram(id);
  }

  void Shader::Unuse() const
  {
    glUseProgram(0);
  }

  void Shader::Set_Uniform(const glm::vec2& vec, const GLchar* name)
  {
    glUniform2fv(glGetUniformLocation(id, name), 1, glm::value_ptr(vec));
  }

  void Shader::Set_Uniform(const glm::vec3& vec, const GLchar* name)
  {
    glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(vec));
  }

  void Shader::Set_Uniform(const glm::vec4& vec, const GLchar* name)
  {
    glUniform4fv(glGetUniformLocation(id, name), 1, glm::value_ptr(vec));
  }

  void Shader::Set_Uniform(const glm::mat4& mtx, const GLchar* name)
  {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(mtx));
  }

  void Shader::Set_Uniform(const float& value, const GLchar* name)
  {
    glUniform1f(glGetUniformLocation(id, name), value);
  }

  void Shader::Set_Uniform(const int& value, const GLchar* name)
  {

    glUniform1i(glGetUniformLocation(id, name), value);
  }
  

  std::string Shader::load_shader_file(const std::string& file)
  {
    std::string src = "";
    std::string line = "";

    std::ifstream in_file;

    in_file.open(file);

    if (!in_file.is_open())
      std::cout << "Cannot open shader: " << file << std::endl;

    while (std::getline(in_file, line))
      src += line + "\n";
    in_file.close();

    return src;
  }

  GLuint Shader::load_shader(GLenum type, const std::string& file)
  {
    GLint success;
    char info_log[512];

    GLuint shader;
    shader = glCreateShader(type);
    std::string shader_src = load_shader_file(file);
    const GLchar* src = shader_src.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 512, NULL, info_log);
      std::cout << "Could not compile shader: " << file << std::endl;
      std::cout << info_log << std::endl;
    }

    return shader;
  }

  void Shader::link_program(GLuint vertex, GLuint fragment, GLuint geometry)
  {
    GLint success;
    char info_log[512];

    id = glCreateProgram();

    glAttachShader(id, vertex);

    glAttachShader(id, fragment);
    
    if (geometry != (GLuint)-1)
      glAttachShader(id, geometry);

    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(id, 512, NULL, info_log);
      std::cout << "Could not link program: \n";
      std::cout << info_log << std::endl;
    }

    // Delete shaders and reset program
    glUseProgram(0);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry != (GLuint)-1)
      glDeleteShader(geometry);
  }

}