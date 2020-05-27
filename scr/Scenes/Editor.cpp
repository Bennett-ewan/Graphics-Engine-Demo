#include <stdafx.h>

#include <Scenes/Editor.h>
#include <Core/System.h>

#include <Graphics/GraphicsSystem.h>
#include <Graphics/Light.h>
#include <Graphics/Camera.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>

#include <Scenes/SceneManager.h>
#include <Scenes/Scene.h>
#include <Scenes/Skybox.h>

#include <Objects/GameObject.h>
#include <Objects/ObjectFactory.h>
#include <Objects/Primitives.h>
#include <Objects/ModelLibrary.h>

#include <Components/Transform.h>
#include <Components/Model.h>
#include <Components/Physics.h>

#include <Graphics/Texture.h>
#include <Graphics/TextureLibrary.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshLibrary.h>

#include <Input/InputSystem.h>

namespace Editor 
{
  static void draw_selected_object(GameObject* obj);

  static void HelpMarker(const char* desc)
  {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
  }

  class EditorSystem : public System
  {
  public:
    EditorSystem() : show_object_editor(false), window_size(0.f, 0.f)
    {

    }

    ~EditorSystem() {
      
    }

    void Init() {
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO();

      ImGui_ImplGlfw_InitForOpenGL(Graphics::Window(), true);
      int version, version_minor;
      glfwGetVersion(&version, &version_minor, NULL);
      ImGui_ImplOpenGL3_Init("#version 330");
      ImGui::StyleColorsDark();

      file_browser = new ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);

      window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    }

    void Update(float dt);

    void Draw() {
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Shutdown() {
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext();

      delete file_browser;
    }

    void Object_Editor(bool archetype);

    void Selected_Object(GameObject* object) { selected_object = object; }
    GameObject* Selected_Object() { return selected_object; }

    void Archetype_Editor();
    void Scene_Selector();

    void Enable_Editor(bool enable) {
      active = enable;
    }

    bool unsaved_changes = false;
    bool show_save = false;
  private:
    ImGuiWindowFlags window_flags;
    bool active = true;
    bool new_object = false;
    bool show_object_editor;
    bool show_texture_library = false;
    bool show_archetype_selector = false;
    bool show_new_component = false;
    bool show_mesh_selector = false;
    bool show_new_scene = false;

    GameObject* selected_object;
    GameObject* copied_object;
    Scene* current_scene;
    glm::vec2 window_size;


    ImGui::FileBrowser* file_browser;
    void open_new_object();
    void scene_objects();
    void sprite_editor(bool archetype);
    void transform_editor(bool archetype);
    void texture_library(Graphics::Mesh* mesh, int index);
    GameObject* archetype_selector();
    void component_selector();
    void mesh_selector();
    void deselect_object();
    void save_popup();
    void edit_skybox(Skybox* skybox);
    void edit_camera();
    void light_editor();
    void open_new_scene();
    void edit_light(Graphics::Light* light);
  };


  void EditorSystem::Update(float dt)
  {
    window_size = glm::vec2(Graphics::Width(), Graphics::Height());
    if (!active)
      return;

    if (ImGui::Begin("Editor", nullptr, window_flags)) {
      ImGui::SetWindowFontScale(window_size.x / 1920.f);
      ImGui::SetWindowSize(ImVec2(window_size.x / 4.f, window_size.y / 2.f));
      ImGui::SetWindowPos(ImVec2(0.f, window_size.y / 60.f));

      current_scene = SceneManager::Current_Scene();

      if (ImGui::IsAnyWindowFocused()) {
        Input::Block_Input(true);
      }
      else
        Input::Block_Input(false);

      // FPS
      ImGui::Text("FPS: %f", Graphics::FPS());
      ImGui::NewLine();
      ImGui::Text("*** Ctrl + Click to input values on slider ***");
      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();

      ImGui::Text("Current Scene: %s", current_scene->Name().c_str());
      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();
      
      // Camera
      if (ImGui::TreeNode("Camera")){
        edit_camera();
        ImGui::TreePop();
      }

      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();

      // Light
      if (ImGui::TreeNode("Scene Lights")) {
        light_editor();
      }
      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();
     
      // Skybox
      if (ImGui::TreeNode("Skybox")) {
        Skybox* skybox = SceneManager::Current_Scene()->Get_Skybox();
        edit_skybox(skybox);
        ImGui::TreePop();
      }
      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();
      ImGui::SameLine(window_size.x / 13.f);

      // Archetype Editor
      if (SceneManager::Current_Scene()->Name() == "ArchetypeEditor") {
        ImGui::SameLine(window_size.x / 15.f);
        if (ImGui::Button("Exit Archetype Editor", ImVec2(window_size.x / 12.f, window_size.y / 18.f)))
        {
          SceneManager::Next_Scene(SceneManager::Previous_Scene());
        }
      }
      else
      {
        ImGui::SameLine(window_size.x / 15.f);
        if (ImGui::Button("Archetype Editor", ImVec2(window_size.x / 15.f, window_size.y / 18.f)))
          SceneManager::Next_Scene("ArchetypeEditor");

        scene_objects();
      }


      if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("New Scene")) {
            show_new_scene = true;
          }

          if (ImGui::MenuItem("Open Scene")) {
            deselect_object();
            SceneManager::Next_Scene("SceneMenu");
          }

          if (ImGui::MenuItem("Save Scene", "CTRL+S")) {
            SceneManager::Current_Scene()->Save_Data();
            unsaved_changes = false;
          }

          if (ImGui::MenuItem("Quit")) {
            if (Editor::Unsaved_Changes()) {
              Editor::Show_Save(true);
            }
            else
              SceneManager::Quit();
          }


          ImGui::EndMenu();
        }
        static bool paste_active = false;
        if (ImGui::BeginMenu("Edit")) {
          if (ImGui::MenuItem("Cut", "CTRL+X")) {
            selected_object->Active(false);
            copied_object = selected_object;
          }
          if (ImGui::MenuItem("Copy", "CTRL+C")) {
            copied_object = selected_object;
          }
          if (copied_object)
            paste_active = true;
          else
            paste_active = false;
          if (ImGui::MenuItem("Paste", "CTRL+V", false, paste_active)) {
            if (copied_object) {
              GameObject* copy = new GameObject(*copied_object);
              copy->Active(true);
              copy->Get(Transform)->Move(glm::vec3(3.0f, 0.0f, 0.0f));
              current_scene->Add_Object(copy);
              selected_object = copy;
              Transform* transform = selected_object->Get(Transform);
              if (transform)
                Graphics::Camera::Direction(glm::normalize(transform->Translation() - Graphics::Camera::Eye()));
              copied_object = nullptr;
            }
          }
          ImGui::EndMenu();
        }


