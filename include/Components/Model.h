#pragma once

#include <Components/Component.h>
#include <Graphics/Material.h>
#include <Graphics/Mesh.h>

namespace Graphics {
  class Texture;
  class Shader;
}

typedef std::vector<Graphics::Mesh> Meshes;

class Model : public Component
{
public:
  Model(const std::string& path = "");
  Model(const Model& other);
  ~Model();

  Component* Clone() const;

  void Init();
  void Tick(float dt);
  void Draw();
  void DInit();

  void Desearialize(rapidjson::Document& document);
  void Write(std::ofstream& file);
  void Read(std::fstream& file);

  void Mesh(const Graphics::Mesh& mesh) { meshes_.push_back(mesh); }
  Graphics::Material& Material()  { return material_; }
  void Material(const Graphics::Material& material) { material_ = material; }

  std::vector<Graphics::Mesh>& Get_Meshes() { return meshes_; }
  void Add_Mesh(Graphics::Mesh& mesh) { meshes_.push_back(mesh); }
  void Remove_Mesh(const unsigned& index) {
    Meshes::iterator it = meshes_.begin() + index;
    meshes_.erase(it);
  }
  void Remove_Mesh(Graphics::Mesh* mesh) {
    for (Meshes::iterator it = meshes_.begin(); it != meshes_.end(); ++it)
      if (mesh == &(*it))
        meshes_.erase(it); return;
  }

  const float& Alpha() const { return alpha_; }
  void Alpha(const float& alpha) { alpha_ = alpha; }

  Graphics::Shader* Shader() const { return shader_; }
  void Shader(Graphics::Shader* shader) { shader_ = shader; }


  void Queue_Draw(Graphics::Mesh* mesh, Graphics::Shader* shader) { to_draw_.push_back(std::make_pair(mesh, shader)); }

  Model& operator=(const Model& other);

private:
  Meshes meshes_;
  Graphics::Material material_;
  Graphics::Shader* shader_;
  float alpha_;

  std::vector<std::pair<Graphics::Mesh*, Graphics::Shader*>> to_draw_;
  std::vector<Graphics::Texture*> textures_loaded;
  std::string model_path_;
  std::string prefix_;

  // model loading
  void load_model(const std::string& path);
  void process_node(aiNode* node, const aiScene* scene);
  Graphics::Mesh* process_mesh(aiMesh* ai_mesh, const aiScene* scene);
  std::vector<Graphics::Texture*> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);

  friend class Mesh;
};