#pragma once

#include <Components\Component.h>



class Transform;

class Collider : public Component
{
public:

  enum class Shape
  {
    Sphere,
    Box
  };

  Collider(::Transform* transform);
  Collider(Shape shape);
  ~Collider();
  Collider(const Collider& other);
  Component* Clone() const;

  void Init();
  void Tick(float dt);
  void DInit();

  void Desearialize(rapidjson::Document& document);
  void Write(std::ofstream& file);
  void Read(std::fstream& file);

  rp3d::Vector3 Glm_To_React(const glm::vec3 v) { return rp3d::Vector3(v.x, v.y, v.z); }

  void Add_Shape(Shape shape);

private:
  Shape shape_;

  rp3d::RigidBody* body_;
  std::vector<rp3d::CollisionShape*> shapes_;
  //rp3d::ConvexShape* collider_shape_;
};