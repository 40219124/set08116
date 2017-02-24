#version 440

uniform mat4 MVP;
uniform mat4 M;
uniform mat3 N;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 10) in vec2 tex_coord_vert;

layout(location = 0) out vec3 world_pos;
layout(location = 1) out vec3 trans_normal;
layout(location = 2) out vec2 tex_coord_frag;

void main() {
	gl_Position = MVP * vec4(position, 1.0f);

	world_pos = (M * vec4(position, 1.0f)).xyz;
	trans_normal = N * normal;
	tex_coord_frag = tex_coord_vert;
}