#include <stdafx.h>

#include <Components/Model.h>
#include <Graphics/MeshLibrary.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>
#include <Graphics/GraphicsSystem.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureLibrary.h>
#include <Objects/ModelLibrary.h>

#include <Objects/GameObject.h>
#include <Components/Transform.h>
#include <Objects/Primitives.h> 


Model::Model(const std::string& path) : Component(ComponentType::cmp_Model), meshes_(), textures_loaded(), shader_(Graphics::Shaders::Get_Shader("Core")), alpha_(1.0f), 
                                          material_(), prefix_(""), model_path_("")
{
  if (path != "")
  {
    load_model(path);
  }
}

Model::Model(const Model& other) : Component(ComponentType::cmp_Model)
{
  *this = other;
}

Component* Model::Clone() const
{
  return new Model(*this);
}


Model::~Model()
{

}

void Model::Init()
{

}

void Model::Tick(float dt)
{
  for (unsigned i = 0; i < meshes_.size(); ++i)
    meshes_[i].Update();
}

void Model::Draw()
{
  Transform* transform = Parent()->Get(Transform);
  shader_->Use();
  material_.Send(shader_);

  glm::mat4 mtx = transform->Matrix();
  shader_->Set_Uniform(mtx, "Model");
  shader_->Set_Uniform(alpha_, "opacity");

  //glStencilMask(0x00);

  for (int i = 0; i < meshes_.size(); ++i)
  {
    meshes_[i].Draw(shader_);
  }

  shader_->Unuse();

  for (std::vector<std::pair<Graphics::Mesh*, Graphics::Shader*>>::iterator it = to_draw_.begin();
    it != to_draw_.end(); ++it)
  {
    it->second->Use();
    it->second->Set_Uniform(mtx, "Model");
    it->first->Draw(it->second);
    it->second->Unuse();
  }
  to_draw_.clear();


}

void Model::DInit()
{

}

void Model::Desearialize(rapidjson::Document& document)
{
  const rapidjson::Value& spr = document["Sprite"];
  assert(spr.IsObject());
  rapidjson::GenericObject sprite = spr.GetObject();
  
  if (sprite.HasMember("Model"))
  {
    std::string path(sprite["Model"].GetString());
    load_model("./Assets/Models/" + path);
  }
  else
  {
    if (sprite.HasMember("Primitive"))
    {
      std::string prim_type = sprite["Primitive"].GetString();
      if (prim_type == "Pyramid")
      {
        
        if (Graphics::MeshLibrary::Has_Mesh("Pyramid"))
        {
          meshes_.push_back(*Graphics::MeshLibrary::Get_Mesh("Pyramid"));
        }
        else
        {
          Shapes::Pyramid pyramid = Shapes::Pyramid();
          Graphics::Mesh* mesh = Graphics::MeshLibrary::Add_Mesh("Pyramid", pyramid.Get_Vertices(), pyramid.Get_Indices());
          //Graphics::Mesh* mesh = new Graphics::Mesh(&pyramid);
          meshes_.push_back(*mesh);
          //Graphics::MeshLibrary::Add_Mesh("Pyramid", mesh);
        }
      }
      else if (prim_type == "Triangle")
      {
        if (Graphics::MeshLibrary::Has_Mesh("Triangle"))
        {
          meshes_.push_back(*Graphics::MeshLibrary::Get_Mesh("Triangle"));
        }
        else
        {
          Shapes::Triangle triangle = Shapes::Triangle();
          Graphics::Mesh* mesh = Graphics::MeshLibrary::Add_Mesh("Triangle", triangle.Get_Vertices(), triangle.Get_Indices());
          //Graphics::Mesh* mesh = new Graphics::Mesh(&triangle);
          meshes_.push_back(*mesh);
          //Graphics::MeshLibrary::Add_Mesh("Triangle", mesh);
        }
      }
      else if (prim_type == "Quad")
      {
        if (Graphics::MeshLibrary::Has_Mesh("Quad"))
        {
          meshes_.push_back(*Graphics::MeshLibrary::Get_Mesh("Quad"));
        }
        else
        {
          Shapes::Quad quad = Shapes::Quad();
          Graphics::Mesh* mesh = Graphics::MeshLibrary::Add_Mesh("Quad", quad.Get_Vertices(), quad.Get_Indices());
          meshes_.push_back(*mesh);
        }
      }
      else if (prim_type == "Cube")
      {
        if (Graphics::MeshLibrary::Has_Mesh("Cube"))
        {
          meshes_.push_back(*Graphics::MeshLibrary::Get_Mesh("Cube"));
        }
        else
        {
          Shapes::Cube cube = Shapes::Cube();
          Graphics::Mesh* mesh = Graphics::MeshLibrary::Add_Mesh("Cube", cube.Get_Vertices(), cube.Get_Indices());
          meshes_.push_back(*mesh);
        }
      }
      else if (prim_type == "Sphere")
      {
        if (Graphics::MeshLibrary::Has_Mesh("Sphere"))
        {
          meshes_.push_back(*Graphics::MeshLibrary::Get_Mesh("Sphere"));
        }
        else
        {
          Shapes::Sphere sphere = Shapes::Sphere();
          Graphics::Mesh* mesh = Graphics::MeshLibrary::Add_Mesh("Sphere", sphere.Get_Vertices(), sphere.Get_Indices());
          meshes_.push_back(*mesh);
        }
      }
    }
    if (sprite.HasMember("Texture"))
    {
      std::string path = sprite["Texture"].GetString();
      Graphics::Texture* texture = Graphics::TextureLibrary::Get_Texture(path);
      if (!texture)
        std::cout << "Unable to find texture: " << path << std::endl;
      else
      {
        texture->type = "texture_diffuse";
        for (int i = 0; i < meshes_.size(); ++i)
          meshes_[i].textures_.push_back(texture);
      }
    }
  }

  if (sprite.HasMember("Ambient"))
  {
    const rapidjson::Value& amb = sprite["Ambient"];
    material_.ambient_ = glm::vec3(amb[0].GetFloat(), amb[1].GetFloat(), amb[2].GetFloat());
  }
  if (sprite.HasMember("Diffuse"))
  {
    const rapidjson::Value& diff = sprite["Diffuse"];
    material_.diffuse_ = glm::vec3(diff[0].GetFloat(), diff[1].GetFloat(), diff[2].GetFloat());
  }
  if (sprite.HasMember("Specular"))
  {
    const rapidjson::Value& spec = sprite["Specular"];
    material_.specular_ = glm::vec3(spec[0].GetFloat(), spec[1].GetFloat(), spec[2].GetFloat());
  }
  if (sprite.HasMember("Shininess"))
  {
    material_.shininess_ = sprite["Shininess"].GetFloat();
  }
}

