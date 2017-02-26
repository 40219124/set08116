#version 440

struct directional_light{
	vec4 ambient_intensity;
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

vec4 calculateDirectionalLight (directional_light light, material mate, vec3 normal, vec3 view_dir, vec4 tex_col) {
	vec4 ambient = mate.diffuse_reflection * light.ambient_intensity;
	
	float k_diff = max(dot(normal, vec3(0.0f,1.0f,0.0f)), 0.0f);
	vec4 diffuse = k_diff * (mate.diffuse_reflection * light.light_colour);

	vec3 half_vector = normalize(vec3(0.0f,1.0f,0.0f) + view_dir);
	float k_spec = pow(max(dot(normal, half_vector), 0.0f), mate.shininess);
	vec4 specular = k_spec * (mate.specular_reflection * light.light_colour);

	vec4 col = (mate.emissive + ambient + diffuse) * tex_col + specular;
	col.a = 1.0f;
	return col;
}

vec4 calculatePointLight (point_light light, material mate, vec3 position, vec3 normal, vec3 view_dir, vec4 tex_col) {
	float light_dist = distance(light.position, position);
	float att = 1.0f / (light.quadratic * pow(light_dist, 2.0f) + light.linear * light_dist + light.constant);
	vec4 point_col = light.light_colour * att;

	vec3 light_dir = normalize(light.position - position);
	vec4 diffuse = max(dot(normal, light_dir), 0.0f) * (mate.diffuse_reflection * point_col);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = pow(max(dot(normal, half_vector), 0.0f), mate.shininess) * (mate.specular_reflection * point_col);
	
	vec4 col = (mate.emissive + diffuse) * tex_col + specular;
	col.a = 1.0f;
	return col;
}

uniform sampler2D tex;
uniform vec3 eye_pos;
uniform point_light point;
uniform directional_light direct;
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
	//colour += calculatePointLight(point, mat, world_pos, trans_normal, view_dir, tex_col);
	colour.a = 1.0f;
}