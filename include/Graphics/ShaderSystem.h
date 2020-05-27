#pragma once

class System;

namespace Graphics
{
  class Shader;
  namespace Shaders
  {
    System* Init();
    void Update(float dt);
    void Shutdown();

    void Add_Shader(const std::string& name, const std::string& vertex, const std::string& fragment);
    Graphics::Shader* Get_Shader(const std::string& name);
  }
  
}