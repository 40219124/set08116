#version 440

struct directional_light{
	vec4 ambient_intesity;
	vec4 light_colour;
	vec3 direction;
};

struct point_light{
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct spot_light{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};

struct material{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

uniform sampler2D tex;
uniform vec3 eye_pos;
uniform point_light point;
uniform material mat;

layout(location = 0) in vec3 world_pos;
layout(location = 1) in vec3 trans_normal;
layout(location = 2) in vec2 tex_coord_frag;

layout(location = 0) out vec4 colour;

void main () {
	vec3 light_dir = normalize(point.position - world_pos);
	float light_dist = distance(point.position, world_pos);
	float att = 1.0f / (point.quadratic * pow(light_dist, 2.0f) + point.linear * light_dist + point.constant);
	vec4 point_col = point.light_colour * att;

	vec4 diffuse = max(dot(trans_normal, light_dir), 0.0f) * (mat.diffuse_reflection * point_col);
	vec3 view_dir = normalize(eye_pos - world_pos);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = pow(max(dot(trans_normal, half_vector), 0.0f), mat.shininess) * (mat.specular_reflection * point_col);

	vec4 tex_col = texture(tex, tex_coord_frag);
	colour = (mat.emissive + diffuse) * tex_col + specular;
	colour.a = 1.0f;
}