#include <stdafx.h>

#include <Objects/ObjectFactory.h>
#include <Objects/GameObject.h>
#include <Core/System.h>

#include <Components/Physics.h>
#include <Components/Transform.h>
#include <Components/Model.h>
#include <Components/Collider.h>


namespace ObjectFactory
{
  class Factory : public System
  {
  public:
    Factory() : archetypes_() {
      Read_Archetypes();
    }

    ~Factory() {
      Write_Archetypes();
    }

    void Init() {
    }
    
    void Update(float dt) {}

    void Shutdown() {}

    GameObject* Load(const std::string& name) {
      if (archetypes_.find(name) == archetypes_.end())
      {
        GameObject* obj = load_from_file(name);
        if (!obj) return nullptr;
        obj->Active(false);
        archetypes_[name] = *obj;
        return obj;
      }
      else
        return &archetypes_.at(name);
    }

    GameObject* Create(const std::string& name) {
      GameObject* arch = nullptr;
      if (archetypes_.find(name) == archetypes_.end())
      {
        arch = Load(name);
      }
      else
        arch = &archetypes_.at(name);
      if (!arch) return nullptr;
      GameObject* obj = new GameObject(*arch);
      obj->Active(true);
      return obj;
    }

    GameObject* New_Archetype(const std::string& name) {
      archetypes_.emplace(std::pair(name, name));
      return &archetypes_[name];
    }

    Archetypes& Get_Archetypes() {
      return archetypes_;
    }

    void Save_Archetype(GameObject* object)
    {
      Archetypes::iterator it = archetypes_.find(object->Name());
      if (it != archetypes_.end())
      {
        //delete it->second;
        //GameObject* obj = new GameObject(*object);
        //it->second = obj;
      }
      else
        std::cout << "ObjectFactory::Save_Archetype:: Unable to find archetype: " << object->Name() << std::endl;
    }

    void Write_Archetypes()
    {
      std::string path("./Assets/Data/Objects/archetypes");
      std::remove(path.c_str());
      std::ofstream outfile(path);
      if (!outfile.is_open())
      {
        std::cout << "Unable to open file to write archetypes\n";
        return;
      }
      size_t num = archetypes_.size();
      outfile.write(reinterpret_cast<const char*>(&num), sizeof(size_t));
      for (Archetypes::iterator it = archetypes_.begin(); it != archetypes_.end(); ++it)
      {
        it->second.Write(outfile);
      }
    }

    void Read_Archetypes()
    {
      std::string path("./Assets/Data/Objects/archetypes");
      std::fstream infile(path);
      if (!infile.is_open())
        return;

      size_t num = 0;
      infile.read(reinterpret_cast<char*>(&num), sizeof(size_t));
      for (unsigned i = 0; i < num; ++i)
      {
        //GameObject* object = new GameObject();
        size_t name_size = 0;
        infile.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
        std::string name;
        name.reserve(name_size);
        infile.read(&name[0], name_size);
        name = std::string(name.c_str(), name_size);
        archetypes_.emplace(name, name);
        GameObject* object = &archetypes_[name];
        object->Read(infile);
       // archetypes_[object->Name()] = object;
      }
    }

    void Remove_Archetype(GameObject* object)
    {
      for (Archetypes::iterator it = archetypes_.begin(); it != archetypes_.end(); ++it)
      {
        if (&(it->second) == object) {
          archetypes_.erase(it);
          return;
        }
      }
    }

  private:
    Archetypes archetypes_;

    GameObject* load_from_file(const std::string& name) {
      std::string path("./Assets/Data/Objects/" + name + ".json");

      std::string src = "";
      std::ifstream file(path);
      if (!file.is_open())
      {
        std::cout << "Cannot open object file: " << name << std::endl;
        return nullptr;
      }

      while (!file.eof())
      {
        std::string line;
        file >> line;
        src += line;
        src += "\n";
      }
      file.close();


      GameObject* object = new GameObject(name);

      rapidjson::Document document;
      document.Parse(src.c_str());

      if (document.HasParseError())
      {
        std::cout << "Parse error on object: " << name << std::endl;
        return nullptr;
      }

      if (document.HasMember("Transform"))
      {
        Transform* transform = new Transform();
        transform->Desearialize(document);
        object->Add_Component(transform);
      }

      if (document.HasMember("Sprite"))
      {
        Model* model = new Model();
        model->Desearialize(document);
        object->Add_Component(model);
      }

      if (document.HasMember("Physics"))
      {
        Physics* physics = new Physics();
        physics->Desearialize(document);
        object->Add_Component(physics);
      }

      if (document.HasMember("Collider"))
      {
        Collider* collider = new Collider(object->Get(Transform));
        object->Add_Component(collider);
        collider->Desearialize(document);
      }
      return object;
    }

  };




  /*************************************************************/
  /*                        ECS Instance                       */
  /*************************************************************/

  static Factory* Object_Factory = nullptr;

  System* Init()
  {
    static Factory factory;
    Object_Factory = &factory;
    return Object_Factory;
  }

  void Update(float dt)
  {
    Object_Factory->Update(dt);
  }

  void Shutdown()
  {
    Object_Factory->Shutdown();
  }

  GameObject* Load(const std::string& name)
  {
    return Object_Factory->Load(name);
  }

  GameObject* Create(const std::string& name)
  {
    return Object_Factory->Create(name);
  }


  std::fstream Blank_File(const std::string& name)
  {
    std::string path("./Assets/Data/Objects/" + name);

    std::fstream file(path);

    if (file.is_open())
    {
      file << "{\n";
      file << "  \"Name\": \"" << name << "\",\n";
      file << "}";
    }

    return file;
  }

  GameObject* New_Archetype(const std::string& name)
  {
    return Object_Factory->New_Archetype(name);
  }

  Archetypes& Get_Archetypes()
  {
    return Object_Factory->Get_Archetypes();
  }

  void Save_Archetype(GameObject* object)
  {
    Object_Factory->Save_Archetype(object);
  }

  void Remove_Archetype(GameObject* object)
  {
    Object_Factory->Remove_Archetype(object);
  }

}