void Model::Write(std::ofstream& file)
{
  size_t model_size = model_path_.size();
  file.write(reinterpret_cast<const char*>(&model_size), sizeof(size_t));
  if (model_size > 0)
    file.write(model_path_.c_str(), model_size);
  size_t mesh_size = meshes_.size();
  file.write(reinterpret_cast<char*>(&mesh_size), sizeof(size_t));
  for (unsigned i = 0; i < meshes_.size(); ++i)
  {
    meshes_[i].Write(file);
  }
  file.write(reinterpret_cast<char*>(&material_), sizeof(Graphics::Material));
  std::string shader_name = shader_->Name();
  size_t name_size = shader_name.size();
  file.write(reinterpret_cast<char*>(&name_size), sizeof(size_t));
  file.write(shader_name.c_str(), name_size);

  //size_t tex_size = textures_.size();
  //file.write(reinterpret_cast<const char*>(&tex_size), sizeof(size_t));
  //for (unsigned i = 0; i < textures_.size(); ++i)
  //{
  //  std::string tex_name = textures_[i]->Name();
  //  size_t name_size = tex_name.size();
  //  file.write(reinterpret_cast<char*>(&name_size), sizeof(size_t));
  //  file.write(tex_name.c_str(), name_size);
  //
  //  //size_t type_size = textures_[i]->type.size();
  //  //file.write(reinterpret_cast<char*>(&type_size), sizeof(size_t));
  //  //file.write(textures_[i]->type.c_str(), type_size);
  //}
}

void Model::Read(std::fstream& file)
{
  size_t model_size = 0;
  file.read(reinterpret_cast<char*>(&model_size), sizeof(size_t));
  if (model_size > 0)
  {
    std::string model_name;
    model_name.reserve(model_size);
    file.read(&model_name[0], model_size);
    model_path_ = std::string(model_name.c_str(), model_size);
    const Model& model = ModelLibrary::Get_Model(model_path_);
    *this = model;
    meshes_.clear();
    //load_model(model_path_);
  }

  size_t mesh_size = 0;
  file.read(reinterpret_cast<char*>(&mesh_size), sizeof(size_t));
  for (unsigned i = 0; i < mesh_size; ++i)
  {
    size_t name_size = 0;
    file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));

    std::string name;
    name.reserve(name_size);
    file.read(&name[0], name_size);
    std::string name_ = std::string(name.c_str(), name_size);
    Graphics::Mesh* mesh = Graphics::MeshLibrary::Get_Mesh(name_);
    mesh->Read(file);
    meshes_.push_back(*mesh);
  }
  file.read(reinterpret_cast<char*>(&material_), sizeof(Graphics::Material));
  size_t name_size = 0;
  file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
  std::string name;
  name.reserve(name_size);
  file.read(&name[0], name_size);
  shader_ = Graphics::Shaders::Get_Shader(std::string(name.c_str(), name_size));

  //size_t tex_size = 0;
  //file.read(reinterpret_cast<char*>(&tex_size), sizeof(size_t));
  //for (unsigned i = 0; i < tex_size; ++i)
  //{
  //  size_t name_size = 0;
  //  file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
  //  std::string name;
  //  name.reserve(name_size);
  //  file.read(&name[0], name_size);
  //  Add_Texture(Graphics::TextureLibrary::Get_Texture(std::string(name.c_str(), name_size)));
  //}
}


