#include <stdafx.h>

#include <Scenes/Scene.h>
#include <Scenes/Skybox.h>
#include <Objects/GameObject.h>
#include <Graphics/Mesh.h>
#include <Graphics/Texture.h>

#include <Components/Model.h>
#include <Components/Transform.h>
#include <Graphics/Material.h>
#include <Objects/Primitives.h>

#include <Graphics/Camera.h>
#include <Input/InputSystem.h>

#include <Graphics/GraphicsSystem.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>

#include <Graphics/Light.h>
#include <Graphics/Material.h>

#include <Objects/ObjectFactory.h>

#include <Components/Lua.h>

static GameObject* test_obj;
static GameObject* pyramid;

Scene::Scene(const std::string& name) : name(name), objects(), skybox(nullptr), lights(),
                                        num_point_lights(0), num_dir_lights(0), sorted_draw(), to_destroy()
{

}

Scene::~Scene()
{

}

void Scene::Load()
{
  Load_Data();

  if (lights.size() == 0)
    Add_Light(new Graphics::DirectionalLight(glm::vec3(0.f, -1.f, 0.f)));

  init_lua();
}




void Scene::Init()
{
  sol::protected_function_result result = functions.init();
  if (!result.valid())
  {
    sol::error err = result;
    std::cout << "Failed to run init on scene: " << name << std::endl << "Error: " << err.what() << std::endl;
  }
  
}

void Scene::Update(float dt)
{
  sol::protected_function_result result = functions.update(this, dt);
  if (!result.valid())
  {
    sol::error err = result;
    std::cout << "Failed to run update on scene: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  Graphics::Shader* shader = Graphics::Shaders::Get_Shader("Core");
  Graphics::Shader* shadow_shader = Graphics::Shaders::Get_Shader("Shadow");
  int point_num = 0; int dir_num = 0;
  for (unsigned i = 0; i < lights.size(); ++i)
  {
    if (lights[i]->Get_Type() == Graphics::Light::Type::Point)
    {
      lights[i]->Send(*shader, point_num);
      lights[i]->Send(*shadow_shader, point_num++);
    }
    else
    {
      lights[i]->Send(*shader, dir_num);
      lights[i]->Send(*shadow_shader, dir_num++);
    }
  }



  Graphics::Camera::Set_Uniforms(shader);


  int count = 0;
  for (Objects::iterator it = objects.begin(); it != objects.end(); ++it)
  {
    Transform* transform = (*it)->Get(Transform);
    if (transform)
    {
      float distance = glm::dot(Graphics::Camera::Eye(), transform->Translation());
      sorted_draw[distance] = std::make_pair(*it, count);
    }
    const std::vector<Graphics::Light*>& obj_lights = (*it)->Get_Lights();
    shader->Use();
    shader->Set_Uniform((int)(num_point_lights + obj_lights.size()), "num_point_lights");
    shader->Set_Uniform((int)num_dir_lights, "num_dir_lights");
    shader->Unuse();
    for (unsigned i = 0; i < obj_lights.size(); ++i)
    {
      obj_lights[i]->Send(*shader, point_num);
      obj_lights[i]->Send(*shadow_shader, point_num++);
    }
    (*it)->Tick(dt);
    ++count;
  }
}

void Scene::Draw()
{
  sol::protected_function_result result = functions.draw();
  if (!result.valid())
  {
    sol::error err = result;
    std::cout << "Failed to run draw on scene: " << name << std::endl << "Error: " << err.what() << std::endl;
  }


  Graphics::Shader* shader = Graphics::Shaders::Get_Shader("Core");
  Graphics::Shader* shadow_shader = Graphics::Shaders::Get_Shader("Shadow");

  if (skybox)
  {
    skybox->Draw();
  }

  //shadow_shader->Use();
  //shadow_shader->Set_Uniform((int)20, "shadowMap");
  //if (lights.size() > 0)
  //{
  //  shadow_shader->Set_Uniform(lights[0]->Matrix(), "lightSpaceMatrix");
  //}
  //int width = Graphics::Width(), height = Graphics::Height();
  //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  //glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  //glClear(GL_DEPTH_BUFFER_BIT);
  ////glActiveTexture(GL_TEXTURE0);
  ////glBindTexture(GL_TEXTURE_2D, depthMap);
  //
  //for (std::map<float, std::pair<GameObject*, unsigned>>::reverse_iterator it = sorted_draw.rbegin(); it != sorted_draw.rend(); ++it)
  //{
  //  Model* model = it->second.first->Get(Model);
  //  Graphics::Shader* prev_shader = model->Shader();
  //  model->Shader(shadow_shader);
  //  it->second.first->Draw();
  //  model->Shader(prev_shader);
  //}
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, width, height);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




  shader->Use();
  //shader->Set_Uniform((int)20, "shadowMap");
  if (lights.size() > 0)
  {
    shader->Set_Uniform(lights[0]->Matrix(), "lightSpaceMatrix");
  }

  for (std::map<float, std::pair<GameObject*, unsigned>>::reverse_iterator it = sorted_draw.rbegin(); it != sorted_draw.rend(); ++it)
  {
    //glActiveTexture(GL_TEXTURE20);
    //glBindTexture(GL_TEXTURE_2D, depthMap);
    it->second.first->Draw();
    if (it->second.first->Is_Destroy())
      to_destroy.push_back(it->second.second);
  }
  sorted_draw.clear();


}

