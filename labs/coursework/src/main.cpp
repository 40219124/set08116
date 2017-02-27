#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> sphereRing;
map<string, mesh> sphereRing2;
map<string, mesh> sphereRing3;
map<string, mesh> sphereRing4;
mesh skyBox;
map<mesh*, texture*> texs;
map<mesh*, mesh*> meshHierarchy;
directional_light dLight;
point_light pLight;
spot_light sLight;
texture tex;
effect eff;
target_camera target_c;
free_camera free_c;
uint cam_state = 0;
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
//make a map of spheres
void makeSphereStructure(map<string, mesh>* sphereStructure, float sphereCount) {
	// Create the parent sphere
	(*sphereStructure)["sphere0"] = mesh(geometry_builder().create_sphere(10, 20, vec3(2.5f)));
	(*sphereStructure)["sphere0"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	(*sphereStructure)["sphere0"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	(*sphereStructure)["sphere0"].get_material().set_shininess(25.0f);
	(*sphereStructure)["sphere0"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// Add relevant data to other maps
	texs[&(*sphereStructure)["sphere0"]] = &tex;
	//(*sphereHierarchy)["sphere0"] = "parent";
	meshHierarchy[&(*sphereStructure)["sphere0"]] = nullptr;

	mesh *sphere;

	//loop to generate x amount of spheres
	for (int i = 0; i < sphereCount; ++i) {
		//creates an index for the map
		string name = "sphere" + (to_string(i + 1));
		(*sphereStructure)[name] = mesh(geometry_builder().create_sphere());
		sphere = &(*sphereStructure)[name];
		// sets material properties
		sphere->get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		sphere->get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		sphere->get_material().set_shininess(25.0f);
		sphere->get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// rotates the sphere on the y-axis, and moves to circumference of ring
		sphere->get_transform().rotate(eulerAngleY(2.0f * i * pi<float>() / sphereCount));
		sphere->get_transform().translate(vec3((*sphereStructure)[name].get_transform().get_transform_matrix() * vec4(/*sphere_count/4.0f*/10.0f, 0.0f, 0.0f, 1.0f)));
		// maps a texture to the sphere's name
		texs[sphere] = &tex;
		// sets the sphere's parent object
		//(*sphereHierarchy)[name] = "sphere0";
		meshHierarchy[sphere] = &(*sphereStructure)["sphere0"];
	}
}

bool load_content() {
	// Loads in a texture   
	tex = texture("textures/check_1.png");

	skyBox = mesh(geometry_builder().create_box(vec3(200.0f)));
	skyBox.get_material().set_emissive(vec4(0.2f, 0.2f, 0.2f, 1.0f));
	texs[&skyBox] = &tex;

	//sphereRing["parent"] = mesh(geometry_builder().create_plane());
	//sphereRing["plane"] = mesh(geometry_builder().create_plane());
	//sphereRing["plane"].get_transform().translate(vec3(0.0f, -1.0f, 0.0f));

	makeSphereStructure(&sphereRing, 200.0f);
	makeSphereStructure(&sphereRing2, 30.0f);
	makeSphereStructure(&sphereRing3, 15.0f);
	makeSphereStructure(&sphereRing4, 25.0f);

	sphereRing2["sphere0"].get_transform().translate(vec3(40.0f, 20.0f, 30.0f));
	sphereRing2["sphere0"].get_transform().rotate(rotate(quat(), half_pi<float>(), vec3(1.0f, 0.0f, 0.0f)));
	sphereRing2["sphere0"].get_transform().scale = vec3(0.5f);
	//meshHierarchy[&sphereRing2["sphere0"]] = &sphereRing["sphere0"];
	sphereRing3["sphere0"].get_transform().translate(vec3(40.0f, 0.0f, -30.0f));
	sphereRing3["sphere0"].get_transform().scale = vec3(0.8f);
	//meshHierarchy[&sphereRing3["sphere0"]] = &sphereRing["sphere0"];
	sphereRing4["sphere0"].get_transform().translate(vec3(-40.0f, -20.0f, -30.0f));
	sphereRing4["sphere0"].get_transform().scale = vec3(0.3f);
	//meshHierarchy[&sphereRing4["sphere0"]] = &sphereRing["sphere0"];

	//meshHierarchy[&skyBox] = &sphereRing["sphere0"]; //<------------- motion sickness can be found here
	// Set point light properties
	pLight.move(vec3(0.0f, 10.0f, 10.0f));
	pLight.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pLight.set_range(20.0f); 
	// Set directional light properties
	dLight.set_ambient_intensity(vec4(0.0f, 0.2f, 0.0f, 1.0f));     
	dLight.set_direction(normalize(vec3(0.0f, 1.0f, 0.0f)));
	dLight.set_light_colour(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	// Set spot light properties
	sLight.set_direction(normalize(vec3(1.0f, -0.5f, 0.0f)));
	sLight.set_position(vec3(-20.0f, 5.0f, 0.0f));
	sLight.set_light_colour(vec4(1.0f, 8.0f, 0.1f, 1.0f));
	sLight.set_range(100.0f);

	// Load in shaders
	eff.add_shader("shaders/coursework.vert", GL_VERTEX_SHADER);
	vector<string> frags{ "shaders/coursework.frag", "shaders/directional.frag",
		"shaders/point.frag", "shaders/spot.frag" };
	eff.add_shader(frags, GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();



	// Set target camera properties 
	target_c.set_position(vec3(0.0f, 20.0f, 20.0f));
	target_c.set_target(vec3(0.0f, 0.0f, 0.0f));
	target_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	// Set free camera properties
	free_c.set_position(vec3(20.0f, 20.0f, 20.0f));
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
	float speed = 20.0f * delta_time;
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
		freeCamHelp(vec3(0.0f, 0.0f, 0.0f));
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
//transform the spheres in a sphere map
void transform_spheres(float delta_time, float time_total, map<string, mesh> *sphere_structure) {
	//string to mathematically aquire spheres
	string name;
	//the number of sphereRing -1
	float spheres = sphere_structure->size() - 1;

	//360 degrees. Shouldn't edit
	float full_circle = two_pi<float>();
	//sphere wave variable manipulation
	float waves_per_circle = 3.0f;	//number of sin waves in the vertical
	float wibble_speed = 0.2f;		//speed at which the spheres travel the sin arc
	float amplitude = pi<float>();			//radians around z axis (pi/2 goes to poles)
	float circling_speed = 1.0f;	//radians per second
	//ring radius transformations
	float waves_per_ring = 14.0f;	//number of sin waves in the horizontal 
	float change = 0.6f;			// percentage the radius fluctuates by
	float width_disable = 1.0f;		//0 for no radial fluctuation, 1 for regular radial fluctuation
	float shrink_factor = spheres / (12.0f + spheres);

	vec3 radius = vec3(13.0f, 0.0f, 0.0f);
	vec3 calculated_radius;
	quat rotq;
	float distFromO;
	mesh *sphere;
	//for loop to access each sphere and transform it
	for (int i = 0; i < spheres; ++i) {
		name = ("sphere" + to_string(i + 1));
		sphere = &(*sphere_structure)[name];
		//maths for spherical wibbling
		rotq = sphere->get_transform().orientation;																									//initialise rotation to the sphere's current rotation
		rotq = rotate(rotq, amplitude * sin((i / spheres) * waves_per_circle * full_circle + time_total * wibble_speed), vec3(0.0f, 0.0f, 1.0f));				//adds rotation around z axis to give effect of a sphere
		calculated_radius = radius * (1.0f + change * width_disable * sin((i * waves_per_ring * full_circle / spheres) + time_total * 2.0f * wibble_speed));	//calculates radius size based on sin fluctuations
		sphere->get_transform().position = rotq * calculated_radius;																					//multiplies radius by rotation to get sphere position
																																								//to change sphere scale based on distance from the origin
		distFromO = calculated_radius.x;
		sphere->get_transform().scale = vec3(shrink_factor * pi<float>() * distFromO / spheres);

		//--------------maths for cylindrical sphere wibbling------------------
		/*sphereRing[name].get_transform().position = (vec3(sphereRing[name].get_transform().position.x,
		amplitude * sin((i / spheres) * waves_per_circle * full_circle + time_total * wibble_speed),
		sphereRing[name].get_transform().position.z));*/
		//---------------------------------------------------------------------
	}

	//to move the centre sphere from side to side
	float centre_movement = 0.0f;
	/*(*sphere_structure)["sphere0"].get_transform().position = vec3(centre_movement * sin(time_total),
		0.0f,
		0.0f);*/

		//rotate centre (parent) sphere
	(*sphere_structure)["sphere0"].get_transform().rotate(eulerAngleY(delta_time * circling_speed));
}

bool update(float delta_time) {
	//print fps
	cout << 1.0f / delta_time << endl;

	static float time_total = 0.0f;
	time_total += delta_time;
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

	transform_spheres(delta_time, time_total, &sphereRing);
	transform_spheres(delta_time, time_total, &sphereRing2);
	transform_spheres(delta_time, time_total, &sphereRing3);
	transform_spheres(delta_time, time_total, &sphereRing4);

	//sphereRing["sphere0"].get_transform().orientation = (rotate(quat(), delta_time / 2.0f, vec3(1.0f, 0.0f, 0.0f)) * sphereRing["sphere0"].get_transform().orientation);

	return true;
}
// Aquire the transformation matrix of a child object
void transformHierarchy(mesh *currentMesh, mat4 &M, mat3 &N) {
	M = currentMesh->get_transform().get_transform_matrix();
	N = currentMesh->get_transform().get_normal_matrix();
	mesh *cMesh = meshHierarchy[currentMesh];
	while (cMesh != nullptr) {
		M = cMesh->get_transform().get_transform_matrix() * M;
		N = cMesh->get_transform().get_normal_matrix() * N;
		cMesh = meshHierarchy[cMesh];
	}
}
// Give the render function the appropriate information based on the currently in-use camera
void renderCams(mat4 &V, mat4 &P) {
	switch (cam_state) {
	case(0):
		V = target_c.get_view();
		P = target_c.get_projection();
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(target_c.get_position()));
		break;
	case(1):
		V = free_c.get_view();
		P = free_c.get_projection();
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(free_c.get_position()));
		break;
	default:
		V = target_c.get_view();
		P = target_c.get_projection();
		//set camera location, eye_pos
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(target_c.get_position()));
		break;
	}
}
//Render a map of spheres
void renderSpheres(map<string, mesh> *sphereStructure, mat4 V, mat4 P) {
	mat4 M;
	mat4 MVP;
	mat3 N;
	for (pair<const string, mesh> &item : *sphereStructure) {
		// get the hierarchy of transformations associated with the object
		mesh* currentMesh = &item.second;
		transformHierarchy(currentMesh, M, N);

		// Create MVP matrix
		MVP = P * (V * M);
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		//bind the texture which shares the name of the mesh 
		renderer::bind(*texs[currentMesh], 0);
		glUniform1i(eff.get_uniform_location("tex"), 0);
		//bind material
		renderer::bind((*currentMesh).get_material(), "mat");

		// Render geometry
		renderer::render(*currentMesh);
	}
}

bool render() {
	// Bind effect
	renderer::bind(eff);
	//bind light
	renderer::bind(pLight, "point");
	renderer::bind(dLight, "direct");
	renderer::bind(sLight, "spot");
	 
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 MVP;
	mat3 N;
	transformHierarchy(&skyBox, M, N);
	renderCams(V, P);
	MVP = P * (V * M);
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(skyBox.get_transform().get_normal_matrix()));

	renderer::bind(skyBox.get_material(), "mat");
	renderer::bind(*texs[&skyBox], 0);
	glUniform1i(eff.get_uniform_location("tex"), 0);

	glDisable(GL_CULL_FACE);
	renderer::render(skyBox);
	glEnable(GL_CULL_FACE);

	renderSpheres(&sphereRing, V, P);
	renderSpheres(&sphereRing2, V, P);
	renderSpheres(&sphereRing3, V, P);
	renderSpheres(&sphereRing4, V, P);
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