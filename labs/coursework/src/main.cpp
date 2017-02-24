#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
map<string, texture*> texs;
map<string, string> hierarchy;
directional_light dLight;
point_light pLight;
spot_light sLight;
texture tex;
effect eff;
target_camera cam;
float time_total = 0.0f;

bool load_content() {

	//meshes["parent"] = mesh(geometry_builder().create_plane());



	//meshes["plane"] = mesh(geometry_builder().create_plane());

	//meshes["plane"].get_transform().translate(vec3(0.0f, -1.0f, 0.0f));

	meshes["sphere0"] = mesh(geometry_builder().create_sphere());

	meshes["sphere0"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	meshes["sphere0"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["sphere0"].get_material().set_shininess(25.0f);
	meshes["sphere0"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	tex = texture("textures/check_1.png");
	texs["sphere0"] = &tex;
	hierarchy["sphere0"] = "parent";

	float sphere_count = 20.0f;

	for (int i = 0; i < sphere_count; ++i) {
		string name = "sphere" + (to_string(i + 1));
		meshes[name] = mesh(geometry_builder().create_sphere());

		meshes[name].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
		meshes[name].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		meshes[name].get_material().set_shininess(25.0f);
		meshes[name].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

		meshes[name].get_transform().rotate(eulerAngleY(2.0f * i * pi<float>() / sphere_count));
		meshes[name].get_transform().translate(vec3(meshes[name].get_transform().get_transform_matrix() * vec4(/*sphere_count/4.0f*/10.0f, 0.0f, 0.0f, 1.0f)));

		texs[name] = &tex;

		hierarchy[name] = "sphere0";
	}

	pLight.move(vec3(1.0f, 5.0f, 5.0f));
	pLight.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pLight.set_range(20.0f);

	// Load in shaders
	eff.add_shader("shaders/coursework.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/coursework.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();



	// Set camera properties
	cam.set_position(vec3(10.0f, 5.0f, 10.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {
	// Update the camera
	time_total += delta_time;
	string name;
	float spheres = meshes.size() - 1;
	for (int i = 0; i < spheres; ++i) {
		name = ("sphere" + to_string(i + 1));
		meshes[name].get_transform().position = (vec3(meshes[name].get_transform().position.x,
			3.0f * sin((2 * i / spheres) * 3.0f * pi<float>() + time_total * 7.0f),
			meshes[name].get_transform().position.z));
		//meshes[name].get_transform().rotate(eulerAngleY(time_total));
	}
	meshes["sphere0"].get_transform().rotate(eulerAngleY(delta_time));
	cam.update(delta_time);
	return true;
}

mat4 transformHierarchyM(string first, mat4 M) {
	if (hierarchy[first] != "parent") {
		M = meshes[first].get_transform().get_transform_matrix() * M;
		return transformHierarchyM(hierarchy[first], M);
	}
	else {
		return meshes[first].get_transform().get_transform_matrix() * M;
	}
}

mat3 transformHierarchyN(string first, mat3 N) {
	if (hierarchy[first] != "parent") {
		N = meshes[first].get_transform().get_normal_matrix() * N;
		return transformHierarchyN(hierarchy[first], N);
	}
	else {
		return meshes[first].get_transform().get_normal_matrix() * N;
	}
}

bool render() {
	// Bind effect
	renderer::bind(eff);
	for (pair<string, mesh> item : meshes) {
		mesh m = item.second;
		// Create MVP matrix
		mat4 M = mat4(1.0f);
		M = transformHierarchyM(item.first, M);
		mat4 V = cam.get_view();
		mat4 P = cam.get_projection();
		mat4 MVP = P * V * M;
		// Set MVP matrix uniform
		//N = transformHierarchyN(item.first, N);
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		//glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(meshes[item.first].get_transform().get_normal_matrix()));
		mat3 N = mat3(1.0f);
		N = transformHierarchyN(item.first, N);
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		renderer::bind(*texs[item.first], 0);
		glUniform1i(eff.get_uniform_location("tex"), 0);
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

		renderer::bind(m.get_material(), "mat");
		renderer::bind(pLight, "point");

		// Render geometry
		renderer::render(m);
	}
	return true;
}

void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	cout << "\\o\n |\\\n /\\" << endl;
	// Run application
	application.run();
}