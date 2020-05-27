#include <stdafx.h>

#include <Graphics/Camera.h>
#include <Core/System.h>
#include <Graphics/GraphicsSystem.h>
#include <Graphics/ShaderSystem.h>
#include <Graphics/Shader.h>

#include <Input/InputSystem.h>

namespace Graphics
{
  namespace Camera
  {
    class CameraSystem : public System
    {
    public:
      CameraSystem();
      ~CameraSystem() {}

      void Init() {}
      void Update(float dt);
      void Shutdown() {}

      glm::mat4 World_Proj();
      glm::mat4 World_View();

      void Set_Uniforms();
      void Set_Uniforms(Shader* shader);
      void Set_Editor_Uniforms();
      void Set_Editor_Uniforms(Shader* shader);

      const float& Spin() const { return spin; }
      void Spin(const float& spin) { this->spin = spin; }
      const float& Tilt() const { return tilt; }
      void Tilt(const float& tilt) { this->tilt = tilt; }
      const float& Zoom() const { return zoom; }
      void Zoom(const float& zoom) { this->zoom = zoom; }
      const float& Sensitivity() const { return sensitivity; }
      void Sensitivity(const float& sensitivity) { this->sensitivity = sensitivity; }
      const glm::vec3& Eye() const { return eye; }
      void Eye(const glm::vec3& eye) { this->eye = eye; }
      const glm::vec3& Direction() const { return cam_front; }
      void Direction(const glm::vec3& direction);
      const float& FOV() const { return fov; }
      void FOV(const float& fov) { this->fov = glm::clamp(fov, MIN_FOV, MAX_FOV); }
      const float& Move_Speed() const { return move_speed; }
      void Move_Speed(const float& move_speed) { this->move_speed = move_speed; }

      void Move(const glm::vec3& x) { eye += x; }

      const float& Far_Plane() const { return back; }
      void Far_Plane(const float& back) { this->back = back; }
      const float& Near_Plane() const { return front; }
      void Near_Plane(const float& front) { this->front = front; }

      void Read(std::fstream& file);
      void Write(std::ofstream& file);

    private:
      float spin, tilt;
      float tx, ty, zoom;
      float rx, ry;
      float front, back;
      float viewHeight;
      float fov;
      float sensitivity;
      float move_speed;
      glm::vec3 eye;

      glm::vec3 world_up;
      glm::vec3 cam_front;

      friend class Scene;
    };

    CameraSystem::CameraSystem() : spin(0.0f), tilt(20.0f), tx(0.0f), ty(0.0f), zoom(5.0f), ry(0.2f), fov(45.f),
      front(0.1f), back(1000.f), eye{ 0, 0, METERS(5.0) }, viewHeight(2.0f), sensitivity(0.1f),
      world_up(0.0f, 1.0f, 0.0f), cam_front(0.0f, 0.0f, -1.0f), move_speed(8.f)
    {
      float width = (float)Graphics::Width();
      float height = (float)Graphics::Height();
      rx = ry * (width / height);
    }

    void CameraSystem::Direction(const glm::vec3& direction)
    {
      cam_front = glm::normalize(direction);
      tilt = glm::degrees(asin(cam_front.y));

      spin = glm::degrees(asin((-cam_front.z / cos(glm::radians(tilt))))) + 180;
      //dir.x = cos(glm::radians(spin)) * cos(glm::radians(tilt));
      //dir.y = sin(glm::radians(tilt));
      //dir.z = sin(glm::radians(spin)) * cos(glm::radians(tilt));
    }

    glm::mat4 CameraSystem::World_Proj()
    {
      float width = static_cast<float>(Graphics::Width());
      float height = static_cast<float>(Graphics::Height());
      glm::mat4 proj = glm::perspective(glm::radians(fov), width / height, front, back);
      return proj;
    }

    glm::mat4 CameraSystem::World_View()
    {
      glm::mat4 view(1.0f);
      glm::mat4 rot(1.f);

      view = glm::lookAt(eye, eye + cam_front, world_up);
      return view;
    }

    void CameraSystem::Set_Uniforms()
    {
      glm::mat4 WorldProj = World_Proj();
      glm::mat4 WorldView = World_View();
      Shader* core_shader = Graphics::Shaders::Get_Shader("Core");

      core_shader->Use();
      core_shader->Set_Uniform(WorldProj, "WorldProj");
      core_shader->Set_Uniform(WorldView, "WorldView");
      core_shader->Set_Uniform(eye, "camera_pos");
      core_shader->Set_Uniform(cam_front, "camera_front");
      core_shader->Unuse();
    }

    void CameraSystem::Set_Uniforms(Shader* shader)
    {
      glm::mat4 WorldProj = World_Proj();
      glm::mat4 WorldView = World_View();

      shader->Use();
      shader->Set_Uniform(WorldProj, "WorldProj");
      shader->Set_Uniform(WorldView, "WorldView");
      shader->Set_Uniform(eye, "camera_pos");
      shader->Set_Uniform(cam_front, "camera_front");
      shader->Unuse();
    }

