#pragma once

class System;
class GameObject;

namespace Editor
{
  System* Init();
  void Update(float dt);
  void Shutdown();
  void Object_Editor(bool archetype);
  void Selected_Object(GameObject* object);
  GameObject* Selected_Object();
  bool Unsaved_Changes();
  void Show_Save(bool show);
  void Scene_Selector();
  void Archetype_Editor();
  void Enable_Editor(bool enable);
}