#version 440


uniform sampler2D textures[4];

uniform sampler2D blend_map;


layout(location = 0) in vec2 tex_coord;

layout(location = 10) out vec4 colour;

void main() {
	vec4 col1 = texture(textures[0], tex_coord);
	vec4 col2 = texture(textures[1], tex_coord);
	vec4 col3 = texture(textures[2], tex_coord);
	vec4 col4 = texture(textures[3], tex_coord);

	vec4 blendvalue = texture(blend_map, tex_coord);

	vec4 mixing = mix(col1, col1, blendvalue.r);
	mixing = mix(mixing, col2, blendvalue.g);
	mixing = mix(mixing, col3, blendvalue.b);
	mixing = mix(mixing, col4, blendvalue.a);

	colour = mixing;
}