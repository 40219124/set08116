

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

vec4 calculateSpotLight (in spot_light light, in material mate, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_col) {
	float light_distance = distance(light.position, position);
	vec3 light_dir = normalize(light.position - position);
	float att = 1.0 / (light.quadratic * pow(light_distance, 2.0) + light.linear * light_distance + light.constant);
	float intens = max(dot(-light.direction, light_dir), 0.0);
	vec4 spot_col = light.light_colour * intens * att;

	float k = max(dot(light_dir, normal), 0.0);
	vec4 diffuse = k * (mate.diffuse_reflection * spot_col);

	vec3 half_vector = normalize(light_dir + view_dir);
	k = pow(max(dot(half_vector, normal), 0.0), mate.shininess);
	vec4 specular = k * (mate.specular_reflection * spot_col);

	vec4 col = (mate.emissive + diffuse) * tex_col + specular;
	col.a = 1.0;
	return col;
}