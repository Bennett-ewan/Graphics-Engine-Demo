#include <stdafx.h>


#include <Graphics/Light.h>
#include <Graphics/Shader.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/MeshLibrary.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>

#include <Graphics/Camera.h>
#include <Scenes/SceneManager.h>
#include <Scenes/Scene.h>

namespace Graphics
{
  Light::Light(float intensity_, const glm::vec3& color_) : intensity(intensity_), diffuse(color_), ambient(0.5f, 0.5f, 0.5f), specular(1.f, 1.f, 1.f), type(Type::None),
                                                            lightProjection(1.f), lightView(1.f)
  {
  }

  Light::~Light()
  {
  }


  PointLight::PointLight(const glm::vec3& pos, float intensity, const glm::vec3& color)
    : Light(intensity, color), position(pos), constant(1.f), linear(0.09f), quadratic(0.032f)
      
  {
    mesh = Graphics::MeshLibrary::Get_Mesh("Sphere");
    type = Type::Point;
    lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
    lightView = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f));
    lightSpaceMatrix = lightProjection * lightView;
  }


  PointLight::~PointLight()
  {

  }

  void PointLight::Send(Shader& shader, unsigned index)
  {
    shader.Use();
    std::string num = std::to_string(index);

    shader.Set_Uniform(position, std::string("pointlights[" + num + "].pos").c_str());
    shader.Set_Uniform(intensity, std::string("pointlights[" + num + "].intensity").c_str());
    shader.Set_Uniform(diffuse, std::string("pointlights[" + num + "].diffuse").c_str());
    shader.Set_Uniform(ambient, std::string("pointlights[" + num + "].ambient").c_str());
    shader.Set_Uniform(specular, std::string("pointlights[" + num + "].specular").c_str());
    
    shader.Set_Uniform(constant, std::string("pointlights[" + num + "].constant").c_str());
    shader.Set_Uniform(linear, std::string("pointlights[" + num + "].linear").c_str());
    shader.Set_Uniform(quadratic, std::string("pointlights[" + num + "].quadratic").c_str());

    shader.Set_Uniform(lightSpaceMatrix, std::string("pointlights[" + num + "].spaceMatrix").c_str());
    
    shader.Unuse();
  }

  void PointLight::Draw()
  {
    glm::mat4 mtx(1.f);
    mtx = glm::translate(mtx, position);
    mtx = glm::scale(mtx, intensity * glm::vec3(0.25f, 0.25f, 0.25f));

    Graphics::Shader* shader = Graphics::Shaders::Get_Shader("Core");

    Graphics::Material material;
    material.Ambient(ambient);
    material.Diffuse(diffuse);
    material.Specular(specular);
    shader->Use();
    material.Send(shader);
    shader->Set_Uniform(mtx, "Model");

    mesh->Draw(shader);

    shader->Unuse();
  }

  void PointLight::Write(std::ofstream& file)
  {
    file.write(reinterpret_cast<const char*>(&type), sizeof(Type));
    file.write(reinterpret_cast<const char*>(&position[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&diffuse[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&ambient[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&specular[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&intensity), sizeof(float));
    size_t name_size = mesh->Name().size();
    file.write(reinterpret_cast<const char*>(&name_size), sizeof(size_t));
    file.write(mesh->Name().c_str(), name_size);
    //mesh->Write(file);
  }

  void PointLight::Read(std::fstream& file)
  {
    file.read(reinterpret_cast<char*>(&position[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&diffuse[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&ambient[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&specular[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&intensity), sizeof(float));

    size_t name_size = 0;
    file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));

    std::string name;
    name.reserve(name_size);
    file.read(&name[0], name_size);
    std::string name_ = std::string(name.c_str(), name_size);
    mesh = Graphics::MeshLibrary::Get_Mesh(name_);
  }


  DirectionalLight::DirectionalLight(const glm::vec3& dir, float intensity, const glm::vec3& color)
    : Light(intensity, color), direction(dir), mesh(nullptr)
  {
    type = Type::Direction;
  }


  DirectionalLight::~DirectionalLight()
  {

  }

  void DirectionalLight::Send(Shader& shader, unsigned index)
  {
    shader.Use();
    std::string num = std::to_string(index);
    shader.Set_Uniform(direction, std::string("dirlight[" + num + "].direction").c_str());
    shader.Set_Uniform(intensity, std::string("dirlight[" + num + "].intensity").c_str());
    shader.Set_Uniform(diffuse, std::string("dirlight[" + num + "].diffuse").c_str());
    shader.Set_Uniform(ambient, std::string("dirlight[" + num + "].ambient").c_str());
    shader.Set_Uniform(specular, std::string("dirlight[" + num + "].specular").c_str());

    shader.Unuse();
  }

  void DirectionalLight::Draw()
  {
  }

  void DirectionalLight::Write(std::ofstream& file)
  {
    file.write(reinterpret_cast<const char*>(&type), sizeof(Type));
    file.write(reinterpret_cast<const char*>(&direction[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&diffuse[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&ambient[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&specular[0]), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&intensity), sizeof(float));

  }

  void DirectionalLight::Read(std::fstream& file)
  {
    file.read(reinterpret_cast<char*>(&direction[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&diffuse[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&ambient[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&specular[0]), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&intensity), sizeof(float));
  }
}

