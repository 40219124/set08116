#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Meshes
float sphereScale = 3.0f;
map<string, mesh> sphereRing;
mesh skyBox;
mesh ground;
mesh hill;
mesh terra;
mesh column;
mesh mirror;
geometry polaroid;
// Reference maps
map<mesh*, texture*> texs;
map<mesh*, texture*> norms;
map<mesh*, mesh*> meshHierarchy;
// Lights
directional_light dLight;
point_light pLight;
spot_light sLight;
// Buffers
frame_buffer snap;
frame_buffer shady;
frame_buffer reflection;
// Texture
texture tex;
texture shadowMap;
cubemap setting;
// Effects
effect eff;
effect shadow_eff;
effect sky_eff;
effect screen_eff;
effect mirror_eff;
effect blur_eff;
effect final_eff;
// Cameras
target_camera target_c;
free_camera free_c;
target_camera shadow_cam;
target_camera anti_cam;
// camera boolean
uint cam_state = 0;
// Mouse positions
double mouse_x;
double mouse_y;
float blurr = 0.002;
// post processing toggles
int eff_state = 0;
int neg_state = -1;
int guides = -1;

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
// Move the anti-cam
void antiCamUpdate() {
	vec3 cam_pos, cam_target;
	// Get camera details based on active camera
	if (cam_state == 1) {
		cam_pos = free_c.get_position();
		cam_target = normalize(-(free_c.get_target() - cam_pos));
	}
	else {
		cam_pos = target_c.get_position();
		cam_target = normalize(-(target_c.get_target() - cam_pos));
	}
	// Get mirror details
	const static vec3 mirror_pos = mirror.get_transform().position;
	const static vec3 mirror_norm = vec3(0.0f, 0.0f, 1.0f);
	// The camera relative to the mirror
	vec3 relative_pos = cam_pos - mirror_pos;
	// Dot product of mirror's normal and mirror->camera
	float dotted_pos = dot(relative_pos, mirror_norm);
	if (dotted_pos > 0) {
		// Set anti camera position to have z coordinate flipped
		vec3 new_pos = mirror_pos + vec3(relative_pos.x, relative_pos.y, -relative_pos.z);
		anti_cam.set_position(new_pos);
		// Set anti camera view direction to be flipped on z axis
		vec3 new_target = -cam_target;
		new_target.z *= -1;
		anti_cam.set_target(new_pos + new_target);
	}
}
//make a map of spheres
void makeSphereStructure(map<string, mesh> *sphereStructure, float sphereCount) {
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
		(*sphereStructure)[name] = technosphere;
		sphere = &(*sphereStructure)[name];
		// sets material properties
		sphere->get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		sphere->get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		sphere->get_material().set_shininess(25.0f);
		sphere->get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// rotates the sphere on the y-axis, and moves to circumference of ring
		sphere->get_transform().rotate(eulerAngleY(2.0f * i * pi<float>() / sphereCount));
		sphere->get_transform().translate(vec3((*sphereStructure)[name].get_transform().get_transform_matrix() * vec4(10.0f, 0.0f, 0.0f, 1.0f)));
		// maps a texture to the sphere's name
		texs[sphere] = &sphere_tex;
		norms[sphere] = &sphere_norm;
		// sets the sphere's parent object
		//(*sphereHierarchy)[name] = "sphere0";
		meshHierarchy[sphere] = &(*sphereStructure)["sphere0"];
	}
}
// Create terrain from a texture
void generate_terrain(geometry &geom, const texture &height_map, int width, int depth, float height_scale) {
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec3> tangents;
	vector<vec3> binormals;
	vector<vec2> tex_coords;
	vector<unsigned int> indices;

	// Put height map data in an array
	glBindTexture(GL_TEXTURE_2D, height_map.get_id());
	auto data = new vec4[height_map.get_width() * height_map.get_height()];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (void *)data);

	// Calculate object to texture ratio
	float width_ratio = static_cast<float>(width) / static_cast<float>(height_map.get_width());
	float depth_ratio = static_cast<float>(depth) / static_cast<float>(height_map.get_height());

	vec3 point;

	// Positions
	for (int x = 0; x < height_map.get_width(); ++x) {
		point.x = -(width / 2.0f) + (width_ratio * static_cast<float>(x));

		for (int z = 0; z < height_map.get_height(); ++z) {
			point.z = -(depth / 2.0f) + (depth_ratio * static_cast<float>(z));
			point.y = data[(z * height_map.get_width()) + x].y * height_scale;
			positions.push_back(point);
		}
	}

	// Indices
	for (unsigned int x = 0; x < height_map.get_width() - 1; ++x) {
		for (unsigned int y = 0; y < height_map.get_height() - 1; ++y) {
			unsigned int tl = (y * height_map.get_width()) + x;
			unsigned int tr = (y * height_map.get_width()) + x + 1;
			unsigned int bl = ((y + 1) * height_map.get_width()) + x;
			unsigned int br = ((y + 1) * height_map.get_width()) + x + 1;

			indices.push_back(tl);
			indices.push_back(br);
			indices.push_back(bl);

			indices.push_back(tl);
			indices.push_back(tr);
			indices.push_back(br);
		}
	}

	// Resize NBT vectors
	normals.resize(positions.size());
	binormals.resize(positions.size());
	tangents.resize(positions.size());

	// Normals
	for (unsigned int i = 0; i < indices.size() / 3; ++i) {
		auto p1 = indices[i * 3];
		auto p2 = indices[i * 3 + 1];
		auto p3 = indices[i * 3 + 2];

		vec3 side1 = positions[p1] - positions[p3];
		vec3 side2 = positions[p1] - positions[p2];

		vec3 norm = normalize(cross(side2, side1));

		normals[p1] += norm;
		normals[p2] += norm;
		normals[p3] += norm;
	}

	// BT's and normalising normals
	for (auto &n : normals) {
		static int i = 0;
		n = normalize(n);
		binormals[i] = normalize(cross(vec3(0.0f, 0.0f, 1.0f), n));
		tangents[i] = normalize(cross(binormals[i], n));
		++i;
	}

	// Tex_coords
	for (unsigned int x = 0; x < height_map.get_width(); ++x) {
		for (unsigned int z = 0; z < height_map.get_height(); ++z) {
			tex_coords.push_back(vec2(width_ratio * x, depth_ratio * z));
		}
	}

	// Buffers
	geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom.add_buffer(normals, BUFFER_INDEXES::NORMAL_BUFFER);
	geom.add_buffer(tangents, BUFFER_INDEXES::TANGENT_BUFFER);
	geom.add_buffer(binormals, BUFFER_INDEXES::BINORMAL_BUFFER);
	geom.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom.add_index_buffer(indices);

	// Memory clean up
	delete[] data;
}

