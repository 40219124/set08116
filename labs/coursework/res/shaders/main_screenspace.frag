#version 440

uniform sampler2D tex;

layout (location = 0) in vec2 tex_coords_frag;

layout (location = 0) out vec4 colour;

void main() {
	vec4 tl = texture(tex, vec2(tex_coords_frag.x - 1, tex_coords_frag.y - 1));
	vec4 tc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y - 1));
	vec4 tr = texture(tex, vec2(tex_coords_frag.x + 1, tex_coords_frag.y - 1));
	vec4 ml = texture(tex, vec2(tex_coords_frag.x - 1, tex_coords_frag.y));
	vec4 mc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y));
	vec4 mr = texture(tex, vec2(tex_coords_frag.x + 1, tex_coords_frag.y));
	vec4 bl = texture(tex, vec2(tex_coords_frag.x - 1, tex_coords_frag.y - 1));
	vec4 bc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y - 1));
	vec4 br = texture(tex, vec2(tex_coords_frag.x + 1, tex_coords_frag.y - 1));
	
	colour = (tl + tc + tr + ml + mc + mr + bl + bc + br)/9.0;

	/*colour = texture(tex, tex_coords_frag);
	colour = vec4(1.0) - colour;*/
	colour.a = 1.0;
}