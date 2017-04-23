#version 450

uniform vec3 eye_pos;

layout(location = 0) in vec4 trans_pos;

layout(location = 0) out vec4 colour;

void main () {
	float dist = trans_pos.x * trans_pos.x +  trans_pos.y * trans_pos.y +  trans_pos.z * trans_pos.z;
	dist /= 10000.0;
	colour = vec4(0.0, 0.0, 0.0, 1.0);
	colour.r = dist;
}