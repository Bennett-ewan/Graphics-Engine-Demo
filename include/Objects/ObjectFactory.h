#pragma once

class GameObject;
class System;

typedef std::unordered_map<std::string, GameObject> Archetypes;

namespace ObjectFactory
{
  System* Init();
  void Update(float dt);
  void Shutdown();

  GameObject* Load(const std::string& name);
  GameObject* Create(const std::string& name);
  std::fstream Blank_File(const std::string& name);
  GameObject* New_Archetype(const std::string& name);
  Archetypes& Get_Archetypes();
  void Save_Archetype(GameObject* object);
  void Remove_Archetype(GameObject* object);
}