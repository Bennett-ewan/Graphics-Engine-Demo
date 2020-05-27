#include <stdafx.h>

#include <Scenes/SceneManager.h>
#include <Core/System.h>
#include <Core/Engine.h>

#include <Scenes/Scene.h>

#include <Graphics/GraphicsSystem.h>

namespace SceneManager
{
  class SceneSystem : public System
  {
  public:
    struct State
    {
      Scene* current;
      Scene* previous;
      Scene* next;
    };


    SceneSystem();
    ~SceneSystem();

    void Init();
    void Update(float dt);
    void Draw();
    void Shutdown();

    void Restart();

    Scene* Current_Scene() const
    {
      return game_state.current;
    }

    void Next_Scene(const std::string& name) {
      Scenes::iterator it = scenes.find(name);
      if (it != scenes.end())
        game_state.next = it->second;
    }

    void Next_Scene(Scene* scene) {
      game_state.next = scene;
    }

    Scene* Previous_Scene()
    {
      return game_state.previous;
    }

    Scene* Create_Scene(const std::string& name) {
      Scenes::iterator it = scenes.find(name);
      if (it != scenes.end())
        return it->second;
      Scene* scene = new Scene(name);
      create_lua_file(name);
      scenes[name] = scene;
      return scene;
    }

    const Scenes& Get_Scenes() const {
      return scenes;
    }


  private:
    State game_state;

    Scenes scenes;


    bool changing() { return game_state.next != nullptr; }
    void create_lua_file(const std::string& name);

    friend class Scene;
  };


  SceneSystem::SceneSystem() : game_state{ nullptr, nullptr, nullptr }
  {
    std::string path = "./Assets/Data/Scenes";
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
      std::string scene_name = entry.path().generic_string();
      size_t pos = scene_name.find("/Scenes/");
      std::string short_name = std::string(scene_name, pos + 8);
      scenes[short_name] = new Scene(short_name);
    }
  }

  SceneSystem::~SceneSystem()
  {
    if (game_state.previous)
      game_state.previous->Unload();
    if (game_state.current)
    {
      game_state.current->Shutdown();
      game_state.current->Unload();
    }
    for (Scenes::iterator it = scenes.begin(); it != scenes.end(); ++it)
    {
      delete it->second;
    }
  }

  void SceneSystem::Init()
  {
    try {
      game_state.next = scenes.at("SceneMenu");
    }
    catch (...) {
      game_state.next = Create_Scene("SceneMenu");
    }

    if (scenes.find("ArchetypeEditor") == scenes.end())
      Create_Scene("ArchetypeEditor");
  }

  void SceneSystem::Update(float dt)
  {
    if (changing()) {
      bool load = game_state.previous == game_state.next ? false : true;
      if (game_state.previous && load)
        game_state.previous->Unload();
      
      game_state.previous = game_state.current;

      if (game_state.current) {
        game_state.current->Shutdown();
      }

      game_state.current = game_state.next;
      game_state.next = nullptr;

      if (load)
        game_state.current->Load();
      game_state.current->Init();
    }

    if (game_state.current)
      game_state.current->Update(dt);
  }

  void SceneSystem::Shutdown()
  {

  }


  void SceneSystem::Draw()
  {
    if (game_state.current)
    {
      game_state.current->Draw();
      game_state.current->Kill_Objects();
    }
  }

  void SceneSystem::Restart()
  {
    if (game_state.current)
    {
      game_state.current->Shutdown();
      game_state.current->Init();
    }

  }

  void SceneSystem::create_lua_file(const std::string& name)
  {
    std::string path("./Assets/Scripts/Scenes/" + name + ".lua");
    if (fopen(path.c_str(), "r"))
      return;
    std::ifstream infile("./Assets/Scripts/Scenes/Stub.lua");
    std::ofstream outfile(path);

    std::string line;
    size_t pos;
    while (std::getline(infile, line))
    {
      if ((pos = line.find("Stub")) != std::string::npos)
      {
        line.replace(pos, 4, name);
      }
      outfile << line;
      outfile << "\n";
    }
  }

  /*************************************************************/
  /*                        ECS Instance                       */
  /*************************************************************/

  static SceneSystem* Scene_System = nullptr;

  System* Init()
  {
    static SceneSystem scene_sys;
    Scene_System = &scene_sys;
    return Scene_System;
  }

  void Update(float dt)
  {
    if (Scene_System)
      Scene_System->Update(dt);
  }

  void Shutdown()
  {
    if (Scene_System)
      Scene_System->Shutdown();
  }

  Scene* Current_Scene()
  {
    if (Scene_System)
      return Scene_System->Current_Scene();
    return nullptr;
  }

  void Next_Scene(const std::string& name)
  {
    Scene_System->Next_Scene(name);
  }

  void Next_Scene(Scene* scene)
  {
    Scene_System->Next_Scene(scene);
  }

  Scene* Previous_Scene()
  {
    return Scene_System->Previous_Scene();
  }

  Scene* Create_Scene(const std::string& name)
  {
    return Scene_System->Create_Scene(name);
  }

  void Quit()
  {
    glfwSetWindowShouldClose(Graphics::Window(), GL_TRUE);
  }

  void Restart()
  {
    Scene_System->Restart();
  }

  const Scenes& Get_Scenes()
  {
    return Scene_System->Get_Scenes();
  }

}
