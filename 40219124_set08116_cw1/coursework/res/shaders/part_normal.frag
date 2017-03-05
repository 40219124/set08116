
vec3 normalMap(in vec3 normal, in vec3 binormal, in vec3 tangent, in sampler2D normal_map, in vec2 tex_coord) {
	normal = normalize(normal);
	binormal = normalize(binormal);
	tangent = normalize(tangent);

	vec4 map_col = texture(normal_map, tex_coord);

	vec3 sampled_normal = 2.0 * map_col.xyz - vec3(1.0);
	
	mat3 TBN = mat3(tangent, binormal, normal);

	return normalize(TBN * sampled_normal);
}