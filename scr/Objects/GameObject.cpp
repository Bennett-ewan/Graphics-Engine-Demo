#include <stdafx.h>

#include <Components/Component.h>
#include <Objects/GameObject.h>

#include <Components/Transform.h>
#include <Components/Model.h>
#include <Components/Physics.h>

#include <Graphics/Light.h>
#include <Graphics/Shader.h>
#include <Graphics/ShaderSystem.h>

GameObject::GameObject(const GameObject& other) : has_components_((int)Component::ComponentType::cmp_Count), destroy_(false), lights()
{
  name_ = other.name_;
  for (Components::const_iterator it = other.components_.begin(); it != other.components_.end(); ++it)
  {
    Add_Component((*it)->Clone());
  }
  for (std::vector<Graphics::Light*>::const_iterator it = other.lights.begin(); it != other.lights.end(); ++it)
  {
    Graphics::PointLight* light = new Graphics::PointLight(*static_cast<Graphics::PointLight*>(*it));
    lights.push_back(light);
  }
}

GameObject::~GameObject()
{
  for (Components::iterator it = components_.begin(); it != components_.end(); ++it)
  {
    delete (*it);
  }
  for (std::vector<Graphics::Light*>::const_iterator it = lights.begin(); it != lights.end(); ++it)
  {
    delete (*it);
  }
  lights.clear();
  components_.clear();
}


void GameObject::Tick(float dt)
{
  if (active_)
  {
    for (Components::iterator it = components_.begin(); it != components_.end(); ++it)
    {
      if ((*it)->Active())
        (*it)->Tick(dt);
    }
  }
}

void GameObject::Draw()
{
  if (active_)
  {
    for (Components::iterator it = components_.begin(); it != components_.end(); ++it)
    {
      if ((*it)->Active())
        (*it)->Draw();
    }
  } 
}

void GameObject::Add_Component(Component::ComponentType type)
{
  switch (type)
  {
  case Component::ComponentType::cmp_Transform:
  {
    Transform* transform = new Transform();
    components_.push_back(transform);
    transform->Parent(this);
    break;
  }
  case Component::ComponentType::cmp_Model:
  {
    Model* model = new Model();
    components_.push_back(model);
    model->Parent(this);
    break;
  }
  has_components_[(int)type] = true;
  }
}

void GameObject::Add_Component(Component* component)
{
  components_.push_back(component);
  component->Parent(this);
  has_components_[(int)component->Type()] = true;
}

void GameObject::Add_Light()
{
  Graphics::PointLight* light = new Graphics::PointLight(this->Get(Transform)->Translation());
  lights.push_back(light);
}

void GameObject::Write(std::ofstream& file)
{
  size_t len = name_.size();
  int count = sizeof(size_t);
  file.write(reinterpret_cast<const char*>(&len), sizeof(len));
  file.write(name_.c_str(), name_.size());
  file.write(reinterpret_cast<char*>(&active_), sizeof(active_));
  size_t comp_size = components_.size();
  file.write(reinterpret_cast<char*>(&(comp_size)), sizeof(size_t));
  for (unsigned i = 0; i < components_.size(); ++i)
  {
    Component::ComponentType type = components_[i]->Type();
    file.write(reinterpret_cast<char*>(&type), sizeof(Component::ComponentType));
    components_[i]->Write(file);
  }

  // lights
  size_t light_size = lights.size();
  file.write(reinterpret_cast<const char*>(&light_size), sizeof(size_t));
  for (unsigned i = 0; i < lights.size(); ++i)
  {
    lights[i]->Write(file);
  }
}

void GameObject::Read(std::fstream& file)
{ 
  bool active = false;
  file.read(reinterpret_cast<char*>(&active), sizeof(bool));
  active_ = active;

  size_t comp_size = 0;
  file.read(reinterpret_cast<char*>(&comp_size), sizeof(size_t));
  for (int i = 0; i < comp_size; ++i)
  {
    Component::ComponentType type = Component::ComponentType::cmp_Count;
    file.read(reinterpret_cast<char*>(&type), sizeof(Component::ComponentType));
    switch (type)
    {
    case Component::ComponentType::cmp_Physics:
    {
      Physics *physics = new Physics();
      physics->Read(file);
      Add_Component(physics);
      break;
    }
    case Component::ComponentType::cmp_Model:
    {
      Model* model = new Model();
      model->Read(file);
      Add_Component(model);
      break;
    }
    case Component::ComponentType::cmp_Transform:
    {
      Transform* transform = new Transform();
      transform->Read(file);
      Add_Component(transform);
      break;
    }
    }
  }

  size_t light_size = 0;
  file.read(reinterpret_cast<char*>(&light_size), sizeof(size_t));
  for (unsigned i = 0; i < light_size; ++i)
  {
    Graphics::Light::Type type = Graphics::Light::Type::None;
    file.read(reinterpret_cast<char*>(&type), sizeof(Graphics::Light::Type));
    switch (type)
    {
    case Graphics::Light::Type::Point:
    {
      Graphics::PointLight* light = new Graphics::PointLight;
      light->Read(file);
      lights.push_back(light);
      break;
    }
    }
  }
}
