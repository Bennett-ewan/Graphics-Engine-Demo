#include <stdafx.h>

#include <Graphics\Mesh.h>
#include <Graphics\GraphicsSystem.h>
#include <Graphics/MeshLibrary.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureLibrary.h>
#include <Objects/Primitives.h>

#include <Scenes/SceneManager.h>
#include <Scenes/Scene.h>
#include <Scenes/Skybox.h>
namespace Graphics
{
  Mesh::Mesh(Shapes::Primitive* primitive) : vertices_(primitive->Get_Vertices()), indices_(primitive->Get_Indices()), scale_(1.f, 1.f), offset_(0.f, 0.f, 0.f)
  {
    Init_VAO();
  }

  Mesh::Mesh(const std::vector<Vertex>& vert, const std::vector<unsigned>& ind) : vertices_(vert), indices_(ind), scale_(1.f, 1.f), offset_(0.f, 0.f, 0.f)
  {
    Init_VAO();
  }

  Mesh::Mesh() : vertices_(Graphics::MeshLibrary::Get_Mesh("Triangle")->Get_Vertices()), indices_(Graphics::MeshLibrary::Get_Mesh("Triangle")->Get_Indices()),
                 scale_(1.f, 1.f), offset_(0.f, 0.f, 0.f)
  {

  }

  Mesh::Mesh(const Mesh& other) : vertices_(other.vertices_), indices_(other.indices_)
  {
    *this = other;
    Init_VAO();
  }

  Mesh::~Mesh()
  {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  void Mesh::Update()
  {
  }

  void Mesh::Draw(Shader* shader)
  {
    unsigned diffuseNum = 1;
    unsigned specularNum = 1;
    unsigned normalNum = 1;

    shader->Set_Uniform(scale_, "texture_scale");
    shader->Set_Uniform(offset_, "pos_offset");

    if (textures_.size() > 0)
    {
      shader->Set_Uniform(1, "has_texture");
    }
    else
      shader->Set_Uniform(0, "has_texture");
    shader->Set_Uniform(0, "has_diffuse");
    shader->Set_Uniform(0, "has_specular");
    shader->Set_Uniform(0, "has_normal");
    for (unsigned i = 0; i < textures_.size(); ++i)
    {
      int has_diffuse = 0;
      int has_specular = 0;
      int has_normal = 0;


      std::string number;
      std::string name = textures_[i]->type;
      if (name == "texture_diffuse")
      {
        number = std::to_string(diffuseNum++);
        if (has_diffuse == 0)
        {
          has_diffuse = 1;
          shader->Set_Uniform(has_diffuse, "has_diffuse");
        }
      }
      else if (name == "texture_specular")
      {
        number = std::to_string(specularNum++);
        if (has_specular == 0)
        {
          has_specular = 1;
          shader->Set_Uniform(has_specular, "has_specular");
        }

      }
      else if (name == "texture_normal")
      {
        number = std::to_string(normalNum++);
        if (has_normal == 0)
        {
          has_normal = 1;
          shader->Set_Uniform(has_normal, "has_normal");
        }
      }

      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, textures_[i]->Get_ID());
      shader->Set_Uniform((int)i, std::string("material." + name + number).c_str());
    }

    glBindVertexArray(VAO);

    Skybox* skybox = SceneManager::Current_Scene()->Get_Skybox();
    if (skybox)
    {
      skybox->Bind();
    }

    // Bind VAO
    if (indices_.size() == 0)
    {
      glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()) * sizeof(Vertex));
    }
    else
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
  }
  



  void Mesh::Init_VAO()
  {
    // VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

    // EBO
    if (indices_.size() > 0)
    {
      glGenBuffers(1, &EBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), &indices_[0], GL_STATIC_DRAW);
    }


    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    //Texcoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
    //Normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    //Tangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
    //Bitangents
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));

    glBindVertexArray(0);

  }


  void Mesh::Write(std::ofstream& file)
  {
    size_t name_size = name_.size();
    file.write(reinterpret_cast<const char*>(&name_size), sizeof(size_t));
    file.write(name_.c_str(), name_size);

    file.write(reinterpret_cast<char*>(&scale_[0]), sizeof(glm::vec2));

    size_t tex_size = textures_.size();
    file.write(reinterpret_cast<const char*>(&tex_size), sizeof(size_t));


    for (unsigned i = 0; i < textures_.size(); ++i)
    {
      std::string tex_name = textures_[i]->Name();
      size_t name_size = tex_name.size();
      file.write(reinterpret_cast<char*>(&name_size), sizeof(size_t));
      file.write(tex_name.c_str(), name_size);

    
      size_t type_size = textures_[i]->type.size();
      file.write(reinterpret_cast<char*>(&type_size), sizeof(size_t));
      file.write(textures_[i]->type.c_str(), type_size);
    }


  }

  void Mesh::Read(std::fstream& file)
  {
    file.read(reinterpret_cast<char*>(&scale_[0]), sizeof(glm::vec2));

    size_t tex_size = 0;
    file.read(reinterpret_cast<char*>(&tex_size), sizeof(size_t));

    for (unsigned i = 0; i < tex_size; ++i)
    {
      size_t name_size = 0;
      file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
      std::string tex_name;
      tex_name.resize(name_size);
      file.read(&tex_name[0], name_size);
      std::string path = std::string(tex_name.c_str(), name_size);
      Graphics::Texture* texture = Graphics::TextureLibrary::Get_Texture(path);
      

      size_t type_size = 0;
      file.read(reinterpret_cast<char*>(&type_size), sizeof(size_t));
      std::string type;
      type.resize(type_size);
      file.read(&type[0], type_size);
      if (texture)
      {
        texture->type = std::string(type.c_str(), type_size);
        Add_Texture(texture);
      }
      
    }

  }

  const Mesh& Mesh::operator=(const Mesh& other)
  {
    textures_ = other.textures_;
    scale_ = other.scale_;
    offset_ = other.offset_;
    name_ = other.name_;
    //VAO = other.VAO;
    //VBO = other.VBO;
    //EBO = other.EBO;
    return *this;
  }


}