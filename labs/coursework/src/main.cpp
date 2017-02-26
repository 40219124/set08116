#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> sphereRing;
mesh skyBox;
map<string, texture*> texs;
map<string, string> hierarchy;
directional_light dLight;
point_light pLight;
spot_light sLight;
texture tex;
effect eff;
target_camera target_c;
free_camera free_c;
uint cam_state = 0;
float time_total = 0.0f;
double mouse_x;
double mouse_y;

//focus the free cam on a target location
void freeCamHelp(vec3 target) {
	target = normalize(target - free_c.get_position());
	vec3 forward = vec3(0.0f, 0.0f, -1.0f);

	vec3 targetxz = normalize(vec3(target.x, 0.0f, target.z));
	vec3 crossy = cross(forward, targetxz);
	float yaw = 1.0f;
	if (crossy.y < 0) {
		yaw *= -1.0f;
	}
	yaw *= acos(dot(forward, targetxz));
	free_c.set_yaw(yaw);

	vec3 targetyz = rotate(quat(), -yaw, vec3(0.0f, 1.0f, 0.0f)) * target;
	vec3 crossx = cross(forward, targetyz);
	float pitch = 1.0f;
	if (crossx.x < 0) {
		pitch *= -1.0f;
	}
	pitch *= acos(dot(forward, targetyz));
	free_c.set_pitch(pitch);
}

bool load_content() {
	// Loads in a texture
	tex = texture("textures/check_1.png");

	skyBox = mesh(geometry_builder().create_box(vec3(100.0f, 100.0f, 100.0f)));
	skyBox.get_material().set_emissive(vec4(0.2f, 0.2f, 0.2f, 1.0f));

	//sphereRing["parent"] = mesh(geometry_builder().create_plane());
	//sphereRing["plane"] = mesh(geometry_builder().create_plane());
	//sphereRing["plane"].get_transform().translate(vec3(0.0f, -1.0f, 0.0f));

	// Create the parent sphere
	sphereRing["sphere0"] = mesh(geometry_builder().create_sphere(10, 20, vec3(4.0f, 4.0f, 4.0f)));
	sphereRing["sphere0"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	sphereRing["sphere0"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	sphereRing["sphere0"].get_material().set_shininess(25.0f);
	sphereRing["sphere0"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// Add relevant data to other maps
	texs["sphere0"] = &tex;
	hierarchy["sphere0"] = "parent";

	//loop to generate x amount of spheres
	float sphere_count = 50.0f;
	for (int i = 0; i < sphere_count; ++i) {
		//creates an index for the map
		string name = "sphere" + (to_string(i + 1));
		sphereRing[name] = mesh(geometry_builder().create_sphere());
		// sets material properties
		sphereRing[name].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
		sphereRing[name].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		sphereRing[name].get_material().set_shininess(25.0f);
		sphereRing[name].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// rotates the sphere on the y-axis, and moves to circumference of ring
		sphereRing[name].get_transform().rotate(eulerAngleY(2.0f * i * pi<float>() / sphere_count));
		sphereRing[name].get_transform().translate(vec3(sphereRing[name].get_transform().get_transform_matrix() * vec4(/*sphere_count/4.0f*/10.0f, 0.0f, 0.0f, 1.0f)));
		// maps a texture to the sphere's name
		texs[name] = &tex;
		// sets the sphere's parent object
		hierarchy[name] = "sphere0";
	}

	// Set point light properties
	pLight.move(vec3(0.0f, 10.0f, 10.0f));
	pLight.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pLight.set_range(20.0f);

	// Load in shaders
	eff.add_shader("shaders/coursework.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/coursework.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();



	// Set target camera properties
	target_c.set_position(vec3(0.0f, 20.0f, 20.0f));
	target_c.set_target(vec3(0.0f, 0.0f, 0.0f));
	target_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	// Set free camera properties
	free_c.set_position(vec3(20.0f, 20.0f, -20.0f));
	free_c.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCamHelp(free_c.get_target());
	free_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	return true;
}

bool initialise() {
	glfwGetCursorPos(renderer::get_window(), &mouse_x, &mouse_y);
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

//all things relating to target cam manipulation
void target_manipulation(float delta_time) {
	//position 1, infront
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1)) {
		target_c.set_position(vec3(0.0f, 20.0f, 20.0f));
	}
	//position 2, front-right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
		target_c.set_position(vec3(20.0f, 10.0f, 20.0f));
	}
	//position 3, back-right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3)) {
		target_c.set_position(vec3(20.0f, 10.0f, -20.0f));
	}
	//position 4, back-left
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4)) {
		target_c.set_position(vec3(-20.0f, 10.0f, -20.0f));
	}
	//zoom in on plus key
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_EQUAL)) {
		target_c.set_position(target_c.get_target() - (1 - 0.2f * delta_time) * (target_c.get_target() - target_c.get_position()));
	}
	//zoon out on minus key
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_MINUS)) {
		target_c.set_position(target_c.get_target() - (1 + 0.25f * delta_time) * (target_c.get_target() - target_c.get_position()));
	}
	//update camera
	target_c.update(delta_time);
}
//free cam manipulation
void free_manipulation(float delta_time) {
	//find window sizes and proportions
	static const float screen_w = static_cast<float>(renderer::get_screen_width());
	static const float screen_h = static_cast<float>(renderer::get_screen_height());
	static const double ratio_w = quarter_pi<float>() / screen_w;
	static const double ratio_h = (quarter_pi<float>() * (screen_h / screen_w)) / screen_h;
	//get current cursor position
	double current_x;
	double current_y;
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
	//delta values for x and y
	double delta_x = current_x - mouse_x;
	double delta_y = current_y - mouse_y;
	//find the movement angle by multiplying deltas by screen ratio
	delta_x *= ratio_w;
	delta_y *= ratio_h;
	//rotate camera
	free_c.rotate(delta_x, -delta_y);

	//speed variable and movement vector
	float speed = 10.0f * delta_time;
	vec3 mov = vec3(0.0f);
	//forwards
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		mov += vec3(0.0f, 0.0f, 1.0f);
	}
	//backwards
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		mov += vec3(0.0f, 0.0f, -1.0f);
	}
	//left
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		mov += vec3(-1.0f, 0.0f, 0.0f);
	}
	//right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		mov += vec3(1.0f, 0.0f, 0.0f);
	}
	//down
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT)) {
		mov += vec3(0.0f, -1.0f, 0.0f);
	}
	//up
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
		mov += vec3(0.0f, 1.0f, 0.0f);
	}
	//speed up
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL)) {
		speed *= 3.0f;
	}
	//flip the camera if upside-down
	if ((glfwGetKey(renderer::get_window(), GLFW_KEY_H) == GLFW_PRESS)) {
		freeCamHelp(free_c.get_target());
	}
	//looks at the origin
	if ((glfwGetKey(renderer::get_window(), GLFW_KEY_O) == GLFW_PRESS)) {
		freeCamHelp(vec3(0.0f,0.0f,0.0f));
	}

	//if movement has occured, normalize the movement, then multiply speed
	if (mov != vec3(0.0f)) {
		mov = speed * normalize(mov);
		free_c.move(mov);
	}
	//reset mouse position
	mouse_x = current_x;
	mouse_y = current_y;
	//update camera
	free_c.update(delta_time);
}

