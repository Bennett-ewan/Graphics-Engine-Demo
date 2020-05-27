#pragma once

#include <Components\Component.h>

#define GRAVITY_CONSTANT -9.8f

class Physics : public Component
{
public:
  struct Mass
  {
    float mass;
    float inv_mass;

    float inertia;
    float inv_inertia;
  };

  struct Material
  {
    float density;
    float restitution;
    float friction_coeff;
  };

  Physics();
  ~Physics();
  Physics(const Physics& other);
  Component* Clone() const;

  void Init();
  void Tick(float dt);
  void DInit();

  void Desearialize(rapidjson::Document& document);

  void Write(std::ofstream& file);
  void Read(std::fstream& file);

  const glm::vec3& Velocity() const { return velocity_; }
  void Velocity(const glm::vec3& velocity) { velocity_ = velocity; }

  void Add_Force(const glm::vec3& force) { force_.push_back(force); }

private:
  glm::vec3 velocity_;
  std::vector<glm::vec3> force_;
  Mass mass_data_;
  Material material_;
  bool gravity_affected_;

  glm::vec3 sum_forces() const;
  void apply_gravity();
};