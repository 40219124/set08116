#version 440

uniform sampler2D cubemap;

layout(location = 0) in vec2 tex_coord_frag;

layout(location = 0) out vec4 colour;

void main() {
	colour = texture(cubemap, tex_coord_frag);
	colour.a = 1.0;
}