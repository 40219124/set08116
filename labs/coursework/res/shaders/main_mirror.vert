#version 450 

uniform mat4 MVP;

layout(location = 0) in vec3 position;
layout(location = 10) in vec2 tex_coord_vert;

layout(location = 0) out vec4 trans_pos;
layout(location = 1) out vec2 tex_coord_frag;

void main() {
	gl_Position = (MVP * vec4(position, 1.0));

	trans_pos = (MVP * vec4(position, 1.0));
	tex_coord_frag = tex_coord_vert;
}