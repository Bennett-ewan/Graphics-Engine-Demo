#version 330 core


uniform mat4 WorldView, WorldInverse, WorldProj, Model, ScaleMtx;
uniform float AspectRatio;

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;


out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;

void main()
{ 
	vs_position = vec4(Model * vec4(vertex_pos.x, vertex_pos.y, vertex_pos.z, 1.f)).xyz;
	vs_color = vertex_color;
	vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
	vs_normal = mat3(Model) * vertex_normal;

	gl_Position = (WorldProj * WorldView * Model * vec4(vertex_pos.x, vertex_pos.y, vertex_pos.z, 1.f));
	//gl_Position = Model * vec4(vertex_pos.x, vertex_pos.y, vertex_pos.z, 1.f);
}