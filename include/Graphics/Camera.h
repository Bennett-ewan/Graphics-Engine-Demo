#pragma once

#define MIN_FOV 25.f
#define MAX_FOV 150.f

class System;

namespace Graphics
{
  class Shader;
  namespace Camera
  {
    System* Init();
    void Set_Uniforms();
    void Set_Uniforms(Shader* shader);

    void Direction(const glm::vec3& direction);
    const glm::vec3& Direction();
    void Eye(const glm::vec3& eye);
    const glm::vec3& Eye();
    void FOV(const float& fov);
    const float& FOV();
    void Sensitivity(const float& sensitivity);
    const float& Sensitivity();
    void Move(const glm::vec3& dir);

    void Move_Speed(const float& move_speed);
    const float& Move_Speed();

    void Read(std::fstream& file);
    void Write(std::ofstream& file);
  }
}