#version 450 

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

float castShadows(in sampler2D shadow_map, in vec4 light_pos, in vec3 world_pos, in vec3 eye_pos);

vec3 normalMap(in vec3 normal, in vec3 binormal, in vec3 tangent, in sampler2D normal_map, in vec2 tex_coord);

uniform sampler2D tex;
uniform sampler2D normal_map;
uniform sampler2D shadow_map;
uniform vec3 eye_pos;
uniform point_light point;
uniform directional_light direct;
uniform spot_light spot;
uniform material mat;

layout(location = 0) in vec3 world_pos;
layout(location = 1) in vec3 trans_normal;
layout(location = 2) in vec3 trans_binormal;
layout(location = 3) in vec3 trans_tangent;
layout(location = 4) in vec2 tex_coord_frag;
layout(location = 5) in vec4 light_pos;

layout(location = 0) out vec4 colour;

void main () {
	vec3 view_dir = normalize(eye_pos - world_pos);
	vec4 tex_col = texture(tex, tex_coord_frag);
	vec3 normal2 = normalMap(trans_normal, trans_binormal, trans_tangent, normal_map, tex_coord_frag);
	vec4 colour_spot;
	colour = vec4(0.0f);
	colour += calculateDirectionalLight(direct, mat, normal2, view_dir, tex_col);
	colour += calculatePointLight(point, mat, world_pos, normal2, view_dir, tex_col);
	colour += calculateSpotLight(spot, mat, world_pos, normal2, view_dir, tex_col);
	colour *= castShadows(shadow_map, light_pos, world_pos, spot.position);
	colour.a = 1.0f;
}