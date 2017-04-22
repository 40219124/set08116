#version 440

uniform sampler2D tex;
uniform float value;
layout (location = 0) in vec2 tex_coords_frag;

layout (location = 0) out vec4 colour;

void main() {
	// blur effect
	/*vec4 tl = texture(tex, vec2(tex_coords_frag.x - value, tex_coords_frag.y - value));
	vec4 tc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y - value));
	vec4 tr = texture(tex, vec2(tex_coords_frag.x + value, tex_coords_frag.y - value));
	vec4 ml = texture(tex, vec2(tex_coords_frag.x - value, tex_coords_frag.y));
	vec4 mc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y));
	vec4 mr = texture(tex, vec2(tex_coords_frag.x + value, tex_coords_frag.y));
	vec4 bl = texture(tex, vec2(tex_coords_frag.x - value, tex_coords_frag.y - value));
	vec4 bc = texture(tex, vec2(tex_coords_frag.x, tex_coords_frag.y - value));
	vec4 br = texture(tex, vec2(tex_coords_frag.x + value, tex_coords_frag.y - value));
	colour = (tl + tc + tr + ml + mc + mr + bl + bc + br)/9.0;*/
	
	// no x effect
	/*float offset = value;
	if (tex_coords_frag.x < 0.5){
		offset *= -1;
	}
	colour = texture(tex, vec2(0.5 + offset, tex_coords_frag.y));*/

	// wiggle effect
	/*float new_y = tex_coords_frag.y + value;
	if (new_y < -1.0){
		new_y += 1.0;
	}
	float new_x = tex_coords_frag.x + sin(new_y * 3.14 * 5)/40.0;
	if (new_x < 1 && new_x > 0){
		colour = texture(tex, vec2(new_x, tex_coords_frag.y));
	}
	else {
		colour = vec4(0.0);
	}*/
	
	// negative effect
	/*colour = texture(tex, tex_coords_frag);
	colour = vec4(1.0) - colour;*/

	// bulge effect (not really functional)
	/*float new_x = 0.5 - (sin(tex_coords_frag.x * (3.1415)) / 2.0);
	float new_y = 0.5 - (sin(tex_coords_frag.y * (3.1415)) / 2.0);
	colour = texture(tex, vec2(new_x, new_y));*/

	// new bulge
	/*float disc_max = 0.50 + value;
	float disc_min = 0.50 - value;
	float mod_x = tex_coords_frag.x - 0.5;
	float mod_y = tex_coords_frag.y - 0.5;
	float dist = distance(vec2(0.0,0.0), vec2(mod_x, mod_y));

	float new_x = 0.5 + mod_x * dist;
	float new_y = 0.5 + mod_y * dist;
	colour = texture(tex, vec2(new_x, new_y));*/

	// spiral
	float mod_x = tex_coords_frag.x - 0.5;
	float mod_y = tex_coords_frag.y - 0.5;
	float dist = distance(vec2(0.0,0.0), vec2(mod_x, mod_y));
	float rads = (/*distance(0.5, 0.5) -*/clamp(0.5 - dist, 0.0, 1.0)) * 3.1415 * value;
	mat2 rot = mat2(cos(rads), sin(rads), -sin(rads), cos(rads));
	vec2 new_tc = rot * vec2(mod_x, mod_y);
	float new_x = 0.5 + new_tc.x;
	float new_y = 0.5 + new_tc.y;
	colour = texture(tex, vec2(new_x, new_y));
	
	
	
	// what are texture coords
	//colour = texture(tex, tex_coords_frag);
	if ((tex_coords_frag.x < 0.8 && tex_coords_frag.x > 0.7) || (tex_coords_frag.y < 0.8 && tex_coords_frag.y > 0.7)){
		colour += vec4(0.5,0.5,0.0,1.0);
	}
	if ((tex_coords_frag.x < 0.3 && tex_coords_frag.x > 0.2) || (tex_coords_frag.y < 0.3 && tex_coords_frag.y > 0.2)){
		colour += vec4(0.5,0.0,0.5,1.0);
	}
	if ((new_x < 0.8 && new_x > 0.7) || (new_y < 0.8 && new_y > 0.7)){
		colour += vec4(new_x,new_y,0.0,1.0);
	}
	if ((new_x < 0.3 && new_x > 0.2) || (new_y < 0.3 && new_y > 0.2)){
		colour += vec4(new_x,0.0,new_y,1.0);
	}
	colour.a = 1.0;
}