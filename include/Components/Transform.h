#pragma once

#include <Components\Component.h>

class Transform : public Component
{
public:
  Transform();
  ~Transform();
  Transform(const Transform& other);

  Component* Clone() const;

  void Init();
  void Tick(float dt);
  void DInit();

  void Desearialize(rapidjson::Document& document);
  void Write(std::ofstream& file);
  void Read(std::fstream& file);

  void Move(const glm::vec3& translation) { this->translation += translation; }

  const glm::vec3& Translation() const { return translation; }
  void Translation(const glm::vec3& translation);
  const glm::vec3& Scale() const { return scale; }
  void Scale(const glm::vec3& scale) { this->scale = scale; is_dirty = true; }
  const glm::vec3& Rotation() { return rotation; }
  void Rotation(const glm::vec3& rotation) { this->rotation = rotation; }
  const float& RotationX() const { return rotation.x; }
  void RotationX(const float& rotation) { 
    this->rotation.x = rotation; 
    if (this->rotation.x > 360) this->rotation.x -= 360;
    if (this->rotation.x < -360) this->rotation.x += 360; 
    is_dirty = true; }
  const float& RotationY() const { return rotation.y; }
  void RotationY(const float& rotation) { 
    this->rotation.y = rotation; 
    if (this->rotation.y > 360) this->rotation.y -= 360;
    if (this->rotation.y < -360) this->rotation.y += 360;
    is_dirty = true; }
  const float& RotationZ() const { return rotation.z; }
  void RotationZ(const float& rotation) { 
    this->rotation.z = rotation; 
    if (this->rotation.z > 360) this->rotation.z -= 360;
    if (this->rotation.z < -360) this->rotation.z += 360;
    is_dirty = true; }

  const glm::mat4& Matrix() {
    if (is_dirty) calc_matrix();
    return matrix;
  }

  void Matrix(const glm::mat4& matrix) {
    this->matrix = matrix;
    this->translation = matrix[3];
  }

private:
  glm::vec3 translation;
  glm::vec3 scale;
  glm::vec3 rotation;
  bool is_dirty;
  glm::mat4 matrix;

  void calc_matrix();
};