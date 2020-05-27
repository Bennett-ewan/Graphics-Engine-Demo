#version 330

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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
uniform Material material;
uniform PointLight pointlight;
uniform vec3 camera_pos;
//uniform vec3 light_pos;


vec3 calculate_ambient(Material material)
{
	return material.ambient;
}

vec3 calculate_diffuse(Material material, vec3 vs_position, vec3 vs_normal, vec3 light_pos)
{
	vec3 light_dir = normalize(light_pos - vs_position);
	float diffuse = clamp(dot(light_dir, normalize(vs_normal)), 0, 1);
	return material.diffuse * diffuse;
}

vec3 calculate_specular(Material material, vec3 vs_position, vec3 vs_normal, vec3 light_pos, vec3 camera_pos)
{
	vec3 light_dir = normalize(vs_position - light_pos);
	vec3 reflect_dir = normalize(reflect(light_dir, normalize(vs_normal)));
	vec3 view_dir = normalize(camera_pos - vs_position);
	float specular = pow(max(dot(view_dir, reflect_dir), 0), 35);
	return material.specular * specular;
}

void main()
{
	vec3 ambient = calculate_ambient(material);
	vec3 diffuse = calculate_diffuse(material, vs_position, vs_normal, pointlight.pos);
	vec3 specular = calculate_specular(material, vs_position, vs_normal, pointlight.pos, camera_pos);


	fs_color = vec4(pointlight.color * vs_color, 1.f);
    
	//if (has_texture == 1)
	//	fs_color = texture(material.diffuse_tex1, vs_texcoord) * (vec4(ambient, 1.f), vec4(diffuse, 1.f), vec4(specular, 1.f));
	//else
	//	fs_color = vec4(vs_color, 1.f) * (vec4(ambient, 1.f), vec4(diffuse, 1.f), vec4(specular, 1.f));

	//if (has_texture == 1)
	//	fs_color = texture(material.texture_diffuse1, vs_texcoord);
	//else
	//	fs_color = vec4(vs_color, 1.f);
}