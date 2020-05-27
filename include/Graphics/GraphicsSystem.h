#pragma once

#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1080

class System;

namespace Graphics
{


  System* Init();
  void Update(float dt);
  void Shutdown();

  bool Running();

  void Clear();
  void End_Frame();

  int Height();
  int Width();

  GLFWwindow* Window();
  const glm::mat4& Window_Scale();
  const float& Aspect_Ratio();
  const float& FPS();
  const bool& Focus();
}