#version 450 

uniform mat4 MVP;
uniform mat4 M;

layout(location = 0) in vec3 position;

layout(location = 0) out vec4 trans_pos;

void main() {
	gl_Position = MVP * vec4(position, 1.0);
	trans_pos = (M * vec4(position, 1.0));
}