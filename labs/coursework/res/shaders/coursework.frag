#version 440

#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light{
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};
#endif

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

#ifndef MATERIAL
#define MATERIAL
struct material{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

vec4 calculateDirectionalLight (in directional_light light, in material mate, in vec3 normal, in vec3 view_dir, in vec4 tex_col);

vec4 calculatePointLight (in point_light light, in material mate, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_col);

vec4 calculateSpotLight (in spot_light light, in material mate, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_col);

uniform sampler2D tex;
uniform vec3 eye_pos;
uniform point_light point;
uniform directional_light direct;
uniform spot_light spot;
uniform material mat;

layout(location = 0) in vec3 world_pos;
layout(location = 1) in vec3 trans_normal;
layout(location = 2) in vec2 tex_coord_frag;

layout(location = 0) out vec4 colour;

void main () {
	vec3 view_dir = normalize(eye_pos - world_pos);
	vec4 tex_col = texture(tex, tex_coord_frag);
	colour = vec4(0.0f);
	colour += calculateDirectionalLight(direct, mat, trans_normal, view_dir, tex_col);
	colour += calculatePointLight(point, mat, world_pos, trans_normal, view_dir, tex_col);
	colour += calculateSpotLight(spot, mat, world_pos, trans_normal, view_dir, tex_col);
	colour.a = 1.0f;
}