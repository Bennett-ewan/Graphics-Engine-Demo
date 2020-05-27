#pragma once


class System;



namespace Collision
{
  System* Init();
  void Update(float dt);
  void Shutdown();

  //reactphysics3d::CollisionWorld& Collision_World();
  reactphysics3d::DynamicsWorld* Dynamics_World();
}