#pragma once

namespace Graphics
{
  class Shader;
  class Mesh;

  class Light
  {
  public:
    enum class Type
    {
      Point,
      Direction,
      None
    };

    Light(float intensity, const glm::vec3& color);
    ~Light();

    virtual void Send(Shader& shader, unsigned index) = 0;
    virtual void Write(std::ofstream& file) = 0;
    virtual void Read(std::fstream& file) = 0;

    const glm::vec3& Diffuse() const { return diffuse; }
    void Diffuse(const glm::vec3& diffuse) { this->diffuse = diffuse; }
    const float& Intensity() const { return intensity; }
    void Intensity(const float& intensity) { this->intensity = intensity; }
    const glm::vec3& Ambient() const { return ambient; }
    void Ambient(const glm::vec3& ambient) { this->ambient = ambient; }
    const glm::vec3& Specular() const { return specular; }
    void Specular(const glm::vec3& specular) { this->specular = specular; }

    const glm::mat4& Matrix() const { return lightSpaceMatrix; }

    const Type& Get_Type() const { return type; }
  protected:
    float intensity;
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
    Type type;

    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
  };


  class PointLight : public Light
  {
  public:
    PointLight(const glm::vec3& pos = { 0.f, 0.f, 0.f }, float intensity = 1.f, const glm::vec3& color = { 0.5f, 0.5f, 0.5f });
    ~PointLight();
    void Send(Shader& shader, unsigned index);

    void Write(std::ofstream& file);
    void Read(std::fstream& file);

    const glm::vec3& Position() const { return position; }
    void Position(const glm::vec3& pos) { 
      position = pos; 
      lightView = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f));
      lightSpaceMatrix = lightProjection * lightView;
    }

    const float& Constant() const { return constant; }
    void Constant(const float& constant) { this->constant = constant; }
    const float& Linear() const { return linear; }
    void Linear(const float& linear) { this->linear = linear; }
    const float& Quadratic() const { return quadratic; }
    void Quadratic(const float& quadratic) { this->quadratic = quadratic; }

    void Draw();
  protected:
    glm::vec3 position;
    Graphics::Mesh* mesh;
    float constant;
    float linear;
    float quadratic;
  };

  class DirectionalLight : public Light
  {
  public:
    DirectionalLight(const glm::vec3& dir = { 1.f, 0.f, 0.f }, float intensity = 1.f, const glm::vec3& color = { 0.5f, 0.5f, 0.5f });
    ~DirectionalLight();
    void Send(Shader& shader, unsigned index);

    void Write(std::ofstream& file);
    void Read(std::fstream& file);

    const glm::vec3& Direction() const { return direction; }
    void Direction(const glm::vec3& dir) { direction = dir; }

    void Draw();
  protected:
    glm::vec3 direction;
    Graphics::Mesh* mesh;

  };
}