bool update(float delta_time) {
	cout << 1.0f / delta_time << endl;
	//which camera type to use
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Z)) {
		cam_state = 0; //target cam
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_X)) {
		cam_state = 1; //free cam
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_C)) {
		cam_state = 2; //arc ball cam
	}
	//which camera to control
	switch (cam_state)
	{
	case (0):
		target_manipulation(delta_time);
		break;
	case (1):
		free_manipulation(delta_time);
		break;
	default:
		cam_state = 0;
		break;
	}

	//sphere manipulation maths
	time_total += delta_time;
	//string to mathematically aquire spheres
	string name;
	//the number of sphereRing -1
	float spheres = sphereRing.size() - 1;

	//360 degrees. Shouldn't edit
	float full_circle = two_pi<float>();
	//sphere wave variable manipulation
	float waves_per_circle = 5.0f;	//number of sin waves in the vertical
	float wibble_speed = 1.0f;		//speed at which the spheres travel the sin arc
	float amplitude = 1.2f;			//radians around z axis (pi/2 goes to poles)
	float circling_speed = 1.0f;	//radians per second
	//ring radius transformations
	float waves_per_ring = 4.0f;	//number of sin waves in the horizontal
	float change = 0.3f;			// percentage the radius fluctuates by
	float width_disable = 1.0f;		//0 for no radial fluctuation, 1 for regular radial fluctuation
	float shrink_factor = spheres / (12.0f + spheres);

	vec3 radius = vec3(13.0f, 0.0f, 0.0f);
	vec3 calculated_radius;
	quat rotq;
	float distFromO;
	//for loop to access each sphere and transform it
	for (int i = 0; i < spheres; ++i) {
		name = ("sphere" + to_string(i + 1));
		//maths for spherical wibbling
		rotq = sphereRing[name].get_transform().orientation;
		rotq = rotate(rotq, amplitude * sin((i / spheres) * waves_per_circle * full_circle + time_total * wibble_speed), vec3(0.0f, 0.0f, 1.0f)); //rotates around z axis to give effect of a sphere
		calculated_radius = radius * (1.0f + change * width_disable * sin((i * waves_per_ring * full_circle / spheres) + time_total * 2.0f * wibble_speed));
		sphereRing[name].get_transform().position = rotq * calculated_radius;
		distFromO = calculated_radius.x;
		sphereRing[name].get_transform().scale = vec3(shrink_factor * pi<float>() * distFromO / spheres);
		//maths for cylindrical sphere wibbling
		/*sphereRing[name].get_transform().position = (vec3(sphereRing[name].get_transform().position.x,
			amplitude * sin((i / spheres) * waves_per_circle * full_circle + time_total * wibble_speed),
			sphereRing[name].get_transform().position.z));*/
	}

	// -------- to move the centre sphere from side to side
	sphereRing["sphere0"].get_transform().position = vec3(5.0f * sin(time_total),
		0.0f,
		0.0f);

		//rotate centre (parent) sphere
	sphereRing["sphere0"].get_transform().rotate(eulerAngleY(delta_time * circling_speed));

	return true;
}

