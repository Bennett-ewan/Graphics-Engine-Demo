#pragma once

class System
{
public:
  System() {};
  virtual ~System() {};

  virtual void Init() = 0;
  virtual void Update(float dt) = 0;
  virtual void Draw() {}
  virtual void Shutdown() = 0;


};