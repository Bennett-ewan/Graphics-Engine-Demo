#include <stdafx.h>

#include <Graphics/Material.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>

namespace Graphics
{
  Material::Material() : ambient_(0.5f, 0.5f, 0.5f), diffuse_(0.75f, 0.75f, 0.75f), specular_(1.f, 1.f, 1.f), shininess_(0.f), refraction_index_(1.f), reflectiveness_(0.f)
  {

  }


  Material::Material(const glm::vec3& ambient,
    const glm::vec3& diffuse,
    const glm::vec3& specular,
    float shininess) : ambient_(ambient), diffuse_(diffuse), specular_(specular), shininess_(shininess), reflectiveness_(0.f), refraction_index_(1.f)
  {

  }

  Material::~Material()
  {
  }

  void Material::Send(Shader* program)
  {
    program->Set_Uniform(ambient_, "material.ambient");
    program->Set_Uniform(diffuse_, "material.diffuse");
    program->Set_Uniform(specular_, "material.specular");
    program->Set_Uniform(shininess_, "material.shininess");
    program->Set_Uniform(reflectiveness_, "material.reflectiveness");
    program->Set_Uniform(refraction_index_, "material.refraction_index");
  }


}

