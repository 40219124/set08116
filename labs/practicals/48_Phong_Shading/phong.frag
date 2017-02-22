#version 440

// A directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord_out;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {

  // *********************************
  // Calculate ambient component
  vec4 ambient_light = mat.diffuse_reflection * light.ambient_intensity;
  // Calculate diffuse component
  float k_diffuse = max(dot(transformed_normal, light.light_dir), 0.0f);
  vec4 diffuse_light = k_diffuse * (mat.diffuse_reflection * light.light_colour);
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vertex_position);
  // Calculate half vector
  vec3 half_vector = normalize(view_dir + light.light_dir);
  // Calculate specular component
  float k_specular = pow(max(dot(transformed_normal, half_vector), 0.0f), mat.shininess);
  vec4 specular_light = k_specular * (mat.specular_reflection * light.light_colour);
  // Sample texture
  vec4 texture_colour = texture(tex, tex_coord_out);
  // Calculate primary colour component
  colour = (mat.emissive + ambient_light + diffuse_light) * texture_colour;
  // Calculate final colour - remember alpha
  colour += specular_light;
  colour.a = 1.0f;
  // *********************************
}