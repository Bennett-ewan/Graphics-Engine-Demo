#include <stdafx.h>

#include <Core/System.h>
#include <Objects/ModelLibrary.h>
#include <Components/Model.h>

  namespace ModelLibrary
  {
    class ModelSystem : public System
    {
    public:
      ModelSystem() : models() {
        std::string path = "./Assets/Models";
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
          if (entry.is_directory())
          {
            std::string name = entry.path().filename().generic_string();
            models.emplace(std::make_pair(name, nullptr));
          }
        }
      }

      ~ModelSystem() {

      }

      void Init() {

      }

      void Update(float dt) {

      }

      void Draw() {

      }

      void Shutdown() {

      }

      const Models& Get_Models() const {
        return models;
      }

      const Model& Get_Model(const std::string& name) {

        Model* sprite = models.at(name);
        if (!sprite)
        {
          std::string path("./Assets/Models/" + name);
          for (const auto& dir : std::filesystem::directory_iterator(path))
          {
            if (dir.path().generic_string().find(".obj") != std::string::npos)
            {
              sprite = new Model(dir.path().generic_string());
              models[name] = sprite;
              continue;
            }
          }
        }
        return *sprite;
      }

      

    private:
      Models models;
    };
  


    /*************************************************************/
    /*                        ECS Instance                       */
    /*************************************************************/

    static ModelSystem* Model_System = nullptr;

    System* Init()
    {
      static ModelSystem model;
      Model_System = &model;
      return Model_System;
    }


    const Models& Get_Models()
    {
      return Model_System->Get_Models();
    }

    const Model& Get_Model(const std::string& name) 
    {
      return Model_System->Get_Model(name);;
    }

    /*************************************************************/
    /*                   Private functions                       */
    /*************************************************************/

  }