void Scene::Shutdown()
{
  sol::protected_function_result result = functions.shutdown();
  if (!result.valid()) {
    sol::error err = result;
    std::cout << "Failed to run shutdown on scene: " << name << std::endl << "Error: " << err.what() << std::endl;
  }
}

void Scene::Unload()
{
  Save_Data();

  num_point_lights = 0;
  num_dir_lights = 0;
  for (Objects::iterator it = objects.begin(); it != objects.end(); ++it)
    delete* it;
  objects.clear();
  delete skybox;
  skybox = nullptr;
  for (unsigned i = 0; i < lights.size(); ++i)
    delete lights[i];
  lights.clear();
  sol::protected_function_result result = functions.unload();
  if (!result.valid()) {
    sol::error err = result;
    std::cout << "Failed to run unload on scene: " << name << std::endl << "Error: " << err.what() << std::endl;
  }
  glDeleteFramebuffers(1, &depthMapFBO);
  glDeleteTextures(1, &depthMap);
}


void Scene::Kill_Objects()
{
  for (unsigned i = 0; i < to_destroy.size(); ++i)
  {
    unsigned index = to_destroy[i];
    std::vector<GameObject*>::iterator it = objects.begin() + index;
    delete *it;
    objects.erase(it);
  }
  to_destroy.clear();
}

void Scene::Add_Light(Graphics::Light* const light) 
{
  lights.push_back(light);
  if (light->Get_Type() == Graphics::Light::Type::Point)
    ++num_point_lights;
  else if (light->Get_Type() == Graphics::Light::Type::Direction)
    ++num_dir_lights;
}

void Scene::Remove_Light(unsigned index) 
{
  std::vector<Graphics::Light*>::iterator it = lights.begin() + index;
  if ((*it)->Get_Type() == Graphics::Light::Type::Point)
    --num_point_lights;
  else if ((*it)->Get_Type() == Graphics::Light::Type::Direction)
    --num_dir_lights;
  delete* it;
  lights.erase(it);
}

void Scene::Remove_Light(Graphics::Light* light)
{
  if (light->Get_Type() == Graphics::Light::Type::Point)
    --num_point_lights;
  else if (light->Get_Type() == Graphics::Light::Type::Direction)
    --num_dir_lights;
  for (SceneLights::iterator it = lights.begin(); it != lights.end(); ++it)
    if (*it == light)
    {
      lights.erase(it);
      return;
    }
}


