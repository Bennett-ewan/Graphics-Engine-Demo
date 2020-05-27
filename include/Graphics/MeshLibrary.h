#pragma once

class System;

namespace Graphics
{
  class Mesh;
  struct Vertex;

  namespace MeshLibrary
  {
    System* Init();

    void Add_Mesh(const std::string& name, Mesh* mesh);
    Mesh* Add_Mesh(const std::string& name, const std::vector<Vertex>& vert, const std::vector<unsigned>& ind);
    bool Has_Mesh(const std::string& name);
    Mesh* Get_Mesh(const std::string& name);
  }
  
}
