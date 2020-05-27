#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <filesystem>
#include <iterator>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

//physics
#include <reactphysics3d.h>

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



// imgui
#include <imgui/imgui.h>
//#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imfilebrowser.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// json
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/reader.h>


//LUA
#define SOL_ALL_SAFETIES_ON 1

#ifdef _DEBUG
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#endif

#define METERS(x) (x / 2.f)