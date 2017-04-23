#version 450

layout(location = 0) in vec3 position;
layout(location = 10) in vec2 tex_coord_vert;

layout(location = 0) out vec2 tex_coord_frag;

void main() {
	gl_Position = mat4(1.0) * vec4(position, 1.0);
	tex_coord_frag = tex_coord_vert;
}