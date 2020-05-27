#include <stdafx.h>

#include <Core/System.h>

#include <Graphics/Texture.h>

namespace Graphics
{
  namespace TextureLibrary
  {
    class TextureSystem : public System
    {
    public:
      TextureSystem() : textures() {
        std::string path = "./Assets/Textures";
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
          Texture* texture = new Texture(entry.path().generic_string());
          textures.push_back(texture);
        }
      }

      ~TextureSystem() {

      }

      void Init() {

      }

      void Update(float dt) {

      }

      void Draw() {

      }

      void Shutdown() {

      }

      const std::vector<Texture*>& Get_Textures() const {
        return textures;
      }

      Texture* Get_Texture(const std::string& name) {
        for (unsigned i = 0; i < textures.size(); ++i)
        {
          if (textures[i]->Name() == name)
            return textures[i];
        }
        return nullptr;
      }

      void Add_Texture(Texture* texture)
      {
        textures.push_back(texture);
      }

      void Remove_Texture(Texture* texture)
      {
        for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
        {
          if (*it == texture)
          {
            delete *it;
            textures.erase(it);
            break;
          }
        }
      }

    private:
      std::vector<Texture*> textures;
    };
  


    /*************************************************************/
    /*                        ECS Instance                       */
    /*************************************************************/

    static TextureSystem* Texture_System = nullptr;

    System* Init()
    {
      static TextureSystem graphics;
      Texture_System = &graphics;
      return Texture_System;
    }

    void Update(float dt)
    {
      Texture_System->Update(dt);
    }

    void Shutdown()
    {
      Texture_System->Shutdown();
    }

    Texture* Get_Texture(const std::string& name)
    {
      return Texture_System->Get_Texture(name);
    }

    const std::vector<Texture*>& Get_Textures()
    {
      return Texture_System->Get_Textures();
    }

    void Add_Texture(Texture* texture)
    {
      Texture_System->Add_Texture(texture);
    }

    void Remove_Texture(Texture* texture)
    {
      Texture_System->Remove_Texture(texture);
    }

    /*************************************************************/
    /*                   Private functions                       */
    /*************************************************************/

  }



  
}
