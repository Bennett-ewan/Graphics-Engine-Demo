#include <stdafx.h>

#include <Components/Transform.h>

#include <Objects/GameObject.h>
#include <Graphics/Light.h>

Transform::Transform() : Component(ComponentType::cmp_Transform), translation(0.f, 0.f, 0.f), scale(1.f, 1.f, 1.f), 
                         is_dirty(true), matrix()
{
}

Transform::~Transform()
{

}

Transform::Transform(const Transform& other) : Component(ComponentType::cmp_Transform)
{
  *this = other;
}

Component* Transform::Clone() const
{
  return new Transform(*this);
}

void Transform::Init()
{

}

void Transform::Tick(float dt)
{

}

void Transform::DInit()
{
 
}

void Transform::Desearialize(rapidjson::Document& document)
{
  const rapidjson::Value& tran = document["Transform"];
  assert(tran.IsObject());
  rapidjson::GenericObject transform = tran.GetObject();

  if (transform.HasMember("Translation"))
  {
    if (transform["Translation"].IsArray())
    {
      const rapidjson::Value& translation = transform["Translation"];
      this->translation = glm::vec3(translation[0].GetFloat(), translation[1].GetFloat(), translation[2].GetFloat());
    }
  }

  if (transform.HasMember("Scale"))
  {
    if (transform["Scale"].IsArray())
    {
      const rapidjson::Value& scale = transform["Scale"];
      this->scale = glm::vec3(scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat());
    }
  }

  if (transform.HasMember("Rotation"))
  {
    if (transform["Rotation"].IsArray())
    {
      const rapidjson::Value& rotation = transform["Rotation"];
      this->rotation.x = rotation[0].GetFloat();
      this->rotation.y = rotation[1].GetFloat();
      this->rotation.z = rotation[2].GetFloat();
    }
  }
}

void Transform::Write(std::ofstream& file)
{
  file.write(reinterpret_cast<char*>(&translation[0]), sizeof(glm::vec3));
  file.write(reinterpret_cast<char*>(&scale[0]), sizeof(glm::vec3));
  file.write(reinterpret_cast<char*>(&rotation[0]), sizeof(glm::vec3));
}


void Transform::Read(std::fstream& file)
{
  file.read(reinterpret_cast<char*>(&translation[0]), sizeof(glm::vec3));
  file.read(reinterpret_cast<char*>(&scale[0]), sizeof(glm::vec3));
  file.read(reinterpret_cast<char*>(&rotation), sizeof(glm::vec3));
}

void Transform::Translation(const glm::vec3& translation) 
{ 
  if (Parent())
  {
    std::vector<Graphics::Light*>& lights = Parent()->Get_Lights();
    for (unsigned i = 0; i < lights.size(); ++i)
    {
      Graphics::PointLight* point = static_cast<Graphics::PointLight*>(lights[i]);
      point->Position(point->Position() + (translation - this->translation));
    }
  }
  this->translation = translation; 
  is_dirty = true;


}

void Transform::calc_matrix()
{
  matrix = glm::mat4(1.f);
  matrix = glm::translate(matrix, translation);
  matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  matrix = glm::scale(matrix, scale);

  is_dirty = false;
}