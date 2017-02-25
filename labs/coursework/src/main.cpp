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
target_camera target_c;
float time_total = 0.0f;

bool load_content() {
	// Loads in a texture
	tex = texture("textures/check_1.png");


	//meshes["parent"] = mesh(geometry_builder().create_plane());
	//meshes["plane"] = mesh(geometry_builder().create_plane());
	//meshes["plane"].get_transform().translate(vec3(0.0f, -1.0f, 0.0f));

	// Create the parent sphere
	meshes["sphere0"] = mesh(geometry_builder().create_sphere(10, 20, vec3(4.0f, 4.0f, 4.0f)));
	meshes["sphere0"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	meshes["sphere0"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["sphere0"].get_material().set_shininess(25.0f);
	meshes["sphere0"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// Add relevant data to other maps
	texs["sphere0"] = &tex;
	hierarchy["sphere0"] = "parent";

	//loop to generate x amount of spheres
	float sphere_count = 30.0f;
	for (int i = 0; i < sphere_count; ++i) {
		//creates an index for the map
		string name = "sphere" + (to_string(i + 1));
		meshes[name] = mesh(geometry_builder().create_sphere());
		// sets material properties
		meshes[name].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
		meshes[name].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		meshes[name].get_material().set_shininess(25.0f);
		meshes[name].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// rotates the sphere on the y-axis, and moves to circumference of ring
		meshes[name].get_transform().rotate(eulerAngleY(2.0f * i * pi<float>() / sphere_count));
		meshes[name].get_transform().translate(vec3(meshes[name].get_transform().get_transform_matrix() * vec4(/*sphere_count/4.0f*/10.0f, 0.0f, 0.0f, 1.0f)));
		// maps a texture to the sphere's name
		texs[name] = &tex;
		// sets the sphere's parent object
		hierarchy[name] = "sphere0";
	}

	// Set point light properties
	pLight.move(vec3(1.0f, 10.0f, 5.0f));
	pLight.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pLight.set_range(20.0f);

	// Load in shaders
	eff.add_shader("shaders/coursework.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/coursework.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();



	// Set camera properties
	target_c.set_position(vec3(0.0f, 20.0f, 20.0f));
	target_c.set_target(vec3(0.0f, 0.0f, 0.0f));
	target_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {
	// Update the camera

	//sphere manipulation maths
	time_total += delta_time;
	//string to mathematically aquire spheres
	string name;
	//the number of meshes -1
	float spheres = meshes.size() - 1;

	//360 degrees. Shouldn't edit
	float full_circle = 2.0f * pi<float>();
	//sphere wave variable manipulation
	float waves_per_circle = 2.0f;
	float wibble_speed = 2.0f;
	float amplitude = 0.50f;
	float circling_speed = 1.0f;
	//ring radius transformations
	float change = 0.3f;  // percentage

	vec3 radius = vec3(8.0f, 0.0f, 0.0f);
	quat rotq;
	//for loop to access each sphere and transform it
	for (int i = 0; i < spheres; ++i) {
		name = ("sphere" + to_string(i + 1));
		
		//maths for spherical wibbling
		rotq = meshes[name].get_transform().orientation;
		rotq = rotate(rotq, amplitude * sin((i / spheres) * waves_per_circle * full_circle + time_total * wibble_speed), vec3(0.0f, 0.0f, 1.0f)); //rotates around z axis to give effect of a sphere
		meshes[name].get_transform().position = (rotq * vec4(radius * (1.0f + change * sin((i * waves_per_circle * full_circle / spheres) + time_total * 2.0f * wibble_speed)), 1.0f));

		//maths for cylindrical sphere wibbling
		/*meshes[name].get_transform().position = (vec3(meshes[name].get_transform().position.x,
			amplitude * sin((i / spheres) * waves_per_circle * full_circle + time_total * wibble_speed),
			meshes[name].get_transform().position.z));*/
	}

	// -------- to move the centre sphere from side to side
	/*meshes["sphere0"].get_transform().position = vec3(5.0f * sin(time_total),
		0.0f,
		0.0f);*/

	//rotate centre (parent) sphere
	meshes["sphere0"].get_transform().rotate(eulerAngleY(delta_time * circling_speed));

	target_c.update(delta_time);
	return true;
}

//A recursive function to get all of the hierarchial transformations to the TRANSFORMATION matrix
mat4 transformHierarchyM(string first, mat4 M) {
	// if not the main parent...
	if (hierarchy[first] != "parent") {
		//multiply M by the matrix of the current mesh and return the recursive operation
		M = meshes[first].get_transform().get_transform_matrix() * M;
		return transformHierarchyM(hierarchy[first], M);
	}
	//if the main parent...
	else {
		// return N by the matrix of the current mesh, and do not cal the function again
		return meshes[first].get_transform().get_transform_matrix() * M;
	}
}

//A recursive function to get all of the hierarchial transformations to the NORMAL matrix
mat3 transformHierarchyN(string first, mat3 N) {
	// if not the main parent...
	if (hierarchy[first] != "parent") {
		//multiply N by the matrix of the current mesh and return the recursive operation
		N = meshes[first].get_transform().get_normal_matrix() * N;
		return transformHierarchyN(hierarchy[first], N);
	}
	//if the main parent...
	else {
		// return N by the matrix of the current mesh, and do not cal the function again
		return meshes[first].get_transform().get_normal_matrix() * N;
	}
}

bool render() {
	// Bind effect
	renderer::bind(eff);
	for (pair<string, mesh> item : meshes) {
		//set mesh to m
		mesh m = item.second;

		// Create MVP matrix
		mat4 M = mat4(1.0f);
		// get the hierarchy of transformations associated with the object
		M = transformHierarchyM(item.first, M);
		mat4 V = target_c.get_view();
		mat4 P = target_c.get_projection();
		mat4 MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		//get normal transform with matrix heirarchy changes
		mat3 N = mat3(1.0f);
		N = transformHierarchyN(item.first, N);
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
		
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(target_c.get_position()));

		//bind the texture which shares the name of the mesh
		renderer::bind(*texs[item.first], 0);
		glUniform1i(eff.get_uniform_location("tex"), 0);
		//bind material
		renderer::bind(m.get_material(), "mat");
		//bind light
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