#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Meshes
float sphereScale = 4.0f;
map<string, mesh> sphereRing;
map<string, mesh> sphereRing2;
map<string, mesh> sphereRing3;
map<string, mesh> sphereRing4;
mesh skyBox;
mesh ground;
mesh hill;
mesh column;
// Reference maps
map<mesh*, texture*> texs;
map<mesh*, texture*> norms;
map<mesh*, mesh*> meshHierarchy;
// Lights
directional_light dLight;
point_light pLight;
spot_light sLight;
// Shadows
shadow_map shadow;
// Texture
texture tex;
texture shadowMap;
// Effects
effect eff;
effect simple_eff;
effect shadow_eff;
// Cameras
target_camera target_c;
free_camera free_c;
// camera boolean
uint cam_state = 0;
// Mouse positions
double mouse_x;
double mouse_y;
// Disable skybox (debug boolean)
bool noSky = true;

//focus the free cam on a target location
void freeCamHelp(vec3 target) {
	// Vector to target = the normalised vector from the camera to the target position
	target = normalize(target - free_c.get_position());
	// Declare the "forward" direction
	vec3 forward = vec3(0.0f, 0.0f, -1.0f);
	// The projection of the target vector onto the x&z plane
	vec3 targetxz = normalize(vec3(target.x, 0.0f, target.z));
	// Cross product of forward and xz-projection
	vec3 crossy = cross(forward, targetxz);
	// Toggle for rotating clockwise/anti-clockwise
	float yaw = 1.0f;
	// If target is clockwise of forwards reverse yaw direction
	if (crossy.y < 0) {
		yaw *= -1.0f;
	}
	// Yaw = the inverse cosine of the dot product between forward and the xz-projection (multiplied by 1 or -1)
	yaw *= acos(dot(forward, targetxz));
	// Set camera yaw
	free_c.set_yaw(yaw);
	// Rotate the vector to be on the z plane
	vec3 targetyz = rotate(quat(), -yaw, vec3(0.0f, 1.0f, 0.0f)) * target;
	// Cross product of forward and rotated vector
	vec3 crossx = cross(forward, targetyz);
	// Toggle for pitch up/down
	float pitch = 1.0f;
	// If forward is above the rotated vector
	if (crossx.x < 0) {
		pitch *= -1.0f;
	}
	// Pitch = the inverse cosine of the dot product between forward and the rotated vector (multiplied by 1 or -1)
	pitch *= acos(dot(forward, targetyz));
	// Set camera pitch
	free_c.set_pitch(pitch);
}
//make a map of spheres
void makeSphereStructure(map<string, mesh>* sphereStructure, float sphereCount) {
	// Create the parent sphere
	static mesh technosphere = mesh(geometry("models/Technosphere_4.obj"));
	static texture sphere_tex = texture("textures/sphere color.jpg");
	static texture sphere_norm = texture("textures/sphere normals.jpg");
	(*sphereStructure)["sphere0"] = technosphere;
	(*sphereStructure)["sphere0"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	(*sphereStructure)["sphere0"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	(*sphereStructure)["sphere0"].get_material().set_shininess(25.0f);
	(*sphereStructure)["sphere0"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// Add texture map information
	texs[&(*sphereStructure)["sphere0"]] = &sphere_tex;
	norms[&(*sphereStructure)["sphere0"]] = &sphere_norm;
	// Set hierarchy pair to be a null pointer
	meshHierarchy[&(*sphereStructure)["sphere0"]] = nullptr;

	// Pointer to use in loop
	mesh *sphere;
	//loop to generate x amount of spheres
	for (int i = 0; i < sphereCount; ++i) {
		//creates an index for the map
		string name = "sphere" + (to_string(i + 1));
		//(*sphereStructure)[name] = mesh(geometry_builder().create_sphere());
		(*sphereStructure)[name] = technosphere;
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
		texs[sphere] = &sphere_tex;
		norms[sphere] = &sphere_norm;
		// sets the sphere's parent object
		//(*sphereHierarchy)[name] = "sphere0";
		meshHierarchy[sphere] = &(*sphereStructure)["sphere0"];
	}
}

bool load_content() {
	// Loads in a texture   
	tex = texture("textures/check_1.png");
	static texture column_tex = texture("textures/Marble_Base_Color.jpg");
	static texture column_norm = texture("textures/Marble_Normal_OpenGL.jpg");

	// Make sky box mesh and give details
	if (!noSky) {
		/*skyBox = mesh(geometry_builder().create_box(vec3(200.0f)));
		skyBox.get_material().set_emissive(vec4(0.2f, 0.2f, 0.2f, 1.0f));
		texs[&skyBox] = &tex;*/
	}

	// Make the plane
	static texture grass_tex = texture("textures/grass01.jpg");
	static texture grass_norm = texture("textures/grass01_n.jpg");
	ground = mesh(geometry_builder().create_plane(10.0f, 10.0f));
	ground.get_transform().scale = vec3(20.0f);
	// Make hill
	hill = ground;
	ground.get_transform().translate(vec3(80.0f, 50.0f, 0.0f));
	ground.get_transform().rotate(eulerAngleZ(half_pi<float>()));
	ground.get_material().set_diffuse(vec4(0.5f, 0.5f, 0.1f, 1.0f));
	texs[&ground] = &grass_tex;
	norms[&ground] = &grass_norm;


	texs[&hill] = &grass_tex;
	norms[&hill] = &grass_norm;


	// Import the column
	column = mesh(geometry("models/Column_HP.obj"));
	texs[&column] = &column_tex;
	norms[&column] = &column_norm;
	meshHierarchy[&column] = nullptr;
	column.get_transform().position = vec3(0.0f, 0.0f, 0.0f);
	column.get_transform().scale = vec3(20.0f);

	// Make various sphere structures
	makeSphereStructure(&sphereRing, 20.0f);
	makeSphereStructure(&sphereRing2, 10.0f);
	makeSphereStructure(&sphereRing3, 10.0f);
	makeSphereStructure(&sphereRing4, 10.0f);

	// Set information for the first sphere ring
	sphereRing["sphere0"].get_transform().translate(vec3(0.0f, 1.7f, 0.0f));
	sphereRing["sphere0"].get_transform().scale = vec3(1.0f / (5.0f * sphereScale));
	meshHierarchy[&sphereRing["sphere0"]] = &column;

	// Set information for the second sphere ring
	sphereRing2["sphere0"].get_transform().translate(vec3(40.0f, 40.0f, 30.0f) * sphereScale * 0.25f);
	sphereRing2["sphere0"].get_transform().rotate(rotate(quat(), half_pi<float>(), vec3(1.0f, 0.0f, 0.0f)));
	sphereRing2["sphere0"].get_transform().scale = vec3(0.5f);
	meshHierarchy[&sphereRing2["sphere0"]] = &sphereRing["sphere0"];

	// Set information for the third sphere ring
	sphereRing3["sphere0"].get_transform().translate(vec3(40.0f, 40.0f, -30.0f) * sphereScale * 0.25f);
	sphereRing3["sphere0"].get_transform().scale = vec3(0.4f);
	meshHierarchy[&sphereRing3["sphere0"]] = &sphereRing["sphere0"];

	// Set information for the fourth sphere ring
	sphereRing4["sphere0"].get_transform().translate(vec3(-40.0f, 40.0f, -30.0f) * sphereScale * 0.25f);
	sphereRing4["sphere0"].get_transform().scale = vec3(0.3f);
	meshHierarchy[&sphereRing4["sphere0"]] = &sphereRing["sphere0"];

	//meshHierarchy[&skyBox] = &sphereRing["sphere0"]; //<------------- motion sickness can be found here

	// Set directional light properties
	bool dLightOn = true;
	dLight.set_ambient_intensity(vec4(0.01f, 0.01f, 0.01f, 1.0f));
	dLight.set_direction(normalize(vec3(0.0f, 1.0f, 0.0f)));
	dLight.set_light_colour(vec4(0.1f, 0.1f, 0.1, 1.0f));
	if (!dLightOn) {
		dLight.set_ambient_intensity(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		dLight.set_light_colour(vec4(0.0f, 0.0f, 0.0, 1.0f));
	}
	// Set point light properties
	bool pLightOn = true;
	pLight.move(vec3(0.0f, 45.0f, 00.0f));
	pLight.set_range(40.0f);
	pLight.set_light_colour(vec4(1.0f, 0.6f, 1.0f, 1.0f));
	if (!pLightOn) {
		pLight.set_light_colour(vec4(0.0f, 0.0f, 0.0f, 0.0f));
	}
	// Set spot light properties
	sLight.set_direction(normalize(vec3(1.0f, -0.0f, 0.0f)));
	sLight.set_position(vec3(-80.0f, 30.0f, 0.0f));
	sLight.set_light_colour(vec4(0.8f, 0.7f, 0.1f, 1.0f));
	sLight.set_range(160.0f);
	sLight.set_power(5.0f);
	//sLight.set_constant_attenuation(0.1f);
	//sLight.set_linear_attenuation(0.1f);
	//sLight.set_quadratic_attenuation(0.1f);

	// Load in shaders
	eff.add_shader("shaders/main_all.vert", GL_VERTEX_SHADER);
	vector<string> frags{ "shaders/main_all.frag", "shaders/part_directional.frag",
		"shaders/part_point.frag", "shaders/part_spot.frag", "shaders/part_shadow.frag",
		"shaders/part_normal.frag" };
	eff.add_shader(frags, GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Set shadow properties
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Load in shadow shaders
	shadow_eff.add_shader("shaders/main_shadows.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("shaders/main_shadows.frag", GL_FRAGMENT_SHADER);
	shadow_eff.build();

	// Create simpler effect
	simple_eff.add_shader("shaders/main_simple.vert", GL_VERTEX_SHADER);
	frags = { "shaders/main_simple.frag", "shaders/part_directional.frag",
		"shaders/part_point.frag", "shaders/part_spot.frag", "shaders/part_shadow.frag" };
	simple_eff.add_shader(frags, GL_FRAGMENT_SHADER);
	// Build effect
	simple_eff.build();

	// Set target camera properties 
	target_c.set_position(sLight.get_position());
	target_c.set_target(vec3(0.0f, sLight.get_position().y, 0.0f));
	target_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	// Set free camera properties
	free_c.set_position(vec3(20.0f, 20.0f, 20.0f));
	free_c.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCamHelp(free_c.get_target());
	free_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	return true;
}

bool initialise() {
	// Get starting mouse position
	glfwGetCursorPos(renderer::get_window(), &mouse_x, &mouse_y);
	// Disable cursor on screen
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

//all things relating to target cam manipulation
void target_manipulation(float delta_time) {
	//position 1, infront
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1)) {
		target_c.set_position(sLight.get_position());
		target_c.set_target(vec3(0.0f, sLight.get_position().y, 0.0f));
	}
	//position 2, front-right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
		target_c.set_position(sLight.get_position() * vec3(-1.0f, 1.0f, 1.0f));
		target_c.set_target(vec3(0.0f, sLight.get_position().y, 0.0f));
	}
	//position 3, back-right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3)) {
		target_c.set_position(vec3(-10.0f, 90.0f, 30.0f));
		target_c.set_target(vec3(30.0f, 0.0f, -5.0f));
	}
	//position 4, back-left
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4)) {
		target_c.set_position(vec3(-40.0f, 50.0f, 30.0f));
		target_c.set_target(vec3(10.0f, sLight.get_position().y, 0.0f));
	}
	//zoom in on plus key
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_EQUAL)) {
		target_c.set_position(target_c.get_target() - (1 - 0.4f * delta_time) * (target_c.get_target() - target_c.get_position()));
	}
	//zoon out on minus key
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_MINUS)) {
		target_c.set_position(target_c.get_target() - (1 + 0.7f * delta_time) * (target_c.get_target() - target_c.get_position()));
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
	//up
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
		mov += vec3(0.0f, 1.0f, 0.0f);
	}
	//down
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT)) {
		mov += vec3(0.0f, -1.0f, 0.0f);
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
	/*delta_time = 0.0f;
	time_total = 0.0f;*/
	//string to mathematically aquire spheres
	string name;
	//the number of sphereRing -1
	float spheres = sphere_structure->size() - 1;

	//360 degrees. Shouldn't edit
	float full_circle = two_pi<float>();
	//sphere wave variable manipulation
	float waves_per_circle = 6.0f;	//number of sin waves in the vertical
	float wibble_speed = 0.5f;		//speed at which the spheres travel the sin arc
	float amplitude = quarter_pi<float>();			//radians around z axis (pi/2 goes to poles)
	float circling_speed = 0.4f;	//radians per second
	//ring radius transformations
	float waves_per_ring = 13.0f;	//number of sin waves in the horizontal 
	float change = 0.4f;			// percentage the radius fluctuates by
	float width_disable = 1.0f;		//0 for no radial fluctuation, 1 for regular radial fluctuation
	float shrink_factor = spheres / (12.0f + spheres);
	// Radius of the ring
	vec3 radius = vec3(5.0f, 0.0f, 0.0f);
	// Variables to be used within loop
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
		calculated_radius = sphereScale * radius * (1.0f + change * width_disable * sin((i * waves_per_ring * full_circle / spheres) + time_total * 2.0f * wibble_speed));	//calculates radius size based on sin fluctuations
		sphere->get_transform().position = rotq * calculated_radius;																					//multiplies radius by rotation to get sphere position
																																								//to change sphere scale based on distance from the origin
		distFromO = calculated_radius.x;
		sphere->get_transform().scale = vec3(shrink_factor * pi<float>() * distFromO / (spheres * sphereScale));

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
	//cout << 1.0f / delta_time << endl;
	// Cumulative total of time
	static float time_total = 0.0f;
	time_total += delta_time;
	//which camera type to use
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Z)) {
		cam_state = 0; //target cam
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_X) && cam_state != 1) {
		cam_state = 1; //free cam
		glfwGetCursorPos(renderer::get_window(), &mouse_x, &mouse_y);
		free_c.set_position(target_c.get_position());
		freeCamHelp(target_c.get_target());
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
	// Transform the spheres in the various maps
	transform_spheres(delta_time, time_total, &sphereRing);
	transform_spheres(delta_time, time_total, &sphereRing2);
	transform_spheres(delta_time, time_total, &sphereRing3);
	transform_spheres(delta_time, time_total, &sphereRing4);
	// Rotate the central sphere on the x axis
	//sphereRing["sphere0"].get_transform().orientation = (rotate(quat(), delta_time / 2.0f, vec3(1.0f, 0.0f, 0.0f)) * sphereRing["sphere0"].get_transform().orientation);

	// Set the shadow to equal the values of the spot light
	shadow.light_position = sLight.get_position();
	shadow.light_dir = sLight.get_direction();

	// To move the back wall on the x-axis
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT)) {
		ground.get_transform().translate(vec3(delta_time * -20.0f, 0.0f, 0.0f));
		cout << ground.get_transform().position.x << endl;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT)) {
		ground.get_transform().translate(vec3(delta_time * 20.0f, 0.0f, 0.0f));
		cout << ground.get_transform().position.x << endl;
	}

	return true;
}
// Aquire the transformation matrix of a child object 
void transformHierarchy(mesh *currentMesh, mat4 &M, mat3 &N) {
	//set M and N to be the matrices of the mesh in question
	M = currentMesh->get_transform().get_transform_matrix();
	N = currentMesh->get_transform().get_normal_matrix();
	mesh *cMesh = meshHierarchy[currentMesh];
	// while the acted on mesh (pointer) has a parent...
	while (cMesh != nullptr) {
		// multiply M and N by the matrices of the parent
		M = cMesh->get_transform().get_transform_matrix() * M;
		N = cMesh->get_transform().get_normal_matrix() * N;
		// change the acted on mesh (use a different pointer)
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
// Renders a mesh
void renderSimpleObject(mesh *obj, const mat4 &V, const mat4 &P, const mat4 &lV, const mat4 &lP) {
	mat4 M, MVP, lMVP;
	mat3 N;
	transformHierarchy(obj, M, N);
	MVP = P * (V * M);
	lMVP = lP * (lV * M);
	glUniformMatrix4fv(simple_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(simple_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(simple_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
	glUniformMatrix4fv(simple_eff.get_uniform_location("lMVP"), 1, GL_FALSE, value_ptr(lMVP));
	renderer::bind(*texs[obj], 0);
	glUniform1i(simple_eff.get_uniform_location("tex"), 0);
	renderer::bind(obj->get_material(), "mat");

	renderer::render(*obj);
}
// Renders a mesh
void renderObject(mesh *obj, const mat4 &V, const mat4 &P, const mat4 &lV, const mat4 &lP) {
	mat4 M, MVP, lMVP;
	mat3 N;
	transformHierarchy(obj, M, N);
	MVP = P * (V * M);
	lMVP = lP * (lV * M);
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
	glUniformMatrix4fv(eff.get_uniform_location("lMVP"), 1, GL_FALSE, value_ptr(lMVP));
	renderer::bind(*texs[obj], 0);
	glUniform1i(eff.get_uniform_location("tex"), 0);
	if (norms[obj] != nullptr) {
		renderer::bind(*norms[obj], 1);
		glUniform1i(eff.get_uniform_location("normal_map"), 1);
	}
	renderer::bind(obj->get_material(), "mat");

	renderer::render(*obj);
}
// Renders the shadow buffer for a mesh
void renderShadow(mesh *obj, const mat4 &lV, const mat4 &lP) {
	mat4 M, MVP;
	mat3 dc;
	transformHierarchy(obj, M, dc);
	MVP = lP * (lV * M);
	glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	renderer::render(*obj);
}

bool render() {
	//Declare matrices
	mat4 M, V, P, MVP;
	mat3 N;
	mat4 lV, lP, lMVP;

	// Shadows!!!------------------------------------
	renderer::set_render_target(shadow);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	renderer::bind(shadow_eff);

	lV = shadow.get_view();
	lP = perspective<float>(half_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	for (pair<const string, mesh> &item : sphereRing) {
		renderShadow(&item.second, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing2) {
		renderShadow(&item.second, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing3) {
		renderShadow(&item.second, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing4) {
		renderShadow(&item.second, lV, lP);
	}
	renderShadow(&ground, lV, lP);
	renderShadow(&column, lV, lP);

	renderer::set_render_target();
	glCullFace(GL_BACK);
	shadowMap = shadow.buffer->get_depth();
	// stop shadows!!!-------------------------------

	// Bind effect
	renderer::bind(eff);
	renderer::bind(shadowMap, 5);
	glUniform1i(eff.get_uniform_location("shadow_map"), 5);

	//bind light
	renderer::bind(pLight, "point");
	renderer::bind(dLight, "direct");
	renderer::bind(sLight, "spot");
	//Get camera information
	renderCams(V, P);
	renderObject(&column, V, P, lV, lP);
	renderObject(&hill, V, P, lV, lP);
	renderObject(&ground, V, P, lV, lP);
	//Render the sphere meshes
	for (pair<const string, mesh> &item : sphereRing) {
		renderObject(&item.second, V, P, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing2) {
		renderObject(&item.second, V, P, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing3) {
		renderObject(&item.second, V, P, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing4) {
		renderObject(&item.second, V, P, lV, lP);
	}

	renderer::bind(simple_eff);
	//bind light
	renderer::bind(pLight, "point");
	renderer::bind(dLight, "direct");
	renderer::bind(sLight, "spot");
	glUniform1i(simple_eff.get_uniform_location("shadow_map"), 5);
	//renderSimpleObject(&ground, V, P, lV, lP);

	//Render the sphere meshes
	/*for (pair<const string, mesh> &item : sphereRing) {
		renderSimpleObject(&item.second, V, P, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing2) {
		renderSimpleObject(&item.second, V, P, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing3) {
		renderSimpleObject(&item.second, V, P, lV, lP);
	}
	for (pair<const string, mesh> &item : sphereRing4) {
		renderSimpleObject(&item.second, V, P, lV, lP);
	}*/


	//Show from inside
	if (!noSky) {
		glDisable(GL_CULL_FACE);
		//	renderObject(&skyBox, V, P, lV, lP);
		glEnable(GL_CULL_FACE);
	}
	//End skybox render

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