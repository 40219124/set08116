#version 450

uniform sampler2D tex;

layout(location = 0) in vec4 trans_pos;
layout(location = 1) in vec2 tex_coord_frag;

layout(location = 0) out vec4 colour;

void main() {
	/*vec3 tex_space = ((trans_pos/trans_pos.w).xyz)/2.0 + 0.5;
	vec2 tex_coords = vec2(tex_space.x, tex_space.z);*/

	vec3 screen_space = gl_FragCoord.xyz;
	vec2 tex_coords = vec2(1.0 - screen_space.x/1280.0, screen_space.y/720.0);

	colour = 0.8 * texture(tex, tex_coords);
	colour.a = 1.0;
}