//A recursive function to get all of the hierarchial transformations to the TRANSFORMATION matrix
mat4 transformHierarchyM(string first, mat4 M) {
	// if not the main parent...
	if (hierarchy[first] != "parent") {
		//multiply M by the matrix of the current mesh and return the recursive operation
		M = sphereRing[first].get_transform().get_transform_matrix() * M;
		return transformHierarchyM(hierarchy[first], M);
	}
	//if the main parent...
	else {
		// return N by the matrix of the current mesh, and do not cal the function again
		return sphereRing[first].get_transform().get_transform_matrix() * M;
	}
}

//A recursive function to get all of the hierarchial transformations to the NORMAL matrix
mat3 transformHierarchyN(string first, mat3 N) {
	// if not the main parent...
	if (hierarchy[first] != "parent") {
		//multiply N by the matrix of the current mesh and return the recursive operation
		N = sphereRing[first].get_transform().get_normal_matrix() * N;
		return transformHierarchyN(hierarchy[first], N);
	}
	//if the main parent...
	else {
		// return N by the matrix of the current mesh, and do not cal the function again
		return sphereRing[first].get_transform().get_normal_matrix() * N;
	}
}
// Give the render function the appropriate information based on the currently in-use camera
void renderCams(mat4* V, mat4* P) {
	switch (cam_state) {
	case(0):
		*V = target_c.get_view();
		*P = target_c.get_projection();
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(target_c.get_position()));
		break;
	case(1):
		*V = free_c.get_view();
		*P = free_c.get_projection();
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(free_c.get_position()));
		break;
	default:
		*V = target_c.get_view();
		*P = target_c.get_projection();
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(target_c.get_position()));
		break;
	}
}

bool render() {
	// Bind effect
	renderer::bind(eff);

	mat4 M;
	mat4 V;
	mat4 P;
	mat4 MVP;

	M = skyBox.get_transform().get_transform_matrix();
	renderCams(&V, &P);
	MVP = P * (V * M);
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(skyBox.get_transform().get_normal_matrix()));

	renderer::bind(pLight, "point");
	renderer::bind(skyBox.get_material(), "mat");
	renderer::bind(tex, 0);
	glUniform1i(eff.get_uniform_location("tex"), 0);

	glDisable(GL_CULL_FACE);
	renderer::render(skyBox);
	glEnable(GL_CULL_FACE);

	for (pair<string, mesh> item : sphereRing) {
		//set mesh to m
		mesh m = item.second;

		// Create MVP matrix
		M = mat4(1.0f);
		// get the hierarchy of transformations associated with the object
		M = transformHierarchyM(item.first, M);
		renderCams(&V, &P);
		MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		//get normal transform with matrix heirarchy changes
		mat3 N = mat3(1.0f);
		N = transformHierarchyN(item.first, N);
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

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
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	cout << "\\o\n |\\\n /\\" << endl;
	// Run application
	application.run();
}