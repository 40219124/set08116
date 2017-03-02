

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

#ifndef MATERIAL
#define MATERIAL
struct material{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

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