#version 450

uniform vec3 eye_pos;

layout(location = 0) in vec4 trans_pos;

layout(location = 0) out vec4 colour;

void main () {
	vec3 direction = trans_pos.xyz - eye_pos;
	float dist = distance(trans_pos.xyz, eye_pos);
	dist /= 300.0;
	colour = vec4(0.0, 0.0, 0.0, 1.0);
	colour.r = dist;
}