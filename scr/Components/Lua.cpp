#include <stdafx.h>

#include <Components/Lua.h>

//Types
#include <Objects/GameObject.h>
#include <Components/Transform.h>
#include <Components/Physics.h>
#include <Components/Model.h>

#include <Scenes/Scene.h>

//Systems
#include <Scenes/SceneManager.h>
#include <Scenes/Editor.h>
#include <Objects/ObjectFactory.h>

namespace Behaviors
{
 
  // Private function declarations
  glm::vec2 vector2_add(const glm::vec2& v0, const glm::vec2& v1);
  glm::vec3 vector3_add(const glm::vec3& v0, const glm::vec3& v1);
  
  Lua::Lua(const std::string& script) : script_name(script)
  {
    state.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os, sol::lib::coroutine);
    state.create_table("Objects");
  }
  
  Lua::~Lua()
  {
    state.collect_garbage();
  }
  
  void Lua::Initialize()
  {
    set_usertypes();
    set_engine_data();
  }

  void Lua::Run()
  {
    try {
      sol::protected_function_result result = state.script_file(std::string(script_name + ".lua"));
    }
    catch (sol::error & err) {
      std::cout << "Error loading lua script: " << script_name << std::endl <<  "Error: " << err.what() << std::endl;
    }
  }
  
  
  //void Lua::Add_Object(GameObject* obj, const std::string& name)
  //{
  //  state["Objects"][obj->Name()] = obj;
  //
  //  state[name.c_str()]["Transform"] = obj->Get(Transform);
  //  state[name.c_str()]["Physics"] = obj->Get(Physics);
  //  state[name.c_str()]["Sprite"] = obj->Get(Sprite);
  //  //state[name.c_str()] = state.create_table_with(
  //  //  "Transform", obj->Get(Transform),
  //  //  "Physics", obj->Get(Physics),
  //  //  "Sprite", obj->Get(Sprite)
  //  //);
  //}

  void Lua::Add_Object(sol::userdata obj)
  {
    GameObject* object = obj.as<GameObject*>();
    sol::usertype<GameObject> obj_type = state.new_usertype<GameObject>("GameObject");
    //if (object->Has_Component(Component::ComponentType::cmp_Transform))
    //{
    //  obj_type["Transform"] = sol::var(object->Get(Transform));
    //  //trans.set("Transform", object->Get(Transform));
    //}
    //if (object->Has_Component(Component::ComponentType::cmp_Physics))
    //  obj["Physics"] = object->Get(Physics);
    //if (object->Has_Component(Component::ComponentType::cmp_Sprite))
    //  obj["Sprite"] = object->Get(Sprite);
  }
  
  void Lua::set_engine_data()
  {
    state.set_function("Add_Lua_Object", &Lua::Add_Object, this);

    void (*nextscene_1)(Scene*) = &SceneManager::Next_Scene;
    void (*nextscene_2)(const std::string&) = &SceneManager::Next_Scene;
    state["SceneManager"] = state.create_table_with(
      "Restart", &SceneManager::Restart,
      "Next_Scene", sol::overload(nextscene_1, nextscene_2),
      "Quit", &SceneManager::Quit,
      "Current_Scene", &SceneManager::Current_Scene,
      "Previous_Scene", &SceneManager::Previous_Scene
    );

  
    state["ObjectFactory"] = state.create_table_with(
      "Create", &ObjectFactory::Create,
      "Load", &ObjectFactory::Load
    );


    state["Components"] = state.create_table_with(
      "Transform", Component::ComponentType::cmp_Transform,
      "Sprite", Component::ComponentType::cmp_Model,
      "Physics", Component::ComponentType::cmp_Physics
    );

    GameObject* (*g_selectobj)() = &Editor::Selected_Object;
    state["Editor"] = state.create_table_with(
      "Scene_Selector", &Editor::Scene_Selector,
      "Enable_Editor", &Editor::Enable_Editor,
      "Archetype_Editor", &Editor::Archetype_Editor,
      "Selected_Object", g_selectobj
    );
  }
  
  
  void Lua::set_usertypes()
  {
    // Vectors
    sol::usertype<glm::vec2> vector2_type = state.new_usertype<glm::vec2>("Vec2", sol::constructors<glm::vec2(), glm::vec2(float, float), glm::vec2(glm::vec2&)>());
    vector2_type["x"] = sol::property(&glm::vec2::x, &glm::vec2::x);
    vector2_type["y"] = sol::property(&glm::vec2::y, &glm::vec2::y);
    state.set_function("Vec2_Add", &vector2_add);
  
    sol::usertype<glm::vec3> vector3_type = state.new_usertype<glm::vec3>("Vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float), glm::vec3(glm::vec3&)>());
    vector3_type["x"] = sol::property(&glm::vec3::x, &glm::vec3::x);
    vector3_type["y"] = sol::property(&glm::vec3::y, &glm::vec3::y);
    vector3_type["z"] = sol::property(&glm::vec3::z, &glm::vec3::z);
    state.set_function("Vec3_Add", &vector3_add);
  
    // GameObject
    sol::usertype<GameObject> object_type = state.new_usertype<GameObject>("GameObject", "Get_Component", &GameObject::Get_Component<Transform>);
    //sol::usertype<GameObject> object_type = state.new_usertype<GameObject>("GameObject");
    
    void (GameObject:: * sObjName)(const std::string&) = &GameObject::Name;
    object_type["Name"] = sObjName;
    void (GameObject:: * sObjActive)(const bool&) = &GameObject::Active;
    const bool& (GameObject:: * gObjActive)() const = &GameObject::Active;
    object_type["Active"] = sol::overload(sObjActive, gObjActive);
    object_type["Tick"] = &GameObject::Tick;
    object_type["Draw"] = &GameObject::Draw;
    //object_type["Transform"] = state.create_table();
    //object_type["Physics"] = state.create_table();
    //object_type["Sprite"] = state.create_table();
  
    // Component
    sol::usertype<Component> component_type = state.new_usertype<Component>("Component");
    GameObject * (Component::*gParent)() const = &Component::Parent;
    component_type["Parent"] = gParent;
  
    // Transform
    sol::usertype<Transform> transform_type = state.new_usertype<Transform>("Transform", sol::base_classes, sol::bases<Component>());
    const glm::vec3& (Transform:: * gTranslation) ()const = &Transform::Translation;
    void (Transform:: * sTranslation)(const glm::vec3&) = &Transform::Translation;
    transform_type["Translation"] = sol::overload(gTranslation, sTranslation);
  
    const glm::vec3& (Transform:: * gScale) ()const = &Transform::Scale;
    void (Transform:: * sScale)(const glm::vec3&) = &Transform::Scale;
    transform_type["Scale"] = sol::overload(gScale, sScale);
  
    const float& (Transform:: * gRotationX)() const = &Transform::RotationX;
    void (Transform:: * sRotationX)(const float&) = &Transform::RotationX;
    transform_type["RotationX"] = sol::overload(gRotationX, sRotationX);
    const float& (Transform:: * gRotationY)() const = &Transform::RotationY;
    void (Transform:: * sRotationY)(const float&) = &Transform::RotationY;
    transform_type["RotationY"] = sol::overload(gRotationY, sRotationY);
    const float& (Transform:: * gRotationZ)() const = &Transform::RotationZ;
    void (Transform:: * sRotationZ)(const float&) = &Transform::RotationZ;
    transform_type["RotationZ"] = sol::overload(gRotationZ, sRotationZ);
  
    // Physics
    sol::usertype<Physics> physics_type = state.new_usertype<Physics>("Physics", sol::base_classes, sol::bases<Component>());
    const glm::vec3& (Physics:: * gVelocity) ()const = &Physics::Velocity;
    void (Physics:: * sVelocity)(const glm::vec3&) = &Physics::Velocity;
    physics_type["Velocity"] = sol::overload(gVelocity, sVelocity);
  
    physics_type["Add_Force"] = &Physics::Add_Force;
  
    // Sprite
    sol::usertype<Model> sprite_type = state.new_usertype<Model>("Model", sol::base_classes, sol::bases<Component>());
  
  
    // Scene
    sol::usertype<Scene> scene_type = state.new_usertype<Scene>("Scene");
    scene_type["Add_Object"] = &Scene::Add_Object;
  }
  
  
  glm::vec2 vector2_add(const glm::vec2& v0, const glm::vec2& v1)
  {
    return v0 + v1;
  }
  
  glm::vec3 vector3_add(const glm::vec3& v0, const glm::vec3& v1)
  {
    return v0 + v1;
  }
}




