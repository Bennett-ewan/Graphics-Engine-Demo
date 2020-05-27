#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
};

struct PointLight
{
	vec3 pos;
	float intensity;
	vec3 color;
};


in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform int has_texture;
uniform int has_specular;
uniform int has_diffuse;

uniform vec4 tint;

uniform Material material;
uniform PointLight pointlight;
uniform vec3 camera_pos;
//uniform vec3 light_pos;

uniform vec3 wireframe_color;

void main()
{
	fs_color = vec4(wireframe_color, 1.f);
}