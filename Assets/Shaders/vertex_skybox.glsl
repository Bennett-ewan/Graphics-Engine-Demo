#version 330 core


uniform mat4 WorldView, WorldProj;
uniform vec3 offset;
uniform vec3 camera_pos;

layout (location = 0) in vec3 vertex_pos;

out vec3 vs_texcoord;

void main()
{ 
	vs_texcoord = vertex_pos;
	//vec4 pos = (WorldProj * WorldView * vec4((vertex_pos + offset) * 500, 1.f));
	vec4 pos = (WorldProj * WorldView * vec4(vertex_pos * 500, 1.f));
	gl_Position = pos.xyww;
}