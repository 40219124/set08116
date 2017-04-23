#version 450

uniform sampler2D tex;

layout(location = 0) in vec2 tex_coord_frag;

layout(location = 0) out vec4 colour;

void main () {
	colour = texture(tex, tex_coord_frag);
}