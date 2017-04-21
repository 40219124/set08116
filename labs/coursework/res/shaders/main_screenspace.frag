#version 440

uniform sampler2D tex;
uniform float value;
layout (location = 0) in vec2 tex_coords_frag;

layout (location = 0) out vec4 colour;

void main() {
	//float value = 0.003;
	/*vec4 tl = texture(tex, vec2(tex_coords_frag.x - value, tex_coords_frag.y - value));
	vec4 tc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y - value));
	vec4 tr = texture(tex, vec2(tex_coords_frag.x + value, tex_coords_frag.y - value));
	vec4 ml = texture(tex, vec2(tex_coords_frag.x - value, tex_coords_frag.y));
	vec4 mc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y));
	vec4 mr = texture(tex, vec2(tex_coords_frag.x + value, tex_coords_frag.y));
	vec4 bl = texture(tex, vec2(tex_coords_frag.x - value, tex_coords_frag.y - value));
	vec4 bc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y - value));
	vec4 br = texture(tex, vec2(tex_coords_frag.x + value, tex_coords_frag.y - value));*/
	
	//colour = (tl + tc + tr + ml + mc + mr + bl + bc + br)/9.0;
	
	/*float offset = value;
	if (tex_coords_frag.x < 0.5){
		offset *= -1;
	}
	colour = texture(tex, vec2(0.5 + offset, tex_coords_frag.y));*/
	float new_x = tex_coords_frag.x + sin(tex_coords_frag.y * 3.14 * 5)/40.0;
	if (new_x < 1 && new_x > 0){
		colour = texture(tex, vec2(new_x, tex_coords_frag.y));
	}
	else {
		colour = vec4(0.0);
	}
	
	/*colour = texture(tex, tex_coords_frag);
	colour = vec4(1.0) - colour;*/
	colour.a = 1.0;
}