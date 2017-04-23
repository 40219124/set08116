#version 440

uniform sampler2D tex;
uniform float value;
uniform int eff_state;
uniform int neg_state;
uniform int guides;

layout (location = 0) in vec2 tex_coords_frag;

layout (location = 0) out vec4 colour;

vec4 show_quarts(float x, float y){
	vec4 col = vec4(0.0,0.0,0.0,1.0);
	if ((x < 0.77 && x > 0.73) || (y < 0.77 && y > 0.73)){
		col += vec4(x, y, 0.0, 1.0);
	}
	if ((x < 0.27 && x > 0.23) || (y < 0.27 && y > 0.23)){
		col += vec4(x, 0.0, y, 1.0);
	}
	return col;
}

vec4 colour_negative(vec4 col) {
	return (vec4(1.0) - col);
}

vec4 bulge() {
	float disc_max = 0.50 + value;
	float disc_min = 0.50 - value;
	float mod_x = tex_coords_frag.x - 0.5;
	float mod_y = tex_coords_frag.y - 0.5;
	float dist = distance(vec2(0.0,0.0), vec2(mod_x, mod_y));

	float new_x = 0.5 + mod_x * dist * 1.38;
	float new_y = 0.5 + mod_y * dist * 1.38;
	vec4 col = texture(tex, vec2(new_x, new_y));
	if (guides > 0){
		col += show_quarts(tex_coords_frag.x, tex_coords_frag.y);
		col += show_quarts(new_x, new_y);
	}
	return col;
}

vec4 spiral() {
	float mod_x = tex_coords_frag.x - 0.5;
	float mod_y = tex_coords_frag.y - 0.5;
	float dist = distance(vec2(0.0,0.0), vec2(mod_x, mod_y));
	float rads = (clamp(0.5 - dist, 0.0, 1.0)) * 3.1415 * 4.0;
	mat2 rot = mat2(cos(rads), sin(rads), -sin(rads), cos(rads));
	vec2 new_tc = rot * vec2(mod_x, mod_y);
	float new_x = 0.5 + new_tc.x;
	float new_y = 0.5 + new_tc.y;
	vec4 col = texture(tex, vec2(new_x, new_y));
	if (guides > 0){
		col += show_quarts(tex_coords_frag.x, tex_coords_frag.y);
		col += show_quarts(new_x, new_y);
	}
	return col;
}

vec4 wiggle() {
	float new_y = tex_coords_frag.y + value;
	if (new_y < -1.0){
		new_y += 1.0;
	}
	float new_x = tex_coords_frag.x + sin(new_y * 3.1415 * 5)/20.0;
	vec4 col = vec4(0.0,0.0,0.0,1.0);
	if (new_x < 1 && new_x > 0){
		col += texture(tex, vec2(new_x, tex_coords_frag.y));
	}
	else {
		return col;
	}
	if (guides > 0){
		col += show_quarts(tex_coords_frag.x, tex_coords_frag.y);
		col += show_quarts(new_x, new_y);
	}
	return col;
}


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

	if (eff_state == 1){
		colour = bulge();
	} else if (eff_state == 2){
		colour = spiral();
	} else if (eff_state == 3){
		colour = wiggle();
	} else {
		colour = texture(tex, tex_coords_frag);
	}

	if (neg_state > 0){
		colour = colour_negative(colour);
	}

	colour.a = 1.0;
}