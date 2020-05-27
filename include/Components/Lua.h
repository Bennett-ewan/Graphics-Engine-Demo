#pragma once

#include <stdafx.h>


class GameObject;

namespace Behaviors
{
  class Lua
  {
  public:
    Lua(const std::string& script);
    ~Lua();

    void Initialize();
    void Run();
    //void Add_Object(GameObject* obj, const std::string& name);
    void Add_Object(sol::userdata obj);

    sol::state state;

  private:
    std::string script_name;
    void set_engine_data();
    void set_usertypes();
  };
}

