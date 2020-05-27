#pragma once

class Model;
namespace Graphics
{
  class Shader;
  class Texture;

  class Material
  {
  public:
    Material();
    Material(const glm::vec3& ambient,
      const glm::vec3& diffuse,
      const glm::vec3& specular,
      float shininess);
    ~Material();

    void Send(Shader* program);

    const glm::vec3& Ambient() const { return ambient_; }
    void Ambient(const glm::vec3& ambient) { ambient_ = ambient; }
    const glm::vec3& Diffuse() const { return diffuse_; }
    void Diffuse(const glm::vec3& diffuse) { diffuse_ = diffuse; }
    const glm::vec3& Specular() const { return specular_; }
    void Specular(const glm::vec3& specular) { specular_ = specular; }
    const float& Shininess() const { return shininess_; }
    void Shininess(const float& shininess) { shininess_ = shininess; }
    const float& Reflectiveness() const { return reflectiveness_; }
    void Reflectiveness(const float& reflectiveness) { reflectiveness_ = reflectiveness; }
    const float& Refraction() const { return refraction_index_; }
    void Refraction(const float& refraction) { refraction_index_ = refraction; }

  private:
    glm::vec3 ambient_;
    glm::vec3 diffuse_;
    glm::vec3 specular_;
    float shininess_;
    float reflectiveness_;
    float refraction_index_;
   

    friend class Model;
  };
}