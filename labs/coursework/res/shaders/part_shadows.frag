
float castShadows(in sampler2D shadow_map, in vec4 light_pos) {
    vec3 proj_coords = light_pos.xyz / light_pos.w;
    vec2 map_coords;
    map_coords.x = 0.5 * proj_coords.x + 0.5;
    map_coords.y = 0.5 * proj_coords.y + 0.5;
    if (light_pos.z < 0 || map_coords.x < 0 || map_coords.x > 1 || map_coords.y < 0 || map_coords.y > 1){
        return 1.0;
    }

	float z_pos = light_pos.z / light_pos.w;
    float depth = (texture(shadow_map, map_coords)).r;
    float dist = (light_pos.x * light_pos.x + light_pos.y * light_pos.y + light_pos.z * light_pos.z)/10000.0;
    if (depth == 0.0) {
        return 1.0;
    }
	else if (depth < dist - 0.015){
        return 0.5;
    }
	 else {
        return 1.0f;
    }

}