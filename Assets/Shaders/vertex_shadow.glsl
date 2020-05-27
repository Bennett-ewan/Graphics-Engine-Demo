#version 330 core

#define MAX_LIGHTS 12

uniform mat4 WorldView, WorldProj, Model;
uniform vec3 pos_offset;
uniform mat4 lightSpaceMatrix;

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;
layout (location = 4) in vec3 vertex_tangent;
layout (location = 5) in vec3 vertex_bitangent;


struct PointLight
{
	vec3 pos;
	float intensity;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
	float linear;
	float quadratic;

	mat4 spaceMatrix;
};

struct DirectionalLight
{
	vec3 direction;
	float intensity;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform int num_point_lights, num_dir_lights;
uniform PointLight pointlights[MAX_LIGHTS];
uniform DirectionalLight dirlight[MAX_LIGHTS];


out VS_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	mat3 TBN;
} vs_out;

//out vec3 vs_position;
//out vec3 vs_color;
//out vec2 vs_texcoord;
//out vec3 vs_normal;
//out vec3 vs_tangent;
//out vec3 vs_bitangent;
//out mat3 vs_TBN;



void main()
{ 
	gl_Position = (lightSpaceMatrix * Model * vec4(vertex_pos, 1.f));
}