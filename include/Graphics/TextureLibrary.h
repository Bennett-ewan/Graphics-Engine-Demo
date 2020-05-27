#pragma once


class System;

namespace Graphics
{
  class Texture;

  namespace TextureLibrary
  {
    System* Init();
    void Update(float dt);
    void Shutdown();

    Texture* Get_Texture(const std::string& name);
    const std::vector<Texture*>& Get_Textures();

    void Add_Texture(Texture* texture);
    void Remove_Texture(Texture* texture);
  }
}