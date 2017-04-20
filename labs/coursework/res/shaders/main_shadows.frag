/*#version 450
void main () {
}*/

#version 450

uniform vec3 eye_pos;

layout(location = 0) in vec3 trans_pos;

layout(location = 0) out vec4 colour;

void main () {
	vec3 direction = trans_pos - eye_pos;
	colour = vec4(0.0, 0.0, 0.0, 1.0);
	colour.r = abs(trans_pos.z/100.0);
}