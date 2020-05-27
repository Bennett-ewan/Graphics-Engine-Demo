#pragma once


class System;
class Model;

typedef std::map<std::string, Model*> Models;

namespace ModelLibrary
{
  System* Init();

  const Models& Get_Models();
  const Model& Get_Model(const std::string& name);

}