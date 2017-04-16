#version 440

uniform mat4 MVP;

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 tex_coord_frag;

void main() {
	gl_Position = MVP * vec4(position, 1.0);
	tex_coord_frag = position;
}