/*#version 450
void main () {
}*/

#version 450

uniform vec3 eye_pos;

layout(location = 0) in vec4 trans_pos;

layout(location = 0) out vec4 colour;

void main () {
	vec3 direction = trans_pos.xyz - eye_pos;
	vec3 trans_pos_small = trans_pos.xyz / trans_pos.w;
	float dist = trans_pos.x * trans_pos.x +  trans_pos.y * trans_pos.y +  trans_pos.z * trans_pos.z;
	dist /= 10000.0;
	colour = vec4(0.0, 0.0, 0.0, 1.0);
	/*if (dist > 100){
		colour.r = 1.0;
	} else if (dist > 90){
		colour.r = 0.9;
	} else if (dist > 80){
		colour.r = 0.8;
	} else if (dist > 70){
		colour.r = 0.7;
	} else if (dist > 60){
		colour.r = 0.6;
	} else if (dist > 50){
		colour.r = 0.5;
	} else if (dist > 40){
		colour.r = 0.4;
	} else if (dist > 30){
		colour.r = 0.3;
	} else if (dist > 20){
		colour.r = 0.2;
	} else if (dist > 10){
		colour.r = 0.1;
	} else {
		colour.r = 0.0;
	}*/
	colour.r = dist;
}