void Model::load_model(const std::string& path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "Error:ASSIMP: " << importer.GetErrorString() << std::endl;
    return;
  }
  int i = static_cast<int>(path.size());
  while (path[i] != '/')
  {
    --i;
  }
  prefix_ = std::string(path, 0, i);
  int count = 0;
  while (prefix_[i] != '/')
  {
    --i;
    ++count;
  }
  model_path_ = std::string(path, i + 1, count - 1);
  prefix_ += "/";


  process_node(scene->mRootNode, scene);
}


void Model::process_node(aiNode* node, const aiScene* scene)
{
  for (unsigned i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(*process_mesh(mesh, scene));
  }

  for (unsigned i = 0; i < node->mNumChildren; i++)
  {
    process_node(node->mChildren[i], scene);
  }
}


Graphics::Mesh* Model::process_mesh(aiMesh* ai_mesh, const aiScene* scene)
{
  std::vector<Graphics::Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Graphics::Texture*> textures;

  std::string mesh_name(ai_mesh->mName.C_Str());

  // Process vertices
  for (unsigned i = 0; i < ai_mesh->mNumVertices; i++)
  {
    aiVector3D vert_vec = ai_mesh->mVertices[i];
    glm::vec3 glm_vert_vec(vert_vec.x, vert_vec.y, vert_vec.z);
    aiVector3D norm_vec = ai_mesh->mNormals[i];
    glm::vec3 glm_norm_vec(norm_vec.x, norm_vec.y, norm_vec.z);

    glm::vec2 tex_vec(0.0f, 0.0f);
    if (ai_mesh->mTextureCoords[0])
    {
      tex_vec = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y);

    }
    glm::vec3 tangent(0.f, 0.f, 0.f);
    glm::vec3 bitangent(0.f, 0.f, 0.f);
    if (ai_mesh->HasTangentsAndBitangents())
    {
      aiVector3D tan = ai_mesh->mTangents[i];
      tangent = glm::vec3(tan.x, tan.y, tan.z);
      aiVector3D bitan = ai_mesh->mBitangents[i];
      bitangent = glm::vec3(bitan.x, bitan.y, bitan.z);
    }

    Graphics::Vertex vertex = { glm_vert_vec, glm::vec3(1.0f, 0.f, 0.f), tex_vec, glm_norm_vec, tangent, bitangent };
    vertices.push_back(vertex);
  }

  // Process indices
  for (unsigned i = 0; i < ai_mesh->mNumFaces; i++)
  {
    aiFace face = ai_mesh->mFaces[i];
    for (unsigned j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  // Process material
  if (ai_mesh->mMaterialIndex >= 0)
  {
    aiMaterial* material = scene->mMaterials[ai_mesh->mMaterialIndex];

    std::vector<Graphics::Texture*> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Graphics::Texture*> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Graphics::Texture*> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
  }
  Graphics::Mesh* mesh = Graphics::MeshLibrary::Add_Mesh(mesh_name, vertices, indices);

  mesh->textures_ = textures;
  return mesh;
}


std::vector<Graphics::Texture*> Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name)
{
  std::vector<Graphics::Texture*> textures;
  for (unsigned i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned j = 0; j < textures_loaded.size(); j++)
    {
      if (std::strcmp(textures_loaded[j]->path.data(), str.C_Str()) == 0)
      {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip)
    {
      Graphics::Texture *texture = new Graphics::Texture(std::string(prefix_) + str.C_Str());
      texture->path = str.C_Str();
      texture->type = type_name;
      textures.push_back(texture);
      textures_loaded.push_back(texture);
    }
  }
  return textures;
}


Model& Model::operator=(const Model& other)
{
  meshes_ = other.meshes_;

  material_ = other.material_;
  shader_ = other.shader_;
  alpha_ = other.alpha_;
  prefix_ = other.prefix_;
  textures_loaded = other.textures_loaded;
  model_path_ = other.model_path_;

  return *this;
}