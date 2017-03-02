

#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
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

vec4 calculateDirectionalLight (in directional_light light, in material mate, in vec3 normal, in vec3 view_dir, in vec4 tex_col) {
	vec4 ambient = mate.diffuse_reflection * light.ambient_intensity;
	
	float k_diff = max(dot(normal, light.light_dir), 0.0f);
	vec4 diffuse = k_diff * (mate.diffuse_reflection * light.light_colour);

	vec3 half_vector = normalize(light.light_dir + view_dir);
	float k_spec = pow(max(dot(normal, half_vector), 0.0f), mate.shininess);
	vec4 specular = k_spec * (mate.specular_reflection * light.light_colour);

	vec4 col = (mate.emissive + ambient + diffuse) * tex_col + specular;
	col.a = 1.0f;
	return col;
}