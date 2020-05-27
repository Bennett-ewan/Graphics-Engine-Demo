#include <stdafx.h>

#include <Graphics/MeshLibrary.h>
#include <Core/System.h>
#include <Graphics/Mesh.h>
#include <Objects/Primitives.h>

namespace Graphics
{
  namespace MeshLibrary
  {
    class MeshSystem : public System
    {
    public:
      MeshSystem() : meshes_(), vertices_(), indices_()
      {
        Shapes::Triangle triangle;
        Add_Mesh("Triangle", triangle.Get_Vertices(), triangle.Get_Indices());
        Shapes::Quad quad;
        Add_Mesh("Quad", quad.Get_Vertices(), quad.Get_Indices());
        Shapes::Pyramid pyramid;
        Add_Mesh("Pyramid", pyramid.Get_Vertices(), pyramid.Get_Indices());
        Shapes::Cube cube;
        Add_Mesh("Cube", cube.Get_Vertices(), cube.Get_Indices());
        Shapes::Sphere sphere;
        Add_Mesh("Sphere", sphere.Get_Vertices(), sphere.Get_Indices());
      }

      ~MeshSystem()
      {
        for (std::map<std::string, Mesh*>::iterator it = meshes_.begin(); it != meshes_.end(); ++it)
          delete it->second;
      }

      void Init() {}

      void Update(float dt) {}

      void Shutdown() {}

      void Add_Mesh(const std::string& name, Mesh* mesh) {
        meshes_[name] = mesh;
        vertices_[name] = mesh->Get_Vertices();
        indices_[name] = mesh->Get_Indices();
        mesh->Name(name);
      }

      Mesh* Add_Mesh(const std::string& name, const std::vector<Vertex>& vert, const std::vector<unsigned>& ind)
      {
        vertices_[name] = vert;
        indices_[name] = ind;
        Mesh* mesh = new Mesh(vertices_[name], indices_[name]);
        meshes_[name] = mesh;
        mesh->Name(name);
        return mesh;
      }

      bool Has_Mesh(const std::string& name) {
        return meshes_.find(name) != meshes_.end();
      }

      Mesh* Get_Mesh(const std::string& name) {
        try { 
          return meshes_.at(name); 
        } 
        catch (...) { return nullptr; }
      }


    private:
      std::map<std::string, Mesh*> meshes_;
      std::map<std::string, std::vector<Vertex>> vertices_;
      std::map<std::string, std::vector<unsigned>> indices_;
    };

    static MeshSystem* Mesh_Manager;

    System* Init()
    {
      static MeshSystem mesh_sys;
      Mesh_Manager = &mesh_sys;
      return Mesh_Manager;
    }

    void Add_Mesh(const std::string& name, Mesh* mesh) 
    {
      Mesh_Manager->Add_Mesh(name, mesh);
    }

    Mesh* Add_Mesh(const std::string& name, const std::vector<Vertex>& vert, const std::vector<unsigned>& ind)
    {
      return Mesh_Manager->Add_Mesh(name, vert, ind);
    }

    bool Has_Mesh(const std::string& name) 
    {
      return Mesh_Manager->Has_Mesh(name);
    }

    Mesh* Get_Mesh(const std::string& name) 
    {
      return Mesh_Manager->Get_Mesh(name);
    }
  }
}