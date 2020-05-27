#include <stdafx.h>

#include <Physics/Collision.h>
#include <Core/System.h>


namespace Collision
{
  using namespace reactphysics3d;

  class CollisionSystem : public System
  {
  public:

    CollisionSystem() : dynamics_world(nullptr) {
      dynamics_world = new DynamicsWorld(Vector3(0.f, 0.f, -9.81f));
    }


    ~CollisionSystem() {
      //delete dynamics_world;
    }

    void Init();
    void Update(float dt);
    void Draw();
    void Shutdown();


    DynamicsWorld* Dynamics_World() {
      return dynamics_world;
    }

  private:
    DynamicsWorld* dynamics_world;
  };


  void CollisionSystem::Init()
  {
    
  }

  void CollisionSystem::Update(float dt)
  {
    //dynamics_world->update(dt);
  }
  
  void CollisionSystem::Shutdown()
  {
    
  }


  void CollisionSystem::Draw()
  {

  }

  /*************************************************************/
  /*                        ECS Instance                       */
  /*************************************************************/

  static CollisionSystem* Collision_System = nullptr;

  System* Init()
  {
    static CollisionSystem coll_sys;
    Collision_System = &coll_sys;
    return Collision_System;
  }

  void Update(float dt)
  {
    Collision_System->Update(dt);
  }

  void Shutdown()
  {
    Collision_System->Shutdown();
  }


  DynamicsWorld* Dynamics_World()
  {
    if (!Collision_System)
    {
      Init();
      Collision_System->Init();
    }
    return Collision_System->Dynamics_World();
  }


}
