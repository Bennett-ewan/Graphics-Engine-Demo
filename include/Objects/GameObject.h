#pragma once

#include <Components/Component.h>

#define Get(type) Get_Component<type>(Component::ComponentType::cmp_##type)

typedef std::vector<Component*> Components;

namespace Graphics { class Light; }

class GameObject
{
public:
  GameObject() : name_(), active_(false), destroy_(false), has_components_((int)Component::ComponentType::cmp_Count), components_() {}
  GameObject(const std::string& name) : name_(name), active_(false), destroy_(false), has_components_((int)Component::ComponentType::cmp_Count), components_() {}
  GameObject(const GameObject& other);
  ~GameObject();

  void Tick(float dt);
  void Draw();

  const bool& Active() const { return active_; }
  void Active(const bool& active) { active_ = active; }

  void Destroy() { destroy_ = true; }
  const bool& Is_Destroy() const { return destroy_; }

  void Add_Component(Component::ComponentType type);
  void Add_Component(Component* component);

  const std::string& Name() const { return name_; }
  void Name(const std::string& name) { name_ = name; }

  std::vector<Graphics::Light*>& Get_Lights() { return lights; }
  void Add_Light();

  template <typename T>
  T* Get_Component(Component::ComponentType type)
  {
    for (Components::iterator it = components_.begin(); it != components_.end(); ++it)
    {
      if ((*it)->Type() == type)
        return static_cast<T*>(*it);
    }
    return nullptr;
  }


  const Components& Get_Components() const { return components_; }

  bool Has_Component(Component::ComponentType type) { return has_components_.at(static_cast<int>(type)); }

  void Write(std::ofstream& file);
  void Read(std::fstream& file);

private:
  std::string name_;
  bool active_;
  bool destroy_;
  std::vector<bool> has_components_;
  Components components_;
  std::vector<Graphics::Light*> lights;
};