#version 330 core



in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;



void main()
{
	fs_color = vec4(1.f, 1.f, 1.f, 1.f);
}