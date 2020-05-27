#pragma once

// forward declarations
class GameObject;
class Skybox;
namespace Behaviors { class Lua; }
namespace Graphics { class Light; class PointLight; class DirectionalLight; }

//typedefs
typedef std::vector<GameObject*> Objects;
typedef std::vector<Graphics::Light*> SceneLights;


//structs
struct LuaFunctions
{
  sol::protected_function load;
  sol::protected_function init;
  sol::protected_function update;
  sol::protected_function draw;
  sol::protected_function shutdown;
  sol::protected_function unload;
};

class Scene
{
public:
  Scene(const std::string& name);
  ~Scene();

  virtual void Load();
  virtual void Init();
  virtual void Update(float dt);
  virtual void Draw();
  virtual void Shutdown();
  virtual void Unload();

  const std::string& Name() const { return name; }

  void Add_Object(GameObject* obj) { objects.push_back(obj); }

  const Objects& Get_Objects() const { return objects; }

  const std::vector<Graphics::Light*>& Lights() const { return lights; }
  void Add_Light(Graphics::Light* const light);
  void Remove_Light(unsigned index);
  void Remove_Light(Graphics::Light* light);

  Skybox* Get_Skybox() const { return skybox; }
  void Set_Skybox(Skybox* skybox) { this->skybox = skybox; }

  void Kill_Objects();

  void Save_Data();
  void Load_Data();
protected:
  std::string name;
  Objects objects;
  Skybox* skybox;
  SceneLights lights;
  unsigned num_point_lights, num_dir_lights;

  std::map<float, std::pair<GameObject*, unsigned>> sorted_draw;
  std::vector<unsigned> to_destroy;

  GLuint depthMapFBO;
  unsigned int depthMap;


  Behaviors::Lua* lua;
  LuaFunctions functions;


  void init_lua();
  void init_shadows();

  friend class Camera;
};