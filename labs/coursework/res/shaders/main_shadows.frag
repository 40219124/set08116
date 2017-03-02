#version 450
/*
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};
#endif

vec4 calculateSpotLight (in spot_light light, in material mate, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_col);
*/
//layout(location = 0) in vec3 world_pos;

layout(location = 0) out vec4 colour;

void main () {/*
	vec3 view_dir = normalize(eye_pos - world_pos);
	vec4 tex_col = texture(tex, tex_coord_frag);
	vec3 normal2 = normalMap(trans_normal, trans_binormal, trans_tangent, normal_map, tex_coord_frag);
	
	colour = vec4(0.0f);
	colour += calculateDirectionalLight(direct, mat, normal2, view_dir, tex_col);
	colour += calculatePointLight(point, mat, world_pos, normal2, view_dir, tex_col);
	colour += calculateSpotLight(spot, mat, world_pos, normal2, view_dir, tex_col);
	colour *= castShadows(shadow_map, light_pos);*/
	colour.a = 1.0f;
}