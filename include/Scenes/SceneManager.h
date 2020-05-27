#pragma once


class System;
class Scene;

typedef std::map<std::string, Scene*> Scenes;

namespace SceneManager
{
  System* Init();
  void Update(float dt);
  void Shutdown();

  Scene* Current_Scene();
  void Next_Scene(const std::string& name);
  void Next_Scene(Scene* scene);
  Scene* Previous_Scene();
  Scene* Create_Scene(const std::string& name);
  void Restart();
  void Quit();
  const Scenes& Get_Scenes();
}