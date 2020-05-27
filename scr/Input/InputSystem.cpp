#include <stdafx.h>

#include <Input/InputSystem.h>
#include <Core/System.h>

#include <Graphics/GraphicsSystem.h>
namespace Input
{
  void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  void cursor_callback(GLFWwindow* window, double xpos, double ypos);
  void mouse_callback(GLFWwindow* window, int button, int action, int mods);
  void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

  class InputSystem : public System
  {
  public:

    InputSystem() : buttons_(), held_(), mouse_pos_(0.f, 0.f), delta_pos_(0.f, 0.f), mouse_moved_(false), scroll_(0.f, 0.f), blocking_(false)
    {
      glfwSetKeyCallback(Graphics::Window(), keyboard_callback);
      glfwSetCursorPosCallback(Graphics::Window(), cursor_callback);
      glfwSetMouseButtonCallback(Graphics::Window(), mouse_callback);
      glfwSetScrollCallback(Graphics::Window(), scroll_callback);

      buttons_[68] = "Right";
      held_["Right"] = false;
      pressed_["Right"] = false;

      buttons_[65] = "Left";
      held_["Left"] = false;
      pressed_["Left"] = false;

      buttons_[87] = "Up";
      held_["Up"] = false;
      pressed_["Up"] = false;

      buttons_[83] = "Down";
      held_["Down"] = false;
      pressed_["Down"] = false;

      buttons_[0] = "LeftMouse";
      held_["LeftMouse"] = false;
      pressed_["LeftMouse"] = false;

      buttons_[1] = "RightMouse";
      held_["RightMouse"] = false;
      pressed_["RightMouse"] = false;

      buttons_[73] = "ToggleInput";
      held_["ToggleInput"] = false;
      pressed_["ToggleInput"] = false;

      buttons_[341] = "Ctrl";
      held_["Ctrl"] = false;
      pressed_["Ctrl"] = false;

      buttons_[GLFW_KEY_SPACE] = "Space";
      held_["Space"] = false;
      pressed_["Space"] = false;

      buttons_[340] = "Shift";
      held_["Shift"] = false;
      pressed_["Shift"] = false;

      buttons_[GLFW_KEY_ESCAPE] = "Escape";
      held_["Escape"] = false;
      pressed_["Escape"] = false;

      buttons_[GLFW_KEY_X] = "Cut";
      held_["Cut"] = false;
      pressed_["Cut"] = false;

      buttons_[GLFW_KEY_C] = "Copy";
      held_["Copy"] = false;
      pressed_["Copy"] = false;

      buttons_[GLFW_KEY_V] = "Paste";
      held_["Paster"] = false;
      pressed_["Paste"] = false;
    }

    ~InputSystem()
    {

    }

    void Init()
    {

    }

    void Update(float dt)
    {
      glfwPollEvents();
    }

    void Shutdown()
    {

    }

    void Set_Button(int key, bool down)
    {
      try {
        std::string& name = buttons_.at(key);
        held_[name] = down;
        pressed_[name] = down;
      }
      catch (...) {}
    }


    bool Button_Down(const std::string& button)
    {
      if (blocking_) return false;

      try {
        return held_.at(button);
      }
      catch (...) {
        return false;
      }
    }

    bool Button_Pressed(const std::string& button)
    {
      if (blocking_) return false;

      try {
        return pressed_.at(button);
      }
      catch (...) {
        return false;
      }
    }

    void Pressed_Reset()
    {
      for (std::map<std::string, bool>::iterator it = pressed_.begin(); it != pressed_.end(); ++it)
      {
        it->second = false;
      }
    }

    glm::vec2 mouse_pos_;
    glm::vec2 delta_pos_;
    glm::vec2 scroll_;
    bool mouse_moved_;
    bool blocking_;

  private:
    std::map<int, std::string> buttons_;
    std::map<std::string, bool> held_;
    std::map<std::string, bool> pressed_;

  };

  /*************************************************************/
  /*                        ECS Instance                       */
  /*************************************************************/

  static InputSystem* Input_System = nullptr;

  System* Init()
  {
    static InputSystem input;
    Input_System = &input;
    return Input_System;
  }

  void Update(float dt)
  {
    Input_System->Update(dt);
  }

  void Shutdown()
  {
    Input_System->Shutdown();
  }

  bool Button_Down(const std::string& button)
  {
    return Input_System->Button_Down(button);
  }

  bool Button_Pressed(const std::string& button)
  {
    return Input_System->Button_Pressed(button);
  }

  const glm::vec2& Mouse_Pos()
  {
    return Input_System->mouse_pos_;
  }

  const glm::vec2& Delta_Pos()
  {
    return Input_System->delta_pos_;
  }

  bool Mouse_Moved()
  {
    return Input_System->mouse_moved_;
  }

  void Reset()
  {
    Input_System->mouse_moved_ = false;
    Input_System->scroll_ = glm::vec2(0.f, 0.f);

    Input_System->Pressed_Reset();
  }

  const glm::vec2& Mouse_Scroll()
  {
    return Input_System->scroll_;
  }

  const bool& Block_Input()
  {
    return Input_System->blocking_;
  }

  void Block_Input(const bool& block)
  {
    Input_System->blocking_ = block;
  }

  /*************************************************************/
  /*                   Private functions                       */
  /*************************************************************/

  void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      Input_System->Set_Button(key, true);
    }
    else if (action == GLFW_RELEASE)
    {
      Input_System->Set_Button(key, false);
    }
  }

  void cursor_callback(GLFWwindow* window, double xpos, double ypos)
  {
    int width, height;
    glfwGetWindowSize(Graphics::Window(), &width, &height);
    
    glm::vec2 pos(xpos - (width / 2), ypos - (height / 2));

    Input_System->delta_pos_ = pos - Input_System->mouse_pos_;

    Input_System->mouse_pos_ = pos;

    Input_System->mouse_moved_ = true;
  }

  void mouse_callback(GLFWwindow* window, int button, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      Input_System->Set_Button(button, true);
    }
    else if (action == GLFW_RELEASE)
    {
      Input_System->Set_Button(button, false);
    }
  }

  void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
  {
    Input_System->scroll_ = glm::vec2(xoffset, yoffset);
  }



}