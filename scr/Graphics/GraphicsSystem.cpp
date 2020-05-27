#include <stdafx.h>

#include <Core/System.h>
#include <Graphics\GraphicsSystem.h>

#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>

#include <Scenes/Editor.h>

namespace Graphics
{
  static void resize_callback(GLFWwindow* window, int width, int height);
  static void window_close_callback(GLFWwindow* window);
  static void focus_callback(GLFWwindow* window, int focus);

  class GraphicsSystem : public System
  {
  public:
    GraphicsSystem();
    ~GraphicsSystem();

    void Init();

    void Update(float dt);

    void Draw();

    void Shutdown();

    bool Running() { 
      return window != nullptr && !glfwWindowShouldClose(window); 
    }

    void End_Frame() {
      glfwSwapBuffers(window);
      glFlush();
    }
    
    glm::ivec2 Get_Viewport_Size()
    {
      glm::ivec2 out;
      int size[4];
      glGetIntegerv(GL_VIEWPORT, size);

      out[0] = size[2];
      out[1] = size[3];

      return out;
    }

    void Set_View(int view_width, int view_height) { 
      if (view_width == 0)
        return;
      width = view_width; height = view_height;    
      window_scale = glm::mat4(1.f);
      window_scale = glm::scale(window_scale, glm::vec3(1.f / (width / 2.f), 1.f / (width / 2.f), 1.f / (width / 2.f)));
    }
    int Width() { return width; }
    int Height() { return height; }

    GLFWwindow* Window() const { return window; }

    const glm::mat4& Window_Scale() const { return window_scale; }

    const float& Aspect_Ratio() const{ return aspect_ratio_; }

    const float& FPS() const{ return fps_; }

    const bool& Focus() const { return focus; }
    void Focus(const bool& focus) { this->focus = focus; }
  private:
    GLFWwindow* window;
    int width, height;
    glm::mat4 window_scale;
    float aspect_ratio_;
    float fps_;
    bool focus;
  };

  GraphicsSystem::GraphicsSystem() : window(nullptr), width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT), window_scale(1.f), 
                                     aspect_ratio_((float)DEFAULT_WIDTH / (float)DEFAULT_HEIGHT), fps_(0.f), focus(true)
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(width, height, "GraphicsDemo", NULL, NULL);
    if (!window)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return;
    }
    glfwMakeContextCurrent(window); 

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    window_scale = glm::scale(window_scale, glm::vec3(1.f / (width / 2.f), 1.f / (width / 2.f), 1.f / (width / 2.f)));

    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetWindowFocusCallback(window, focus_callback);
  }
  
  GraphicsSystem::~GraphicsSystem()
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void GraphicsSystem::Init()
  {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_FRAMEBUFFER_SRGB);

    //glEnable(GL_TEXTURE_2D);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void GraphicsSystem::Update(float dt)
  {
    fps_ = 1.f / dt;

    Graphics::Clear();

    glStencilMask(0x00);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

  }

  void GraphicsSystem::Draw()
  {

  }

  void GraphicsSystem::Shutdown()
  {

  }




  /*************************************************************/
  /*                        ECS Instance                       */
  /*************************************************************/

  static GraphicsSystem* graphics_system = nullptr;

  System* Init()
  {
    static GraphicsSystem graphics;
    graphics_system = &graphics;
    return graphics_system;
  }

  void Update(float dt)
  {
    if (graphics_system)
      graphics_system->Update(dt);
  }

  void Shutdown()
  {
    if (graphics_system)
      graphics_system->Shutdown();
  }

  bool Running()
  {
    if (graphics_system)
      return graphics_system->Running();
    return false;
  }

  void Clear()
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClearStencil(0);
    //glStencilMask(0x00);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  void End_Frame()
  {
    if (graphics_system)
      graphics_system->End_Frame();
  }

  int Width()
  {
    return graphics_system->Width();
  }

  int Height()
  {
    return graphics_system->Height();
  }

  GLFWwindow* Window()
  {
    return graphics_system->Window();
  }

  const glm::mat4& Window_Scale()
  {
    return graphics_system->Window_Scale();
  }

  const float& Aspect_Ratio()
  {
    return graphics_system->Aspect_Ratio();
  }

  const float& FPS()
  {
    return graphics_system->FPS();
  }

  const bool& Focus()
  {
    return graphics_system->Focus();
  }

  /*************************************************************/
  /*                   Private functions                       */
  /*************************************************************/

  static void resize_callback(GLFWwindow* window, int width, int height)
  {
    glViewport(0, 0, width, height);
    graphics_system->Set_View(width, height);
  }

  static void window_close_callback(GLFWwindow* window)
  {
    if (Editor::Unsaved_Changes())
    {
      Editor::Show_Save(true);
      glfwSetWindowShouldClose(window, GL_FALSE);
      return;
    }
    glfwSetWindowShouldClose(window, GL_TRUE);
    //glfwTerminate();
  }

  static void focus_callback(GLFWwindow* window, int focus)
  {
    graphics_system->Focus(focus);
  }
}
