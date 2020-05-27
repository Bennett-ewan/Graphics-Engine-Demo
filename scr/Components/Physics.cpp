#include <stdafx.h>

#include <Components/Physics.h>
#include <Objects/GameObject.h>
#include <Components/Transform.h>

Physics::Physics() : Component(ComponentType::cmp_Physics), velocity_(0.f), mass_data_{0.f, 0.f, 0.f, 0.f}, material_{1.f, 0.f, 0.f}, gravity_affected_(false)
{
}

Physics::~Physics()
{

}

Physics::Physics(const Physics& other) : Component(ComponentType::cmp_Physics)
{
  *this = other;
}


Component* Physics::Clone() const
{
  return new Physics(*this);
}

void Physics::Init()
{


}

void Physics::Tick(float dt)
{
  GameObject* object = Parent();
  Transform* transform = object->Get(Transform);

  velocity_ += (mass_data_.inv_mass * sum_forces()) * dt;
  glm::vec3 pos = transform->Translation();
  pos += velocity_ * dt;
  transform->Translation(pos);

  force_.clear();

  apply_gravity();
}

void Physics::DInit()
{
 
}

void Physics::Desearialize(rapidjson::Document& document)
{
  const rapidjson::Value& phy = document["Physics"];
  assert(phy.IsObject());
  rapidjson::GenericObject physics = phy.GetObject();

  if (physics.HasMember("Density"))
  {
    material_.density = physics["Density"].GetFloat();
  }
  if (physics.HasMember("Restitution"))
  {
    material_.restitution = physics["Restitution"].GetFloat();
  }
  if (physics.HasMember("Gravity_Affected"))
  {
    gravity_affected_ = physics["Gravity_Affected"].GetBool();
  }
}


void Physics::Write(std::ofstream& file)
{
  file.write(reinterpret_cast<char*>(&mass_data_), sizeof(mass_data_));
  file.write(reinterpret_cast<char*>(&material_), sizeof(material_));
  file.write(reinterpret_cast<char*>(&gravity_affected_), sizeof(gravity_affected_));
}

void Physics::Read(std::fstream& file)
{
  file.read(reinterpret_cast<char*>(&mass_data_), sizeof(mass_data_));
  file.read(reinterpret_cast<char*>(&material_), sizeof(material_));
  file.read(reinterpret_cast<char*>(&gravity_affected_), sizeof(gravity_affected_));
}


glm::vec3 Physics::sum_forces() const
{
  glm::vec3 sum(0.f);
  for (unsigned i = 0; i < force_.size(); ++i)
  {
    sum += force_[i];
  }
  return sum;
}


void Physics::apply_gravity()
{
  force_.push_back(glm::vec3(0.f, GRAVITY_CONSTANT, 0.f));
}