void Scene::Load_Data()
{
  std::string path("./Assets/Data/Scenes/" + name);
  std::fstream infile(path);
  if (!infile.is_open())
    return;

  // camera
  Graphics::Camera::Read(infile);

  bool has_skybox = false;
  infile.read(reinterpret_cast<char*>(&has_skybox), sizeof(bool));
  if (has_skybox)
  {
    size_t dir_size = 0;
    infile.read(reinterpret_cast<char*>(&dir_size), sizeof(size_t));
    std::string skybox_dir;
    skybox_dir.resize(dir_size);
    infile.read(&skybox_dir[0], dir_size);
    skybox = new Skybox(std::string(skybox_dir.c_str(), dir_size));
  }
  size_t light_size = 0;
  infile.read(reinterpret_cast<char*>(&light_size), sizeof(size_t));
  for (unsigned i = 0; i < light_size; ++i)
  {
    Graphics::Light::Type type = Graphics::Light::Type::None;
    infile.read(reinterpret_cast<char*>(&type), sizeof(Graphics::Light::Type));
    switch (type)
    {
    case Graphics::Light::Type::Point:
    {
      Graphics::PointLight* light = new Graphics::PointLight;
      light->Read(infile);
      Add_Light(light);
      break;
    }
    case Graphics::Light::Type::Direction:
    {
      Graphics::DirectionalLight* light = new Graphics::DirectionalLight;
      light->Read(infile);
      Add_Light(light);
      break;
    }
    }
  }
  size_t num_obj = 0;
  infile.read(reinterpret_cast<char*>(&num_obj), sizeof(size_t));
  for (unsigned i = 0; i < num_obj; ++i)
  {
    size_t name_size = 0;
    infile.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
    std::string name;
    name.reserve(name_size);
    infile.read(&name[0], name_size);
    GameObject* object = new GameObject(std::string(name.c_str(), name_size));
    object->Read(infile);
    //if (object->Name() != "")
    objects.push_back(object);
  }
}


void Scene::Save_Data()
{
  std::string path("./Assets/Data/Scenes/" + name);
  std::remove(path.c_str());
  std::ofstream outfile(path);
  if (!outfile.is_open())
    return;

  // camera
  Graphics::Camera::Write(outfile);

  // skybox
  bool has_skybox = skybox ? true : false;
  outfile.write(reinterpret_cast<const char*>(&has_skybox), sizeof(bool));
  if (skybox) {
    const std::string& skybox_dir = skybox->Directory();
    size_t dir_size = skybox_dir.size();
    outfile.write(reinterpret_cast<const char*>(&dir_size), sizeof(size_t));
    outfile.write(skybox_dir.c_str(), dir_size);
  }

  // lights
  size_t light_size = lights.size();
  outfile.write(reinterpret_cast<const char*>(&light_size), sizeof(size_t));
  for (unsigned i = 0; i < lights.size(); ++i)
  {
    lights[i]->Write(outfile);
  }

  // objects
  size_t num_obj = objects.size();
  outfile.write(reinterpret_cast<const char*>(&num_obj), sizeof(size_t));
  for (unsigned i = 0; i < num_obj; ++i)
  {
    objects[i]->Write(outfile);
  }
}


void Scene::init_lua()
{
  std::string path("./Assets/Scripts/Scenes/" + name);

  lua = new Behaviors::Lua(path);
  lua->Initialize();
  lua->state["This"] = lua->state.create_table_with("Scene", this);
  lua->state.create_named_table(name);

  lua->Run();

  try {
    functions.load = lua->state[name.c_str()]["Load"];
  }
  catch (sol::error& err) {
    std::cout << "Scene load lua error: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  try {
    functions.init = lua->state[name.c_str()]["Init"];
  }
  catch (sol::error& err) {
    std::cout << "Scene init lua error: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  try {
    functions.update = lua->state[name.c_str()]["Update"];
  }
  catch (sol::error& err) {
    std::cout << "Scene update lua error: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  try {
    functions.draw = lua->state[name.c_str()]["Draw"];
  }
  catch (sol::error& err) {
    std::cout << "Scene draw lua error: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  try {
    functions.shutdown = lua->state[name.c_str()]["Shutdown"];
  }
  catch (sol::error& err) {
    std::cout << "Scene shutdown lua error: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  try {
    functions.unload = lua->state[name.c_str()]["Unload"];
  }
  catch (sol::error& err) {
    std::cout << "Scene unload lua error: " << name << std::endl << "Error: " << err.what() << std::endl;
  }

  sol::protected_function_result result = functions.load();
  if (!result.valid()) {
    sol::error err = result;
    std::cout << "Failed to run load on scene: " << name << std::endl << "Error: " << err.what() << std::endl;
  }
}

void Scene::init_shadows()
{
  // Shadow framebuffer
  glGenFramebuffers(1, &depthMapFBO);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}