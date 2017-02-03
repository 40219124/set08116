#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <memory>

using namespace std;
using namespace graphics_framework;
using namespace glm;

mesh m;
mesh m2;
mesh m3;
mesh m4;
mesh m5;
geometry geom;
geometry geom2;
geometry geom3;
geometry geom4;
geometry geom5;
effect eff;
effect jeff;
effect steff;
target_camera cam;
texture tex;
texture tex2;

bool load_content() {
	// Construct geometry object
	// Create triangle data
	// Positions
	vector<vec3> positions{ vec3(5.0f, 1.0f, 0.0f), vec3(4.0f, -1.0f, 0.0f), vec3(6.0f, -1.0f, 0.0f) };
	//square
	vector<vec3> positions2{ vec3(-1.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f) };
	//index positions
	vector<vec3> positions3{ vec3(0.0f,0.0f,0.0f), vec3(1.0f,0.0f,0.0f), vec3(1.0f,1.0f,0.0f), vec3(0.0f,1.0f,0.0f),
		vec3(0.0f,0.0f,1.0f), vec3(1.0f,0.0f,1.0f), vec3(1.0f,1.0f,1.0f), vec3(0.0f,1.0f,1.0f) };
	//positions for geom3: back-top-right, front-top-left, front-lower-right
	vector<GLuint> indices3{
		2, 7, 5
	};
	geom4.set_type(GL_TRIANGLE_FAN);
	vector<GLuint> indices4{
		6, 2, 3, 7, 4, 5, 1, 2
	};

	geom5.set_type(GL_TRIANGLE_FAN);
	vector<vec3> indices5{ vec3(0.0f,0.0f,0.0f) };

	float divisions = 50.0f;
	for (float i = 0.0f; i < (divisions + 1.0f); ++i) {
		indices5.push_back(vec3(cosf((pi<float>()*2.0f)*i / divisions), sinf((pi<float>()*2.0f)*i / divisions), -0.2f));
	}
	// *********************************
	// Define texture coordinates for triangle
	vector<vec2> tex_pos{ vec2(0.5f, 0.0f), vec2(0.0f, -1.0f), vec2(1.0f, -1.0f) };
	//square
	vector<vec2> tex_pos2{ vec2(0.0f, 0.0f), vec2(0.0f, -1.0f), vec2(1.0f, -1.0f),
		vec2(1.0f, -1.0f), vec2(1.0f, 0.0f), vec2(0.0f, 0.0f) };
	//geom3
	vector<vec2> tex_pos3{
		vec2(0.0f,0.0f),  vec2(0.0f,0.0f), /*2*/vec2(1.0f, 0.0f), vec2(0.0f,0.0f),  vec2(0.0f,0.0f),  /*5*/vec2(0.5f, -1.0f), vec2(0.0f,0.0f),  /*7*/vec2(0.0f,0.0f)
	};
	vector<vec4> colours4;
	for (float i = 0.0f; i < 8.0f; ++i) {
		colours4.push_back(vec4(0.9f / i, 0.3f / i, 1.0f / i, 1.0f));
	}

	vector<vec2> tex_pos5{ vec2(0.5f, 0.5f) };
	for (float i = 0.0f; i < (divisions + 1.0f); ++i) {
		tex_pos5.push_back(vec2((cosf((pi<float>()*2.0f)*i / divisions)+1)/2.0f, (sinf((pi<float>()*2.0f)*i / divisions)-1)/2.0f));
	}
	// *********************************
	// Add to the geometry
	geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom2.add_buffer(positions2, BUFFER_INDEXES::POSITION_BUFFER);
	geom3.add_buffer(positions3, BUFFER_INDEXES::POSITION_BUFFER);
	geom4.add_buffer(positions3, BUFFER_INDEXES::POSITION_BUFFER);
	geom5.add_buffer(indices5, BUFFER_INDEXES::POSITION_BUFFER);
	// *********************************
	geom3.add_index_buffer(indices3);
	geom4.add_index_buffer(indices4);
	// Add texture coordinate buffer to geometry
	geom.add_buffer(tex_pos, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom2.add_buffer(tex_pos2, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom3.add_buffer(tex_pos3, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom4.add_buffer(colours4, BUFFER_INDEXES::COLOUR_BUFFER);
	geom5.add_buffer(tex_pos5, BUFFER_INDEXES::TEXTURE_COORDS_0);

	// *********************************

	// Create mesh object
	m = mesh(geom);
	m2 = mesh(geom2);
	m3 = mesh(geom3);
	m4 = mesh(geom4);
	m4.get_transform().position = (vec3(-1.0f, -2.0f, 0.0f));
	m5 = mesh(geom5);
	m5.get_transform().position = (vec3(0.0f, 3.0f, 0.0f));
	// Load in texture shaders here
	eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
	eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);

	jeff.add_shader("shaders/basic.vert", GL_VERTEX_SHADER);
	jeff.add_shader("shaders/basic.frag", GL_FRAGMENT_SHADER);

	steff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
	steff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);
	// *********************************
	// Build effect
	eff.build();

	jeff.build();

	steff.build();
	// Load texture "textures/sign.jpg"
	tex = texture("textures/sign.jpg");
	tex2 = texture("textures/grass.jpg");
	// *********************************

	// Set camera properties
	cam.set_position(vec3(10.0f, 10.0f, 10.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);

	return true;
}

bool update(float delta_time) {
	// Update the camera
	cam.update(delta_time);
	return true;
}

bool render() {
	// Bind effect
	renderer::bind(eff);
	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), // Location of uniform
		1,                               // Number of values - 1 mat4
		GL_FALSE,                        // Transpose the matrix?
		value_ptr(MVP));                 // Pointer to matrix data

// *********************************
// Bind texture to renderer
	renderer::bind(tex, 0);
	renderer::bind(tex2, 1);
	// Set the texture value for the shader here
	glUniform1i(eff.get_uniform_location("tex"), 0);
	// *********************************

	// Render the mesh
	renderer::render(m);


	renderer::render(m2);


	renderer::render(m3);


	renderer::bind(jeff);

	mat4 M4 = m4.get_transform().get_transform_matrix();
	mat4 MVP4 = P * (V * M4);
	glUniformMatrix4fv(jeff.get_uniform_location("MVP"),
		1,
		GL_FALSE,
		value_ptr(MVP4));

	renderer::render(m4);

	renderer::bind(steff);
	mat4 M5 = m5.get_transform().get_transform_matrix();
	mat4 MVP5 = P * (V * M5);
	glUniformMatrix4fv(steff.get_uniform_location("MVP"),
		1,
		GL_FALSE,
		value_ptr(MVP5));
	glUniform1i(steff.get_uniform_location("tex"), 1);
	renderer::render(m5);

	return true;
}

void main() {
	// Create application
	app application("27_Texturing_Shader");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}