bool load_content() {

	// Blur effect
	/*blur_eff.add_shader("shaders/main_blur.vert", GL_VERTEX_SHADER);
	blur_eff.add_shader("shaders/main_blur.frag", GL_FRAGMENT_SHADER);
	blur_eff.build();*/

	// Final effect
	final_eff.add_shader("shaders/end.vert", GL_VERTEX_SHADER);
	final_eff.add_shader("shaders/end.frag", GL_FRAGMENT_SHADER);
	final_eff.build();

	// Mirror effect
	mirror_eff.add_shader("shaders/main_mirror.vert", GL_VERTEX_SHADER);
	mirror_eff.add_shader("shaders/main_mirror.frag", GL_FRAGMENT_SHADER);
	mirror_eff.build();

	// Load in shaders
	eff.add_shader("shaders/main_all.vert", GL_VERTEX_SHADER);
	vector<string> frags{ "shaders/main_all.frag", "shaders/part_directional.frag",
		"shaders/part_point.frag", "shaders/part_spot.frag", "shaders/part_shadows.frag",
		"shaders/part_normal.frag" };
	eff.add_shader(frags, GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Set shadow properties
	snap = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	shady = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	reflection = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

	// Load in shadow shaders
	shadow_eff.add_shader("shaders/main_shadows.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("shaders/main_shadows.frag", GL_FRAGMENT_SHADER);
	shadow_eff.build();

	// create skybox effect
	sky_eff.add_shader("shaders/main_skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("shaders/main_skybox.frag", GL_FRAGMENT_SHADER);
	sky_eff.build();

	// Create screen effect
	screen_eff.add_shader("shaders/main_screenspace.vert", GL_VERTEX_SHADER);
	screen_eff.add_shader("shaders/main_screenspace.frag", GL_FRAGMENT_SHADER);
	screen_eff.build();

	// Loads in a texture   
	static texture column_tex = texture("textures/Marble_Base_Color.jpg");
	static texture column_norm = texture("textures/Marble_Normal_OpenGL.jpg");

	// Make sky box mesh and give details
	skyBox = mesh(geometry_builder().create_box(vec3(200.0f)));
	array<string, 6> box_files = { "textures/posx.jpg",  "textures/negx.jpg",  "textures/posy.jpg",
		"textures/negy.jpg",  "textures/posz.jpg",  "textures/negz.jpg" };
	setting = cubemap(box_files);

	// Load grass textures
	static texture grass_tex = texture("textures/grass01.jpg");
	static texture grass_norm = texture("textures/grass01_n.jpg");

	ground = mesh(geometry_builder().create_plane());
	ground.get_transform().scale = vec3(30.0f);
	texs[&ground] = &grass_tex;
	norms[&ground] = &grass_norm;

	// Make the hill from a height map
	geometry terr;
	texture contours = texture("textures/my_hill_2.png");
	generate_terrain(terr, contours, 20, 20, 2.0f);
	terra = mesh(terr);
	// Map stuff
	texs[&terra] = &grass_tex;
	norms[&terra] = &grass_norm;
	meshHierarchy[&terra] = nullptr;
	// Transform terrain
	float hill_scale = 90.0f;
	terra.get_transform().scale = vec3(hill_scale, 30.0f, hill_scale);
	terra.get_transform().translate(vec3(0.0f, -49.0f, 0.0f));

	// Make a "mirror"
	mirror = mesh(geometry_builder::create_plane(1, 1));
	mirror.get_transform().scale = vec3(30.0f, 1.0f, 50.0f);
	mirror.get_transform().rotate(eulerAngleX(half_pi<float>()));
	mirror.get_transform().rotate(eulerAngleY(pi<float>()));
	mirror.get_transform().translate(vec3(0.0f, 30.0f, -50.0f));
	static texture free_cam_ui = texture("textures/free_cam_ui.png");
	texs[&mirror] = &free_cam_ui;
	meshHierarchy[&mirror] = nullptr;

	// Make the quad for rendering to screen region
	vector<vec3> quad_pos = { vec3(-1.0f, -1.0f, 0.0f),  vec3(1.0f, -1.0f, 0.0f),  vec3(-1.0f, 1.0f, 0.0f),  vec3(1.0f, 1.0f, 0.0f) };
	vector<vec2> quad_texco = { vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
	polaroid.set_type(GL_TRIANGLE_STRIP);
	polaroid.add_buffer(quad_pos, BUFFER_INDEXES::POSITION_BUFFER);
	polaroid.add_buffer(quad_texco, BUFFER_INDEXES::TEXTURE_COORDS_0);

	// Import the column
	column = mesh(geometry("models/Column_HP.obj"));
	texs[&column] = &column_tex;
	norms[&column] = &column_norm;
	meshHierarchy[&column] = nullptr;
	column.get_transform().position = vec3(0.0f, 0.0f, 0.0f);
	column.get_transform().scale = vec3(20.0f);

	// Make various sphere structures
	makeSphereStructure(&sphereRing, 20.0f);

	// Set information for the first sphere ring
	sphereRing["sphere0"].get_transform().translate(vec3(0.0f, 1.7f, 0.0f));
	sphereRing["sphere0"].get_transform().scale = vec3(1.0f / (20.0f * sphereScale));
	meshHierarchy[&sphereRing["sphere0"]] = &column;

	// Set directional light properties
	bool dLightOn = true;
	dLight.set_ambient_intensity(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	dLight.set_direction(normalize(vec3(1.0f, 1.0f, 1.0f)));
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
	// Shadow casting light properties
	vec3 shadow_light_pos = vec3(90.0f, 90.0f, 90.0f) / 2.0f;
	vec3 shadow_light_dir = normalize(vec3(-1.0f));
	// Set spot light properties
	sLight.set_direction(shadow_light_dir);
	sLight.set_position(shadow_light_pos);
	sLight.set_light_colour(vec4(0.8f, 0.7f, 0.1f, 1.0f));
	sLight.set_range(160.0f);
	sLight.set_power(5.0f);

	// Set target camera properties 
	target_c.set_position(vec3(-80.0f, 30.0f, 0.0f));
	target_c.set_target(vec3(0.0f, 30.0f, 0.0f));
	target_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	// Set free camera properties
	free_c.set_position(vec3(20.0f, 20.0f, 20.0f));
	free_c.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCamHelp(free_c.get_target());
	free_c.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	shadow_cam.set_position(shadow_light_pos);
	shadow_cam.set_target(shadow_light_pos + shadow_light_dir);
	shadow_cam.set_projection(half_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	anti_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

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
		target_c.set_position(vec3(-80.0f, 30.0f, 0.0f));
		target_c.set_target(vec3(0.0f, 30.0f, 0.0f));
	}
	//position 2, front-right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
		target_c.set_position(vec3(80.0f, 30.0f, 0.0f));
		target_c.set_target(vec3(0.0f, 30.0f, 0.0f));
	}
	//position 3, back-right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3)) {
		target_c.set_position(vec3(-10.0f, 90.0f, 30.0f));
		target_c.set_target(vec3(30.0f, 0.0f, -5.0f));
	}
	//position 4, back-left
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4)) {
		target_c.set_position(vec3(-40.0f, 50.0f, 30.0f));
		target_c.set_target(vec3(10.0f, 30.0f, 0.0f));
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
	float speed = 25.0f * delta_time;
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
	//string to mathematically aquire spheres
	string name;
	//the number of sphereRing -1
	float spheres = sphere_structure->size() - 1;

	//360 degrees. Shouldn't edit
	float full_circle = two_pi<float>();
	//sphere wave variable manipulation
	float waves_per_circle = 2.0f;	//number of sin waves in the vertical
	float wibble_speed = 1.0f;		//speed at which the spheres travel the sin arc
	float amplitude = pi<float>() / 8.0;			//radians around z axis (pi/2 goes to poles)
	float circling_speed = 0.9f;	//radians per second
	//ring radius transformations
	float waves_per_ring = 3.0f;	//number of sin waves in the horizontal 
	float change = 0.5f;			// percentage the radius fluctuates by
	float width_disable = 1.0f;		//0 for no radial fluctuation, 1 for regular radial fluctuation
	float shrink_factor = spheres / (20.0f + spheres);
	// Radius of the ring
	vec3 radius = vec3(9.0f, 0.0f, 0.0f);
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
	}
	//rotate centre (parent) sphere
	(*sphere_structure)["sphere0"].get_transform().rotate(eulerAngleY(delta_time * circling_speed));
}
//move the skybox
void sky_follow() {
	// Sets the skybox position to the active camera
	switch (cam_state)
	{
	case(0):
		skyBox.get_transform().position = target_c.get_position();
		break;
	case(1):
		skyBox.get_transform().position = free_c.get_position();
		break;
	default:
		break;
	}
}

bool update(float delta_time) {
	//print fps
	cout << 1.0f / delta_time << endl;
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
		cam_state = 2; //default to target cam
	}
	// Manipulates the active camera
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
	// Transform the spheres
	transform_spheres(delta_time, time_total, &sphereRing);

	// To move the back wall on the x-axis
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
		blurr *= 1.0f + 0.4f * delta_time;
		cout << blurr << endl;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
		blurr *= 1.0f - 0.4f * delta_time;
		cout << blurr << endl;
	}

	// To toggle post processing
	static bool key_i = false;
	static bool key_g = false;
	static bool key_b = false;
	// on key i toggle negative
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_I) == GLFW_PRESS && !key_i) {
		neg_state *= -1;
		key_i = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_I) == GLFW_RELEASE && key_i) {
		key_i = false;
	}
	// on key g toggle guides
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_G) == GLFW_PRESS && !key_g) {
		guides *= -1;
		key_g = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_G) == GLFW_RELEASE && key_g) {
		key_g = false;
	}
	// on key b toggle effect
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_B) == GLFW_PRESS && !key_b) {
		++eff_state;
		if (eff_state > 3) {
			eff_state = 0;
		}
		key_b = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_B) == GLFW_RELEASE && key_b) {
		key_b = false;
	}

	// Updating of cameras
	antiCamUpdate();
	anti_cam.update(delta_time);
	shadow_cam.update(delta_time);
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
void renderCams(mat4 &V, mat4 &P, vec3 &cam_pos) {
	switch (cam_state) {
	case(0):
		V = target_c.get_view();
		P = target_c.get_projection();
		cam_pos = target_c.get_position();
		break;
	case(1):
		V = free_c.get_view();
		P = free_c.get_projection();
		cam_pos = free_c.get_position();
		break;
	default:
		V = target_c.get_view();
		P = target_c.get_projection();
		cam_pos = target_c.get_position();
		break;
	}
}
// Renders a mesh
void renderObject(mesh *obj, const mat4 &VP, const mat4 &lVP) {
	static texture* prev_tex;
	static material prev_mat;
	mat4 M, MVP, lMVP;
	mat3 N;
	transformHierarchy(obj, M, N);
	MVP = VP * M;
	lMVP = lVP * M;
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
	glUniformMatrix4fv(eff.get_uniform_location("lMVP"), 1, GL_FALSE, value_ptr(lMVP));
	// If the texture is the same as the last used, do not rebind
	if (prev_tex != texs[obj]) {
		prev_tex = texs[obj];
		renderer::bind(*prev_tex, 0);
		glUniform1i(eff.get_uniform_location("tex"), 0);
	}
	// If the normal is the same as the last used, do not rebind
	if (norms[obj] != nullptr) {
		renderer::bind(*norms[obj], 1);
		glUniform1i(eff.get_uniform_location("normal_map"), 1);
	}
	renderer::bind(obj->get_material(), "mat");

	renderer::render(*obj);
}
// Renders the shadow buffer for a mesh
void renderShadow(mesh *obj, const mat4 &lVP) {
	mat4 M, MVP;
	mat3 dc;
	transformHierarchy(obj, M, dc);
	MVP = lVP * M;
	glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	renderer::render(*obj);
}
// Renders the shadow buffer for a mesh
void renderShady(mesh *obj, const mat4 &lVP) {
	mat4 M, MVP;
	mat3 dc;
	transformHierarchy(obj, M, dc);
	MVP = lVP * M;
	glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(shadow_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniform3fv(shadow_eff.get_uniform_location("eye_pos"), 1, value_ptr(shadow_cam.get_position()));
	renderer::render(*obj);
}

bool render() {
	//Declare matrices
	mat4 M, V, P, VP, MVP;
	mat3 N;
	mat4 lV, lP, lVP, lMVP;
	vec3 cam_pos;
	renderCams(V, P, cam_pos);
	// Begin shadow rendering
	renderer::set_render_target(shady);
	renderer::clear();
	renderer::bind(shadow_eff);
	// Get the view and projection matrix for the shadow
	lV = shadow_cam.get_view();
	lP = shadow_cam.get_projection();
	lVP = lP * lV;
	// Render geometry
	for (pair<const string, mesh> &item : sphereRing) {
		renderShady(&item.second, lVP);
	}
	renderShady(&column, lVP);
	renderShady(&terra, lVP);
	renderShady(&mirror, lVP);
	// Save frame to texture
	shadowMap = shady.get_frame();
	// code xl break point ------------------------------------------------------------------
	glClearColor(0.0, 1.0, 1.0, 1.0);
	// Render the reflection view only if camera is infront of the mirror
	if (cam_pos.z > mirror.get_transform().position.z) {
		// Set render target
		renderer::set_render_target(reflection);
		// Clear buffer
		renderer::clear();
		// Set anti cam view, projection, position
		mat4 aV = anti_cam.get_view();
		mat4 aP = anti_cam.get_projection();
		vec3 acam_pos = anti_cam.get_position();
		mat4 aVP = aP * aV;
		// Render the sky box
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		renderer::bind(sky_eff);
		skyBox.get_transform().position = acam_pos;
		M = skyBox.get_transform().get_transform_matrix();
		MVP = aVP * M;
		glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		renderer::bind(setting, 0);
		glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);
		renderer::render(skyBox);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		// Bind effect
		renderer::bind(eff);
		// Bind shadow details
		renderer::bind(shadowMap, 5);
		glUniform1i(eff.get_uniform_location("shadow_map"), 5);
		// Bind lights
		renderer::bind(pLight, "point");
		renderer::bind(dLight, "direct");
		renderer::bind(sLight, "spot");
		// Bind camera information
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(acam_pos));
		// Render objects
		renderObject(&column, aVP, lVP);
		renderObject(&terra, aVP, lVP);
		for (pair<const string, mesh> &item : sphereRing) {
			renderObject(&item.second, aVP, lVP);
		}
		// Send frame contents to a texture
		static texture reflection_tex = reflection.get_frame();
		texs[&mirror] = &reflection_tex;
	}

	// begin rendering scene for first proper time
	renderer::set_render_target(snap);
	renderer::clear();
	// Calculate VP
	VP = P * V;
	// Render the sky box
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	renderer::bind(sky_eff);
	sky_follow();
	M = skyBox.get_transform().get_transform_matrix();
	MVP = VP * M;
	glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	renderer::bind(setting, 0);
	glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);
	renderer::render(skyBox);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	// Bind effect
	renderer::bind(eff);
	// Bind shadow details
	renderer::bind(shadowMap, 5);
	glUniform1i(eff.get_uniform_location("shadow_map"), 5);
	// Bind lights
	renderer::bind(pLight, "point");
	renderer::bind(dLight, "direct");
	renderer::bind(sLight, "spot");
	// Bind camera information
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam_pos));
	// Render objects
	renderObject(&column, VP, lVP);
	renderObject(&terra, VP, lVP);
	for (pair<const string, mesh> &item : sphereRing) {
		renderObject(&item.second, VP, lVP);
	}
	// Render mirror
	renderer::bind(mirror_eff);
	M = mirror.get_transform().get_transform_matrix();
	MVP = VP * M;
	glUniformMatrix4fv(mirror_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	renderer::bind(*(texs[&mirror]), 0);
	glUniform1i(mirror_eff.get_uniform_location("tex"), 0);
	renderer::render(mirror);
	// Set texture to equal rendered frame
	texture processing = texture();
	processing = snap.get_frame();
	// Render scene with post processing
	if (eff_state > 0 || neg_state > 0) {
		static frame_buffer proc_buff = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
		renderer::set_render_target(proc_buff);
		renderer::clear();
		renderer::bind(screen_eff);
		MVP = mat4(1.0f);
		glUniformMatrix4fv(screen_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		renderer::bind(processing, 0);
		glUniform1i(screen_eff.get_uniform_location("tex"), 0);
		glUniform1f(screen_eff.get_uniform_location("value"), blurr);
		glUniform1i(screen_eff.get_uniform_location("eff_state"), eff_state);
		glUniform1i(screen_eff.get_uniform_location("neg_state"), neg_state);
		glUniform1i(screen_eff.get_uniform_location("guides"), guides);
		renderer::render(polaroid);
		// Set texture to equal rendered frame
		processing = proc_buff.get_frame();
	}
	// Render scene with blur
	
	// Render final output to processing to screen
	renderer::set_render_target();
	renderer::clear();
	renderer::bind(final_eff);
	renderer::bind(processing, 0);
	glUniform1i(final_eff.get_uniform_location("tex"), 0);
	glUniformMatrix4fv(final_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(mat4(1.0f)));
	renderer::render(polaroid);

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
	cout << renderer::get_screen_width() << ", " << renderer::get_screen_height() << endl;
	// Run application
	application.run();
}