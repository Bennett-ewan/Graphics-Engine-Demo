#include <stdafx.h>

#include <Core\System.h>
#include <Graphics\ShaderSystem.h>
#include <Graphics/Shader.h>

namespace Graphics
{
  namespace Shaders
  {


    class ShaderSystem : public System
    {
      


    public:
      ShaderSystem();
      ~ShaderSystem();

      void Init();

      void Update(float dt);

      void Draw();

      void Shutdown();

      GLuint Core_Program() { return shaders["Core"]->Get_ID(); }

      void Add_Shader(const std::string& name, const std::string& vertex, const std::string& fragment)
      {
        Shader* shader = new Shader(vertex, fragment);
        shader->Name(name);
        shaders[name] = shader;
      }

      Shader* Get_Shader(const std::string& name) const {
        try {
          return shaders.at(name);
        }
        catch (...)
        {
          return nullptr;
        }
      }

    private:
      std::map<std::string, Shader*> shaders;
    };

    ShaderSystem::ShaderSystem()
    {
      Add_Shader("Core", "./Assets/Shaders/vertex_core.glsl", "./Assets/Shaders/fragment_core.glsl");
      Add_Shader("Editor", "./Assets/Shaders/vertex_editor.glsl", "./Assets/Shaders/fragment_editor.glsl");
      Add_Shader("Wireframe", "./Assets/Shaders/vertex_wireframe.glsl", "./Assets/Shaders/fragment_wireframe.glsl");
      Add_Shader("Outline", "./Assets/Shaders/vertex_outline.glsl", "./Assets/Shaders/fragment_outline.glsl");
      Add_Shader("Skybox", "./Assets/Shaders/vertex_skybox.glsl", "./Assets/Shaders/fragment_skybox.glsl");
      Add_Shader("Shadow", "./Assets/Shaders/vertex_shadow.glsl", "./Assets/Shaders/fragment_shadow.glsl");
    }
    
    ShaderSystem::~ShaderSystem()
    {

    }

    void ShaderSystem::Init()
    {

    }

    void ShaderSystem::Update(float dt)
    {
      
    }

    void ShaderSystem::Draw()
    {
      
    }

    void ShaderSystem::Shutdown()
    {

    }


    /*************************************************************/
    /*                   Private functions                       */
    /*************************************************************/


    /*************************************************************/
    /*                        ECS Instance                       */
    /*************************************************************/

    static ShaderSystem* Shader_System = nullptr;

    System* Init()
    {
      static ShaderSystem shader;
      Shader_System = &shader;
      return Shader_System;
    }

    void Update(float dt)
    {
      if (Shader_System)
        Shader_System->Update(dt);
    }

    void Shutdown()
    {
      if (Shader_System)
        Shader_System->Shutdown();
    }

    void Add_Shader(const std::string& name, const std::string& vertex, const std::string& fragment)
    {
      if (Shader_System)
        Shader_System->Add_Shader(name, vertex, fragment);
    }


    Shader* Get_Shader(const std::string& name)
    {
      return Shader_System->Get_Shader(name);
    }
  }
}
