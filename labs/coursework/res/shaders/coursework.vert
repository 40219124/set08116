#version 450 

uniform mat4 MVP;
uniform mat4 M;
uniform mat3 N;
uniform mat4 lMVP;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 10) in vec2 tex_coord_vert;

layout(location = 0) out vec3 world_pos;
layout(location = 1) out vec3 trans_normal;
layout(location = 2) out vec2 tex_coord_frag;
layout(location = 3) out vec4 light_pos;

void main() {
	gl_Position = MVP * vec4(position, 1.0);

	world_pos = (M * vec4(position, 1.0)).xyz;
	trans_normal = normalize(N * normal);
	tex_coord_frag = tex_coord_vert;
	light_pos = lMVP * vec4(position, 1.0);
}