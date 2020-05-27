#version 330 core

#define MAX_LIGHTS 12
#define PI 3.1415926535897932384626433832795

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float reflectiveness;
	float refraction_index;

	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_normal1;
};

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

in VS_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	mat3 TBN;
} fs_in;

out vec4 fs_color;

uniform int has_texture;
uniform int has_specular;
uniform int has_diffuse;
uniform int has_normal;
uniform vec2 texture_scale;
uniform float opacity;

uniform Material material;
uniform vec3 camera_pos;

uniform int num_point_lights, num_dir_lights;
uniform PointLight pointlights[MAX_LIGHTS];
uniform DirectionalLight dirlight[MAX_LIGHTS];

void main()
{
	

}