        if (ImGui::BeginMenu("Objects")) {
          if (ImGui::MenuItem("New Archetype")) {
            new_object = true;
          }

          //if (ImGui::MenuItem("Edit Archetype")) {
          //
          //}
          ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
      }
    }
    else
      Input::Block_Input(false);

    ImGui::End();

    if (Input::Button_Down("Ctrl") && Input::Button_Pressed("Down")) {
      SceneManager::Current_Scene()->Save_Data();
      unsaved_changes = false;
    }

    if (Input::Button_Pressed("Escape")) {
      if (Editor::Unsaved_Changes())
        Editor::Show_Save(true);
      else
        SceneManager::Quit();
    }

    if (Input::Button_Down("Ctrl") && Input::Button_Pressed("Cut")) {
      selected_object->Active(false);
      copied_object = selected_object;
    }
    if (Input::Button_Down("Ctrl") && Input::Button_Pressed("Copy")) {
      copied_object = selected_object;
    }
    if (Input::Button_Down("Ctrl") && Input::Button_Pressed("Paste")) {
      if (copied_object) {
        GameObject* copy = new GameObject(*copied_object);
        copy->Active(true);
        copy->Get(Transform)->Move(glm::vec3(10.0f, 0.0f, 0.0f));
        current_scene->Add_Object(copy);
        selected_object = copy;
        Transform* transform = selected_object->Get(Transform);
        if (transform)
          Graphics::Camera::Direction(glm::normalize(transform->Translation() - Graphics::Camera::Eye()));
        //copied_object = nullptr;
      }
    }

    if (selected_object)
      Object_Editor(false);
    
    if (show_save)
      save_popup();

    if (new_object)
      open_new_object();

    if (show_new_scene)
      open_new_scene();

    //ImGui::ShowDemoWindow();

    //ImGui::Render();
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }



  void EditorSystem::open_new_object()
  {
    if (ImGui::Begin("New Archetype"), &show_object_editor, window_flags) {
      ImGui::SetWindowPos(ImVec2((window_size.x / 2.f) - (window_size.x / 12.f), (window_size.y / 2.f) - (window_size.y / 16.f)));
      ImGui::SetWindowSize(ImVec2(window_size.x / 6.f, window_size.y / 8.f));
      ImGui::SetWindowFontScale(window_size.x / (float)DEFAULT_WIDTH);
      static char buffer[100] = "";
      ImGui::InputText("Object Name", buffer, sizeof(buffer));

      if (ImGui::Button("Okay")) {
        GameObject* obj = ObjectFactory::New_Archetype(buffer);
        obj->Add_Component(Component::ComponentType::cmp_Transform);
        if (current_scene->Name() != "ArchetypeEditor") {
          current_scene->Save_Data();
          SceneManager::Next_Scene("ArchetypeEditor");
        }
        selected_object = obj;
        //show_object_editor = true;
        new_object = false;
      }

    }
    ImGui::End();
  }

  void EditorSystem::scene_objects()
  {
    
    if (ImGui::Begin("Scene Objects", nullptr, window_flags)) {
      ImGui::SetWindowSize(ImVec2(window_size.x / 5.f, window_size.y / 2.1f));
      ImGui::SetWindowPos(ImVec2(0.f, (window_size.y / 60.f) + (window_size.y / 2.f)));
      ImGui::SetWindowFontScale(window_size.x / 1920.f);

      if (ImGui::Button("Add Object", ImVec2(128.f, 32.f)))
        show_archetype_selector = true;

      const Objects& objects = SceneManager::Current_Scene()->Get_Objects();
      for (unsigned i = 0; i < objects.size(); ++i)
      {
        ImGui::PushID(i);
        if (ImGui::Button(objects[i]->Name().c_str())) {
          if (selected_object || selected_object == objects[i])
            deselect_object();
          
          selected_object = objects[i];
          Transform* transform = selected_object->Get(Transform);
          if (transform) {
            Graphics::Camera::Direction(glm::normalize(transform->Translation() - Graphics::Camera::Eye()));

          }
        }
        ImGui::PopID();
      }


      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::NewLine();

      if (show_archetype_selector) {
        GameObject* obj = archetype_selector();
        if (obj)
        {
          SceneManager::Current_Scene()->Add_Object(obj);
          obj->Active(true);
        }
      }

    }
    ImGui::End();
  }
  

  void EditorSystem::Object_Editor(bool archetype)
  {
    if (ImGui::Begin(std::string("Object Editor - " + selected_object->Name()).c_str(), nullptr, window_flags)) {
      ImGui::SetWindowSize(ImVec2(window_size.x / 3.f, window_size.y / 1.5f));
      ImGui::SetWindowPos(ImVec2(window_size.x / 1.52f, window_size.y / 60.f));
      ImGui::SetWindowFontScale(window_size.x / 1920.f);

      std::string name = selected_object->Name();
      if (ImGui::InputText("Name", &name))
        unsaved_changes = true;
      selected_object->Name(name);

      if (ImGui::TreeNode("Components")) {
        const Components& components = selected_object->Get_Components();
        for (unsigned i = 0; i < components.size(); ++i)
        {
          ImGui::PushID(i);
          if (components[i]->Type() == Component::ComponentType::cmp_Transform) {
            Transform* transform = selected_object->Get(Transform);
            if (ImGui::TreeNode("Transform")) {
              transform_editor(!selected_object->Active());
              ImGui::TreePop();
            }
          }
          else if (components[i]->Type() == Component::ComponentType::cmp_Model) {
            Model* model = selected_object->Get(Model);
            
            if (ImGui::TreeNode("Model")) {
              sprite_editor(!selected_object->Active());
              ImGui::TreePop();
            }
          }
          ImGui::PopID();
        }
        std::vector<Graphics::Light*>& obj_lights = selected_object->Get_Lights();
        if (obj_lights.size() > 0) {
          if (ImGui::TreeNode("Lights")) {
            for (unsigned j = 0; j < obj_lights.size(); ++j)
            {
              ImGui::PushID(j);
              std::string light_name = "Light " + std::to_string(j);
              if (ImGui::TreeNode(light_name.c_str())) {

                Graphics::PointLight* point_light = static_cast<Graphics::PointLight*>(obj_lights[j]);
                Transform* obj_transform = selected_object->Get(Transform);
                glm::vec3 offset = point_light->Position() - obj_transform->Translation();
                if (ImGui::DragFloat3("Offset", &offset[0], 1.f / 100.f)) {
                  unsaved_changes = true;
                  point_light->Position(offset + obj_transform->Translation());
                }
                edit_light(obj_lights[j]);
                static_cast<Graphics::PointLight*>(obj_lights[j])->Draw();
                ImGui::TreePop();
              }
              ImGui::PopID();
            }
            ImGui::TreePop();

          }
        }
        
        ImGui::TreePop();

        if (ImGui::Button("Add Component"))
          show_new_component = true;


        if (show_new_component)
          component_selector();
      }

      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();
      ImGui::NewLine();

      if (!selected_object->Active()) {
        if (ImGui::Button("Save Archetype", ImVec2(128, 64)) && selected_object) {
          selected_object = nullptr;
        }
        ImGui::NewLine();
        if (ImGui::Button("Remove Archetype", ImVec2(128, 64)) && selected_object) {
          ObjectFactory::Remove_Archetype(selected_object);
          selected_object = nullptr;
        }
      }
      else {
        if (ImGui::Button("Save Object", ImVec2(128, 64)) && selected_object)
          deselect_object();

        ImGui::NewLine();

        if (ImGui::Button("Remove Object", ImVec2(128, 64)) && selected_object) {
          selected_object->Destroy();
          deselect_object();
        }
      }
      ImGui::NewLine();

      if (selected_object && selected_object->Active())
        draw_selected_object(selected_object);


    }
    ImGui::End();
  }


  void EditorSystem::sprite_editor(bool archetype)
  {
    static bool wireframe = false;
    static Graphics::Mesh* active_mesh = nullptr;
    static int mesh_index = 0;

    Model* sprite = selected_object->Get(Model);

    float alpha = sprite->Alpha();
    if (ImGui::DragFloat("Opacity", &alpha, 1.f / 150.f, 0.f, 1.f))
      unsaved_changes = true;
    sprite->Alpha(alpha);

    Graphics::Material material = sprite->Material();
    glm::vec3 ambient = material.Ambient();
    if (ImGui::ColorEdit3("Ambient Color", glm::value_ptr(ambient)))
      unsaved_changes = true;
    material.Ambient(ambient);

    glm::vec3 diffuse = material.Diffuse();
    if (ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(diffuse)))
      unsaved_changes = true;
    material.Diffuse(diffuse);

    glm::vec3 specular = material.Specular();
    if (ImGui::ColorEdit3("Specular Color", glm::value_ptr(specular)))
      unsaved_changes = true;
    material.Specular(specular);

    float shininess = material.Shininess();
    if (ImGui::DragFloat("Shininess", &shininess, 1.0f / 100.f, 0.f, 150.f))
      unsaved_changes = true;
    material.Shininess(shininess);

    static int e = 0;
    ImGui::RadioButton("Reflection", &e, 0); ImGui::SameLine();
    ImGui::RadioButton("Refraction", &e, 1); ImGui::SameLine();
    ImGui::RadioButton("None", &e, 2);

    if (e == 0)
    {
      float reflectiveness = material.Reflectiveness();
      if (ImGui::SliderFloat("Reflectiveness", &reflectiveness, 0.f, 1.f))
        unsaved_changes = true;
      material.Reflectiveness(reflectiveness);
    }
    else if (e == 1)
    {
      float refraction = material.Refraction();
      if (ImGui::DragFloat("Refraction Index", &refraction, 1.f / 100.f, 0.f, 5.f))
        unsaved_changes = true;
      material.Refraction(refraction);
    }
    else if (e == 2)
    {
      material.Refraction(1.f);
      material.Reflectiveness(0.f);
    }

    sprite->Material(material);

    ImGui::Checkbox("Draw Wireframe", &wireframe);


    if (ImGui::TreeNode("Meshes")) {
      Meshes& meshes = sprite->Get_Meshes();
      for (unsigned i = 0; i < meshes.size(); ++i)
      {
        
        if (ImGui::TreeNode(std::string("Mesh" + std::to_string(i)).c_str())) {
          if (ImGui::Button("Remove Mesh")) {
            sprite->Remove_Mesh(i);
            ImGui::TreePop();
            break;
          }

          glm::vec2 tex_scale = meshes[i].Texture_Scale();
          if (ImGui::DragFloat2("Texture Scale", &tex_scale[0], 1.f / 100.f, 0.f, 5.f))
            unsaved_changes = true;
          meshes[i].Texture_Scale(tex_scale);

          ImGui::NewLine();

          glm::vec3 offset = meshes[i].Pos_Offset();
          if (ImGui::DragFloat3("Position Offset", &offset[0], 1.f / 100.f, -10.f, 10.f))
            unsaved_changes = true;
          meshes[i].Pos_Offset(offset);

          Graphics::Shader* shader = Graphics::Shaders::Get_Shader("Wireframe");
          if (archetype)
            Graphics::Camera::Set_Uniforms(shader);
          else
            Graphics::Camera::Set_Uniforms(shader);
          shader->Use();
          Transform* transform = selected_object->Get(Transform);
          if (transform) {
            shader->Set_Uniform(transform->Matrix(), "Model");
            shader->Set_Uniform(glm::vec3(1.f, 0.f, 0.f), "wireframe_color");
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            meshes[i].Draw(shader);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            shader->Unuse();
          }
          

          const std::vector<Graphics::Texture*>& textures = meshes[i].Textures();

          ImGui::Text("Textures:");
          for (unsigned j = 0; j < textures.size(); ++j)
          {
            std::string tex_name = textures[j]->Name();
            std::string button_name("File: " + tex_name);

            if (ImGui::Button(button_name.c_str())) {
              show_texture_library = true;
              active_mesh = &meshes[i];
              mesh_index = j;
            }

            ImGui::SameLine();
            if (ImGui::Button("Remove"))
              meshes[i].Remove_Texture(j);
          }
          if (ImGui::Button("Add Texture")) {
            show_texture_library = true;
            active_mesh = &meshes[i];
            mesh_index = -1;
          }

          ImGui::TreePop();
        }
      }
      ImGui::TreePop();

      if (ImGui::Button("Add Mesh"))
        show_mesh_selector = true;
    }

    if (show_texture_library)
      texture_library(active_mesh, mesh_index);

    if (show_mesh_selector)
      mesh_selector();

    if (wireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


  }


  void EditorSystem::transform_editor(bool archetype)
  {
    Transform* transform = selected_object->Get(Transform);
    
    if (!archetype)
    {
      glm::vec3 pos = transform->Translation();
      if (ImGui::DragFloat3("Translation", &pos[0], 1.f / 25.f))
        unsaved_changes = true;
      transform->Translation(pos);
      //ImGui::NewLine();
      ImGui::Separator();
      //ImGui::NewLine();
    }


    glm::vec3 scale = transform->Scale();
    if (ImGui::DragFloat3("Scale", &scale[0], 1.f / 100.f, 0.f))
      unsaved_changes = true;
    transform->Scale(scale);
    //ImGui::NewLine();
    ImGui::Separator();
    //ImGui::NewLine();

    glm::vec3 rotate(transform->RotationX(), transform->RotationY(), transform->RotationZ());
    if (ImGui::DragFloat3("Rotation", &rotate[0], 1.f / 15.f, -360, 360))
      unsaved_changes = true;
    transform->Rotation(rotate);
    //ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();
  }

  void EditorSystem::texture_library(Graphics::Mesh* mesh, int index)
  {
    if (ImGui::Begin("Texture Library", &show_texture_library, window_flags))
    {
      ImGui::SetWindowSize(ImVec2(window_size.x / 4.5f, window_size.y / 3.f));
      ImGui::SetWindowPos(ImVec2(window_size.x / 1.4f, window_size.y / 1.7f));
      ImGui::SetWindowFontScale(window_size.x / 1920.f);

      static int e = 0;
      ImGui::RadioButton("Diffuse Texture", &e, 0); ImGui::SameLine();
      ImGui::RadioButton("Specular Texture", &e, 1); ImGui::SameLine();
      ImGui::RadioButton("Normal Texture", &e, 2);

      const std::vector<Graphics::Texture*>& textures = Graphics::TextureLibrary::Get_Textures();
      for (int i = 0; i < textures.size(); ++i)
      {
        ImGui::Text(textures[i]->Name().c_str());
        ImGui::SameLine(150.f);
        ImGui::PushID(textures[i]->Get_ID());
        if (ImGui::ImageButton((ImTextureID)(textures[i]->Get_ID()), ImVec2(64, 64)))
        {
          if (e == 0)
            textures[i]->Type("texture_diffuse");
          else if (e == 1)
            textures[i]->Type("texture_specular");
          else if (e == 2)
            textures[i]->Type("texture_normal");
          if (index == -1)
            mesh->Add_Texture(textures[i]);
          else
            mesh->Set_Texture(textures[i], index);
          unsaved_changes = true;
          show_texture_library = false;
        }
        ImGui::SameLine(250.f);

        if (ImGui::Button("Remove"))
        {
          Graphics::TextureLibrary::Remove_Texture(textures[i]);
        }
        ImGui::PopID();
        ImGui::NewLine();
      }

      if (ImGui::Button("Add Texture") && !file_browser->IsOpened())
      {
        file_browser->Open();
      }

      file_browser->Display();
      if (file_browser->IsOpened() && file_browser->HasSelected())
      {
        std::filesystem::path path = file_browser->GetSelected();
        std::string path_str = path.generic_string();
        int x = static_cast<int>(path_str.size());
        while (path_str[x] != '/')
        {
          --x;
        }
        std::string name = std::string(path_str, x + 1);
        std::ofstream("./Assets/Textures/" + name, std::ios::binary) << std::ifstream(path_str, std::ios::binary).rdbuf();
        Graphics::TextureLibrary::Add_Texture(new Graphics::Texture("./Assets/Textures/" + name));
      }

    }
    ImGui::End();
  }


  GameObject* EditorSystem::archetype_selector()
  {
    if (ImGui::Begin("Archetype Selector", &show_archetype_selector, window_flags))
    {
      ImGui::SetWindowPos(ImVec2(window_size.x / 5.f, window_size.y / 2.f));
      ImGui::SetWindowSize(ImVec2(window_size.x / 5.f, window_size.y / 3.f));
      ImGui::SetWindowFontScale(window_size.x / 1920.f);

      Archetypes& archetypes = ObjectFactory::Get_Archetypes();
      for (Archetypes::iterator it = archetypes.begin(); it != archetypes.end(); ++it)
      {
        if (ImGui::Button(it->second.Name().c_str()))
        {
          ImGui::End();
          show_archetype_selector = false;
          unsaved_changes = true;

          //return ObjectFactory::Create(it.second.Name());
          return new GameObject(it->second);
        }
      }
    }
    ImGui::End();
    return nullptr;
  }

  void EditorSystem::component_selector()
  {
    if (ImGui::Begin("Component Selector", &show_new_component, window_flags))
    {
      ImGui::SetWindowPos(ImVec2(window_size.x / 1.7f, window_size.y / 15.f));
      ImGui::SetWindowSize(ImVec2(window_size.x / 6.4f, window_size.y / 3.6f));
      ImGui::SetWindowFontScale(window_size.x / 1920.f);

      if (!selected_object->Has_Component(Component::ComponentType::cmp_Model) && ImGui::Button("Model"))
      {
        Model* model = new Model();
        selected_object->Add_Component(model);
        show_new_component = false;
        unsaved_changes = true;
      }
      else if (!selected_object->Has_Component(Component::ComponentType::cmp_Physics) && ImGui::Button("Physics"))
      {
        Physics* physics = new Physics();
        selected_object->Add_Component(physics);
        show_new_component = false;
        unsaved_changes = true;
      }
      else if (ImGui::Button("Light"))
      {
        selected_object->Add_Light();
        unsaved_changes = true;
        show_new_component = false;
      }
    }
    ImGui::End();
  }


  void EditorSystem::mesh_selector()
  {
    if (ImGui::Begin("Mesh Selector", &show_mesh_selector, window_flags)) {
      ImGui::SetWindowPos(ImVec2(window_size.x / 1.8f, window_size.y / 15.f));
      ImGui::SetWindowSize(ImVec2(window_size.x / 6.4f, window_size.y / 3.6f));
      ImGui::SetWindowFontScale(window_size.x / 1920.f);

      Model* model = selected_object->Get(Model);

      if (ImGui::Button("Quad")) {
        Graphics::Mesh mesh = *Graphics::MeshLibrary::Get_Mesh("Quad");
        model->Add_Mesh(mesh);
        show_mesh_selector = false;
        unsaved_changes = true;
      }
      else if (ImGui::Button("Triangle")) {
        Graphics::Mesh mesh = *Graphics::MeshLibrary::Get_Mesh("Triangle");
        model->Add_Mesh(mesh);
        show_mesh_selector = false;
        unsaved_changes = true;
      }
      else if (ImGui::Button("Cube")) {
        Graphics::Mesh mesh = *Graphics::MeshLibrary::Get_Mesh("Cube");
        model->Add_Mesh(mesh);
        show_mesh_selector = false;
        unsaved_changes = true;
      }
      else if (ImGui::Button("Pyramid")) {
        Graphics::Mesh mesh = *Graphics::MeshLibrary::Get_Mesh("Pyramid");
        model->Add_Mesh(mesh);
        show_mesh_selector = false;
        unsaved_changes = true;
      }
      else if (ImGui::Button("Sphere")) {
        Graphics::Mesh mesh = *Graphics::MeshLibrary::Get_Mesh("Sphere");
        model->Add_Mesh(mesh);
        show_mesh_selector = false;
        unsaved_changes = true;
      }

      const Models& models = ModelLibrary::Get_Models();
      for (Models::const_iterator it = models.begin(); it != models.end(); ++it)
      {
        if (ImGui::Button(it->first.c_str())) {
          *model = ModelLibrary::Get_Model(it->first);
          show_mesh_selector = false;
          unsaved_changes = true;
        }
      }
    }
    ImGui::End();
  }

  void EditorSystem::deselect_object()
  {
    Model* model = selected_object->Get(Model);
    if (model)
      model->Active(true);
    selected_object = nullptr;
  }

  static void draw_selected_object(GameObject* obj)
  {
    Graphics::Shader* shader = Graphics::Shaders::Get_Shader("Outline");
    Graphics::Shader* core_shader = Graphics::Shaders::Get_Shader("Core");
    Model* model = obj->Get(Model);
    Transform* transform = obj->Get(Transform);
    //model->Active(false);
    Graphics::Camera::Set_Uniforms(core_shader);
    Graphics::Camera::Set_Uniforms(shader);
    Graphics::Shader* prev_shader = model->Shader();
    
    glStencilMask(0x00);
    model->Draw();

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    model->Draw();

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    model->Shader(shader);
    float scale = 1.1f;
    glm::vec3 old_scale = transform->Scale();
    glm::vec3 new_scale = scale * old_scale;
    transform->Scale(new_scale);
    model->Draw();
    model->Shader(prev_shader);
    transform->Scale(old_scale);
    glStencilMask(0xFF);  
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
  }


  void EditorSystem::save_popup()
  {
    if (ImGui::Begin("Save")) {
      ImGui::SetWindowPos(ImVec2((window_size.x / 2.f) - (window_size.x / 12.f), (window_size.y / 2.f) - (window_size.y / 16.f)));
      ImGui::SetWindowSize(ImVec2(window_size.x / 6.f, window_size.y / 8.f));
      ImGui::SetWindowFontScale(window_size.x / (float)DEFAULT_WIDTH);

      ImGui::TextWrapped("  You have unsaved changes in this scene!");
      ImGui::TextWrapped("          Would you like to save?");
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::SameLine(window_size.x / 45.f);
      if (ImGui::Button("Yes"))
      {
        SceneManager::Current_Scene()->Save_Data();
        unsaved_changes = false;
        show_save = false;
        SceneManager::Quit();
      }
      ImGui::SameLine(window_size.x / 8.f);
      if (ImGui::Button("No"))
      {
        show_save = false;
        SceneManager::Quit();
      }
    }
    ImGui::End();
  }

  static bool check_skybox_dir(const std::filesystem::path& path, std::vector<std::string>& filenames)
  {
    bool has_left = false, has_right = false, has_top = false, has_bottom = false, has_back = false, has_front = false;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
      std::string file = entry.path().generic_string();
      if (file.find("back.") != std::string::npos) {
        filenames[5] = file;
        has_back = true;
      }
      else if (file.find("front.") != std::string::npos) {
        filenames[4] = file;
        has_front = true;
      }
      else if (file.find("left.") != std::string::npos) {
        filenames[1] = file;
        has_left = true;
      }
      else if (file.find("right.") != std::string::npos) {
        filenames[0] = file;
        has_right = true;
      }
      else if (file.find("top.") != std::string::npos) {
        filenames[2] = file;
        has_top = true;
      }
      else if (file.find("bottom.") != std::string::npos) {
        filenames[3] = file;
        has_bottom = true;
      }
    }
    return has_bottom && has_top && has_left && has_right && has_front && has_back;
  }
  void EditorSystem::edit_skybox(Skybox* skybox)
  {
    static std::vector<std::string> filenames;
    static bool show_error = false;
    filenames.resize(6);
    if (skybox) {

      //glm::vec3 offset = skybox->Offset();
      //if (ImGui::DragFloat3("Offset", &offset[0], 1 / 200.f)) {
      //  unsaved_changes = true;
      //  skybox->Offset(offset);
      //}
      //ImGui::NewLine();

      bool pressed = ImGui::Button("Edit Skybox");
      ImGui::SameLine();
      HelpMarker("Skybox images must be in own folder in 'Assets/Skybox' and images must be labeled back, front, top, bottom, left, and right");
      if (pressed)
        file_browser->Open();

      file_browser->Display();
      if (file_browser->HasSelected() && file_browser->IsOpened() == false) {
        bool success = check_skybox_dir(file_browser->GetSelected(), filenames);
        if (success)
          SceneManager::Current_Scene()->Set_Skybox(new Skybox(filenames));
        else
          show_error = true;
        filenames.clear();
        file_browser->ClearSelected();
      }

      ImGui::NewLine();

      if (ImGui::Button("Remove Skybox")) {
        delete skybox;
        SceneManager::Current_Scene()->Set_Skybox(nullptr);
      }
      ImGui::NewLine();
    }
    else {
      bool pressed = ImGui::Button("Add Skybox");
      ImGui::SameLine(); 
      HelpMarker("Skybox images must be in own folder in 'Assets/Skybox' and images must be labeled back, front, top, bottom, left, and right");
      if (pressed) {
        file_browser->ClearSelected();
        file_browser->SetPwd(std::filesystem::current_path().generic_string() + "/Assets/Skybox");
        file_browser->Open();
      }
      file_browser->Display();
      if (file_browser->HasSelected() && file_browser->IsOpened() == false) {
        bool success = check_skybox_dir(file_browser->GetSelected(), filenames);
        if (success)
          SceneManager::Current_Scene()->Set_Skybox(new Skybox(filenames));
        else
          show_error = true;
        filenames.clear();
        file_browser->ClearSelected();
      }
    }

    if (show_error) {
      if (ImGui::Begin("Skybox Error", &show_error, window_flags)) {
        ImGui::SetWindowPos(ImVec2((window_size.x / 2.f) - (window_size.x / 12.f), (window_size.y / 2.f) - (window_size.y / 16.f)));
        ImGui::SetWindowSize(ImVec2(window_size.x / 6.f, window_size.y / 8.f));
        ImGui::SetWindowFontScale(window_size.x / (float)DEFAULT_WIDTH);

        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Error: ");
        ImGui::TextWrapped("Skybox files not in correct format. Skybox images must contain six images for each side of box, containing \"left, right, top, bottom, front, back\" in the filenames");

        if (ImGui::Button("Ok"))
          show_error = false;
      }
      ImGui::End();
    }
  }


  void EditorSystem::light_editor()
  {
    const std::vector<Graphics::Light*>& lights = SceneManager::Current_Scene()->Lights();
    for (unsigned i = 0; i < lights.size(); ++i)
    {
      Graphics::Light* light = lights[i];
      ImGui::PushID(i);
      std::string prefix;
      if (dynamic_cast<Graphics::PointLight*>(light)) {
        prefix += "PointLight";
        static_cast<Graphics::PointLight*>(light)->Draw();
      }
      else if (dynamic_cast<Graphics::DirectionalLight*>(lights[i]))
        prefix += "DirectionalLight";

      if (ImGui::TreeNode(std::string(prefix + std::to_string(i)).c_str())) {
        if (dynamic_cast<Graphics::PointLight*>(lights[i])) {
          Graphics::PointLight* point_light = static_cast<Graphics::PointLight*>(light);
          glm::vec3 pos = point_light->Position();
          if (ImGui::DragFloat3("Light Position", &pos[0], 1.f / 100.f))
            unsaved_changes = true;
          point_light->Position(pos);

          //ImGui::Text("Attenuation: ");
          //float constant = point_light->Constant();
          //if (ImGui::DragFloat("Constant", &constant, 1.f / 200.f))
          //  unsaved_changes = true;
          //point_light->Constant(constant);
          //
          //float linear = point_light->Linear();
          //if (ImGui::DragFloat("Linear", &linear, 1.f / 200.f))
          //  unsaved_changes = true;
          //point_light->Linear(linear);
          //
          //float quadratic = point_light->Quadratic();
          //if (ImGui::DragFloat("Quadratic", &quadratic, 1.f / 200.f))
          //  unsaved_changes = true;
          //point_light->Quadratic(quadratic);

        }
        else if (dynamic_cast<Graphics::DirectionalLight*>(light)) {
          glm::vec3 dir = static_cast<Graphics::DirectionalLight*>(light)->Direction();
          if (ImGui::DragFloat3("Light Direction", &dir[0], 1.f / 100.f))
            unsaved_changes = true;
          static_cast<Graphics::DirectionalLight*>(light)->Direction(dir);
        }
        edit_light(lights[i]);
        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    if (ImGui::TreeNode("Add Light"))
    {
      if (ImGui::Button("Point Light"))
      {
        SceneManager::Current_Scene()->Add_Light(new Graphics::PointLight(glm::vec3(0.0f, 0.0f, 0.f), 1.f, glm::vec3(1.f, 1.f, 1.f)));
        unsaved_changes = true;
        ImGui::CloseCurrentPopup();
      }
      else if (ImGui::Button("Directional Light"))
      {
        SceneManager::Current_Scene()->Add_Light(new Graphics::DirectionalLight(glm::vec3(0.0f, -1.0f, 0.f), 1.f, glm::vec3(1.f, 1.f, 1.f)));
        unsaved_changes = true;
      }
      ImGui::TreePop();
    }
    ImGui::TreePop();
  }


  void EditorSystem::edit_camera()
  {
    float sensitivity = Graphics::Camera::Sensitivity() * 10;
    if (ImGui::DragFloat("Camera Sensitivity", &sensitivity, 1.0f / 150.f, 0.0f, 10.0f)) {
      unsaved_changes = true;
      Graphics::Camera::Sensitivity(sensitivity / 10.f);
    }
    ImGui::NewLine();

    float fov = Graphics::Camera::FOV();
    if (ImGui::DragFloat("FOV", &fov, 1.0f / 100.f, MIN_FOV, MAX_FOV)) {
      unsaved_changes = true;
      Graphics::Camera::FOV(fov);
    }
    ImGui::NewLine();

    float move_speed = Graphics::Camera::Move_Speed();
    if (ImGui::DragFloat("Move Speed", &move_speed, 1.f / 100.f, 5.f, 100.f)) {
      unsaved_changes = true;
      Graphics::Camera::Move_Speed(move_speed);
    }
  }

  void EditorSystem::edit_light(Graphics::Light* light)
  {
    

    ImGui::Text("Color: ");
    glm::vec3 amb = light->Ambient();
    if (ImGui::ColorEdit3("Ambient Color", &amb[0]))
      unsaved_changes = true;
    light->Ambient(amb);

    glm::vec3 diff = light->Diffuse();
    if (ImGui::ColorEdit3("Diffuse Color", &diff[0]))
      unsaved_changes = true;
    light->Diffuse(diff);

    glm::vec3 spec = light->Specular();
    if (ImGui::ColorEdit3("Specular Color", &spec[0]))
      unsaved_changes = true;
    light->Specular(spec);

    float intensity = light->Intensity();
    if (ImGui::DragFloat("Light Intensity", &intensity, 1.f / 200.f, 0.f, 3.f))
      unsaved_changes = true;
    light->Intensity(intensity);

    ImGui::NewLine();
    if (ImGui::Button("Remove"))
      SceneManager::Current_Scene()->Remove_Light(light);

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();
  }


/*************************************************************/
/*                      Scene Selector                       */
/*************************************************************/
  void EditorSystem::Scene_Selector()
  {
    if (ImGui::Begin("Scene Selector", nullptr, window_flags | ImGuiWindowFlags_NoCollapse)) {
      ImGui::SetWindowPos(ImVec2((window_size.x / 2.f) - (window_size.x / 8.f), (window_size.y / 2.f) - (window_size.y / 12.f)));
      ImGui::SetWindowSize(ImVec2(window_size.x / 4.f, window_size.y / 6.f));
      ImGui::SetWindowFontScale(window_size.x / (float)DEFAULT_WIDTH);

      if (ImGui::BeginMenu("Select Scene: ")) {
        const Scenes& scenes = SceneManager::Get_Scenes();
        Scene* current = SceneManager::Current_Scene();
        for (Scenes::const_iterator it = scenes.begin(); it != scenes.end(); ++it)
        {
          if (current == it->second)
            continue;
          if (ImGui::MenuItem(it->second->Name().c_str())) {
            current->Save_Data();
            SceneManager::Next_Scene(it->second);
            active = true;
          }
        }
        ImGui::EndMenu();
      }
      ImGui::NewLine();
      ImGui::Separator();
      ImGui::NewLine();

      if (ImGui::Button("Create New Scene")) {
        show_new_scene = true;
      }

      if (show_new_scene)
        open_new_scene();

    }
    ImGui::End();
  }

  void EditorSystem::open_new_scene()
  {
    if (ImGui::Begin("New Scene", &show_new_scene, window_flags))
    {
      ImGui::SetWindowPos(ImVec2((window_size.x / 2.f) - (window_size.x / 12.f), (window_size.y / 2.f) - (window_size.y / 16.f)));
      ImGui::SetWindowSize(ImVec2(window_size.x / 6.f, window_size.y / 8.f));
      ImGui::SetWindowFontScale(window_size.x / (float)DEFAULT_WIDTH);
      static char buffer[100] = "";
      ImGui::InputText("Scene Name", buffer, sizeof(buffer));

      if (ImGui::Button("Okay"))
      {
        show_new_scene = false;
        Scene* scene = SceneManager::Create_Scene(buffer);
        //scene->Save_Data();
        SceneManager::Next_Scene(scene);
      }
    }
    ImGui::End();
  }

/*************************************************************/
/*                   Archetype Editor                        */
/*************************************************************/
  void EditorSystem::Archetype_Editor()
  {
    if (ImGui::Begin("Archetypes")) {
      ImGui::SetWindowSize(ImVec2(400.0f, 350.f));
      ImGui::SetWindowPos(ImVec2(0.f, 625.f));

      if (ImGui::Button("New Archetype"))
        new_object = true;
      ImGui::Separator();

      Archetypes& archetypes = ObjectFactory::Get_Archetypes();
      for (Archetypes::iterator it = archetypes.begin(); it != archetypes.end(); ++it)
      {
        if (ImGui::Button(it->second.Name().c_str())) {
          selected_object = &it->second;

          Transform* transform = selected_object->Get(Transform);
          if (transform) {
            Graphics::Camera::Direction(glm::normalize(transform->Translation() - Graphics::Camera::Eye()));
          }
        }
      }

      ImGui::End();
    }
  }

/*************************************************************/
/*                        ECS Instance                       */
/*************************************************************/

  static EditorSystem* Editor_System = nullptr;

  System* Init()
  {
    static EditorSystem editor_sys;
    Editor_System = &editor_sys;
    return Editor_System;
  }

  void Update(float dt)
  {
    if (Editor_System)
      Editor_System->Update(dt);
  }

  void Shutdown()
  {
    if (Editor_System)
      Editor_System->Shutdown();
  }

  void Object_Editor(bool archetype)
  {
    Editor_System->Object_Editor(archetype);
  }

  void Selected_Object(GameObject* object)
  {
    Editor_System->Selected_Object(object);
  }

  GameObject* Selected_Object()
  {
    return Editor_System->Selected_Object();
  }

  bool Unsaved_Changes()
  {
    return Editor_System->unsaved_changes;
  }

  void Show_Save(bool show)
  {
    Editor_System->show_save = true;
  }

  void Scene_Selector()
  {
    Editor_System->Scene_Selector();
  }

  void Archetype_Editor()
  {
    Editor_System->Archetype_Editor();
  }

  void Enable_Editor(bool enable)
  {
    Editor_System->Enable_Editor(enable);
  }
}



















