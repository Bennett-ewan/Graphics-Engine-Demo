#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 12) out;

out VS_OUT {
	vec3 color;
	vec3 normal;
}gs_in[];

uniform vec3 cam_front;

bool IsFront(vec2 A, vec2 B, vec2 C)
{
    return 0 < (A.x * B.y - B.x * A.y) + (B.x * C.y - C.x * B.y) + (C.x * A.y - A.x * C.y);
}




void main()
{
	vec2 v0 = gs_in[0].gl_Position.xy;
    vec2 v1 = gs_in[1].gl_Position.xy;
    vec2 v2 = gs_in[2].gl_Position.xy;
    vec2 v3 = gs_in[3].gl_Position.xy;
    vec2 v4 = gs_in[4].gl_Position.xy;
    vec2 v5 = gs_in[5].gl_Position.xy;
    
    if (IsFront(v0, v2, v4)) {
        if (!IsFront(v0, v1, v2)) EmitEdge(v0, v2);
        if (!IsFront(v2, v3, v4)) EmitEdge(v2, v4);
        if (!IsFront(v0, v4, v5)) EmitEdge(v4, v0);
    } 
}