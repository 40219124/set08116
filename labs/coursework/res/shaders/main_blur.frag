#version 450

uniform sampler2D tex;

layout(location = 0) in vec2 tex_coords_frag;

layout(location = 0) out vec4 colour;

void main(){
	int blur_radius = 4;
	float x_size = 1.0/(1280.0 * 2.0);
	float y_size = 1.0/(720.0 * 2.0);
	vec4 col = vec4(0.0,0.0,0.0,1.0);
	for (int x = -blur_radius; x <= blur_radius; ++x){
		for (int y = -blur_radius; y <= blur_radius; ++y){
			col += texture(tex, vec2(tex_coords_frag.x + x * x_size, tex_coords_frag.y + y * y_size));
		}
	}
	col = col/pow((2*blur_radius)+1, 2);
	col.a = 1.0;
	colour = col;
}