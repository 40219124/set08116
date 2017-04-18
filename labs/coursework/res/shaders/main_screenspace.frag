#version 440

uniform sampler2D tex;

layout (location = 0) in vec2 tex_coords_frag;

layout (location = 0) out vec4 colour;

void main() {
	colour = texture(tex, tex_coords_frag);
	colour.a = 1.0;
}