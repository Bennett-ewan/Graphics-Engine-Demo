#pragma once

class GameObject;

class Component
{
public:
  enum class ComponentType{
    cmp_Transform,
    cmp_Model,
    cmp_Physics,
    cmp_Collider,

    cmp_Count
  };

  Component(ComponentType type) : type_(type), parent_(nullptr), active_(true) {}
  virtual ~Component() {};
  virtual Component* Clone() const = 0;

  virtual void Init() = 0;
  virtual void Tick(float dt) = 0;
  virtual void Draw() {};
  virtual void DInit() = 0;

  virtual void Desearialize(rapidjson::Document& document) = 0;

  virtual void Write(std::ofstream& file) = 0;
  virtual void Read(std::fstream& file) = 0;

  ComponentType Type() const { return type_; }

  GameObject* Parent() const { return parent_; }
  void Parent(GameObject* parent) { parent_ = parent; }

  void Active(const bool& active) { active_ = active; }
  const bool& Active() const { return active_; }

private:
  ComponentType type_;
  GameObject* parent_;
  bool active_;
};