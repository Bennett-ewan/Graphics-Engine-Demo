#version 330 core


uniform mat4 WorldView, WorldProj, Model;
uniform vec3 pos_offset;
uniform mat4 lightSpaceMatrix;


layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;
layout (location = 4) in vec3 vertex_tangent;
layout (location = 5) in vec3 vertex_bitangent;


out VS_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	mat3 TBN;
	vec4 posLightSpace;
} vs_out;




void main()
{ 
	vs_out.position = vec4(Model * vec4(vertex_pos + pos_offset, 1.f)).xyz;
	vs_out.texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
	vs_out.normal = mat3(Model) * vertex_normal;

	vec3 T = normalize(vec3(Model * vec4(vertex_tangent, 0.0)));
	vec3 B = normalize(vec3(Model * vec4(vertex_bitangent, 0.0)));
	vec3 N = normalize(vec3(Model * vec4(vertex_normal, 0.0)));
	vs_out.TBN = mat3(T, B, N);

	vs_out.posLightSpace = lightSpaceMatrix * vec4(vs_out.position, 1.0);

	gl_Position = (WorldProj * WorldView * Model * vec4(vertex_pos + pos_offset, 1.f));
}