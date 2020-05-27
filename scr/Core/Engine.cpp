#include <stdafx.h>

#include <Core\Engine.h>
#include <Core\System.h>
#include <Core\ScopeTimer.h>

// Systems
#include <Graphics\GraphicsSystem.h>
#include <Graphics\ShaderSystem.h>
#include <Scenes/SceneManager.h>
#include <Graphics/MeshLibrary.h>
#include <Input/InputSystem.h>
#include <Objects/ObjectFactory.h>
#include <Scenes/Editor.h>
#include <Physics/Collision.h>
#include <Graphics/TextureLibrary.h>
#include <Objects/ModelLibrary.h>
#include <Graphics/Camera.h>

class System;

namespace Engine
{
  typedef std::vector<System*> Systems;

  class EngineSystem
  {
  public:
    EngineSystem();
    ~EngineSystem();
  
    void Update();

    void Add_System(System* sys) {
      systems.push_back(sys);
      sys->Init();
    }

    void System_Restart();
    
  private:
    Systems systems;
  
    float dt;
  };
  
  EngineSystem::EngineSystem() : dt(1 / 60.f)
  {
    Add_System(Graphics::Init());
    Add_System(Graphics::Shaders::Init());
    Add_System(Graphics::MeshLibrary::Init());
    Add_System(Graphics::TextureLibrary::Init());
    Add_System(ModelLibrary::Init());
    Add_System(Input::Init());
    Add_System(ObjectFactory::Init());
    Add_System(Graphics::Camera::Init());
    Add_System(SceneManager::Init());
    Add_System(Editor::Init());
    //Add_System(Collision::Init()); 


  }

  EngineSystem::~EngineSystem()
  {

  }
  
  
  void EngineSystem::Update()
  {
    while (Graphics::Running())
    {
      Time::ScopeTimer timer(&dt);

      //if (!Graphics::Focus())
      //{
      //  continue;
      //}
      
      for (Systems::iterator it = systems.begin(); it != systems.end(); ++it)
      {
        (*it)->Update(dt);
        (*it)->Draw();
      }

      Graphics::End_Frame();
      Input::Reset();
    }
  }

  void EngineSystem::System_Restart()
  {
    for (Systems::iterator it = systems.begin(); it != systems.end(); ++it)
    {
      (*it)->Shutdown();
    }

    for (Systems::iterator it = systems.begin(); it != systems.end(); ++it)
    {
      (*it)->Init();
    }
  }

  
  /*************************************************************/
  /*                        ECS Instance                       */
  /*************************************************************/
  
  static EngineSystem *Engine = nullptr;

  void Init()
  {
    static EngineSystem engine;
    Engine = &engine;
  }

  void Update()
  {
    Engine->Update();
  }

  void Shutdown()
  {

  }

  void System_Restart()
  {
    Engine->System_Restart();
  }


}