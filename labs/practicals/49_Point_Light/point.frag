#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light point;
// Material for the object
uniform material mat;
// Eye position
uniform vec3 eye_pos;
// Texture
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
  // Get distance between point light and vertex
  float light_distance = distance(point.position, vertex_position);
  // Calculate attenuation factor
  float attenuation = 1.0f/(point.constant + point.linear * light_distance + point.quadratic * pow(light_distance, 2.0f));
  // Calculate light colour
  vec4 attenuated_light = attenuation * point.light_colour;

  // Calculate light dir
  vec3 light_dir = normalize(point.position - vertex_position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k_diffuse = max(dot(transformed_normal, light_dir), 0.0f);
  vec4 diffuse_light = k_diffuse * (mat.diffuse_reflection * attenuated_light);
  vec3 view_dir = normalize(eye_pos - vertex_position);
  vec3 half_vector = normalize(view_dir + light_dir);
  float k_specular = pow(max(dot(transformed_normal, half_vector), 0.0f), mat.shininess);
  vec4 specular_light = k_specular * (mat.specular_reflection * attenuated_light);
  vec4 tex_colour = texture(tex, tex_coord_frag);
  colour = (mat.emissive + diffuse_light) * tex_colour;
  colour += specular_light;
  colour.a = 1.0f;
  // *********************************
}