    void CameraSystem::Set_Editor_Uniforms()
    {
      glm::mat4 WorldView(1.f);
      WorldView = glm::translate(WorldView, glm::vec3(tx, ty, -zoom));
      WorldView = glm::rotate(WorldView, glm::radians(tilt), glm::vec3(1.f, 0.f, 0.f));
      WorldView = glm::rotate(WorldView, glm::radians(spin), glm::vec3(0.f, 0.f, 1.f));
      WorldView = glm::translate(WorldView, -eye);

      glm::mat4 WorldProj = World_Proj();
      Shader* shader = Graphics::Shaders::Get_Shader("Editor");

      shader->Use();
      shader->Set_Uniform(WorldProj, "WorldProj");
      shader->Set_Uniform(WorldView, "WorldView");
      shader->Set_Uniform(eye, "camera_pos");
      shader->Unuse();
    }

    void CameraSystem::Set_Editor_Uniforms(Shader* shader)
    {
      glm::mat4 WorldView(1.f);
      WorldView = glm::translate(WorldView, glm::vec3(tx, ty, -zoom));
      WorldView = glm::rotate(WorldView, glm::radians(tilt), glm::vec3(1.f, 0.f, 0.f));
      WorldView = glm::rotate(WorldView, glm::radians(spin), glm::vec3(0.f, 0.f, 1.f));
      WorldView = glm::translate(WorldView, -eye);

      glm::mat4 WorldProj = World_Proj();
      shader->Use();
      shader->Set_Uniform(WorldProj, "WorldProj");
      shader->Set_Uniform(WorldView, "WorldView");
      shader->Set_Uniform(eye, "camera_pos");
      shader->Unuse();
    }


    void CameraSystem::Update(float dt)
    {
      if (Input::Button_Down("Ctrl"))
        return;
      if (Input::Button_Down("Up"))
        eye += (glm::vec3(cam_front.x, 0, cam_front.z) * dt * move_speed);
      if (Input::Button_Down("Down"))
        eye += (-glm::vec3(cam_front.x, 0, cam_front.z) * dt * move_speed);
      if (Input::Button_Down("Left")) {
        glm::vec3 left(cam_front.z, 0, -cam_front.x);
        eye += (left * dt * move_speed);
      }
      if (Input::Button_Down("Right")) {
        glm::vec3 right(-cam_front.z, 0, cam_front.x);
        eye += (right * dt * move_speed);
      }
      if (Input::Button_Down("Space"))
        eye.y += move_speed * dt;
      if (Input::Button_Down("Shift"))
        eye.y -= move_speed * dt;

      glm::vec2 scroll = Input::Mouse_Scroll();
      if (scroll.y != 0) {
        fov += scroll.y;
        fov = glm::clamp(fov, 25.f, 150.f);
      }

      if (Input::Button_Down("LeftMouse") && Input::Mouse_Moved()) {
        const glm::vec2& dM = Input::Delta_Pos();
        int width = Graphics::Width();
        int height = Graphics::Height();
        spin += dM.x * sensitivity;
        tilt -= dM.y * sensitivity;

        tilt = glm::clamp(tilt, -90.f, 90.f);
        glm::vec3 dir;
        dir.x = cos(glm::radians(spin)) * cos(glm::radians(tilt));
        dir.y = sin(glm::radians(tilt));
        dir.z = sin(glm::radians(spin)) * cos(glm::radians(tilt));
        cam_front = glm::normalize(dir);
      }
    }

    void CameraSystem::Read(std::fstream& file)
    {
      file.read(reinterpret_cast<char*>(&eye[0]), sizeof(glm::vec3));
      file.read(reinterpret_cast<char*>(&cam_front[0]), sizeof(glm::vec3));
      Direction(cam_front);
      file.read(reinterpret_cast<char*>(&sensitivity), sizeof(float));
      file.read(reinterpret_cast<char*>(&fov), sizeof(float));
    }

    void CameraSystem::Write(std::ofstream& file)
    {
      file.write(reinterpret_cast<const char*>(&eye[0]), sizeof(glm::vec3));
      file.write(reinterpret_cast<const char*>(&cam_front[0]), sizeof(glm::vec3));
      file.write(reinterpret_cast<const char*>(&sensitivity), sizeof(float));
      file.write(reinterpret_cast<const char*>(&fov), sizeof(float));
    }

    /*************************************************************/
    /*                        ECS Instance                       */
    /*************************************************************/

    static CameraSystem* camera = nullptr;

    System* Init()
    {
      static CameraSystem cam;
      camera = &cam;
      return camera;
    }

    void Set_Uniforms()
    {
      camera->Set_Uniforms();
    }

    void Set_Uniforms(Shader* shader)
    {
      camera->Set_Uniforms(shader);
    }

    void Direction(const glm::vec3& direction)
    {
      camera->Direction(direction);
    }

    const glm::vec3& Direction()
    {
      return camera->Direction();
    }

    void Eye(const glm::vec3& eye)
    {
      camera->Eye(eye);
    }

    const glm::vec3& Eye()
    {
      return camera->Eye();
    }

    void FOV(const float& fov)
    {
      camera->FOV(fov);
    }

    const float& FOV()
    {
      return camera->FOV();
    }

    void Sensitivity(const float& sensitivity)
    {
      camera->Sensitivity(sensitivity);
    }

    const float& Sensitivity()
    {
      return camera->Sensitivity();
    }

    void Move(const glm::vec3& dir)
    {
      camera->Move(dir);
    }

    
    void Move_Speed(const float& move_speed)
    {
      camera->Move_Speed(move_speed);
    }

    const float& Move_Speed()
    {
      return camera->Move_Speed();
    }

    void Read(std::fstream& file)
    {
      camera->Read(file);
    }

    void Write(std::ofstream& file)
    {
      camera->Write(file);
    }
  }
  
}
