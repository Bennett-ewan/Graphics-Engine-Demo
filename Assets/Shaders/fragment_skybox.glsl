#version 330 core

in vec3 vs_texcoord;

out vec4 fs_color;

uniform samplerCube skybox;



void main()
{
	fs_color = texture(skybox, vs_texcoord);
}

