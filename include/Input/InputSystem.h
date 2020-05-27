#pragma once

class System;

namespace Input
{
  System* Init();
  void Update(float dt);
  void Shutdown();

  bool Button_Down(const std::string& button);
  bool Button_Pressed(const std::string& button);

  const glm::vec2& Mouse_Pos();
  const glm::vec2& Delta_Pos();
  const glm::vec2& Mouse_Scroll();

  bool Mouse_Moved();
  void Reset();

  const bool& Block_Input();
  void Block_Input(const bool& block);
}