#version 440

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord_frag;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate direction to the light
  vec3 dir_to_light = normalize(spot.position - vertex_position);
  // Calculate distance to light
  float light_distance = distance(spot.position, vertex_position);
  // Calculate attenuation value
  float attenuation = 1.0f/(spot.quadratic * pow(light_distance, 2.0f) + spot.linear * light_distance + spot.constant);
  // Calculate spot light intensity
  float intensity = pow(max(dot(-spot.direction, dir_to_light), 0.0f), spot.power);
  // Calculate light colour
  vec4 calculated_light = spot.light_colour * intensity * attenuation;
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vertex_position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k_diffuse = max(dot(transformed_normal, dir_to_light), 0.0f);
  vec4 diffuse_light = k_diffuse * (mat.diffuse_reflection * calculated_light);
  vec3 half_vector = normalize(view_dir + dir_to_light);
  float k_specular = pow(max(dot(half_vector, transformed_normal), 0.0f), mat.shininess);
  vec4 specular_light = k_specular * (mat.specular_reflection * calculated_light);
  vec4 tex_colour = texture(tex, tex_coord_frag);
  colour = (mat.emissive + diffuse_light) * tex_colour;
  colour += specular_light;
  colour.a = 1.0f;
  // *********************************
}