#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

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

// Point lights being used in the scene
uniform point_light points[4];
// Spot lights being used in the scene
uniform spot_light spots[5];
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

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // *********************************
  // Get distance between point light and vertex
  float light_distance = distance(point.position, position);
  // Calculate attenuation factor
  float attenuation = 1.0f/(point.quadratic * pow(light_distance, 2.0f) + point.linear * light_distance + point.constant);
  // Calculate light colour
  vec4 calculated_light = point.light_colour * attenuation;

  // Calculate light dir
  vec3 dir_to_light = normalize(point.position - position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k = max(dot(dir_to_light, transformed_normal), 0.0f);
  vec4 diffuse_light = k * (mat.diffuse_reflection * calculated_light);
  vec3 half_vec = normalize(view_dir + dir_to_light);
  k = pow(max(dot(half_vec, normal), 0.0f), mat.shininess);
  vec4 specular_light = k * (mat.specular_reflection * calculated_light);
  vec4 col = (mat.emissive + diffuse_light) * tex_colour + specular_light;
  col.a = 1.0f;
  // *********************************
  return col;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
  vec3 dir_to_light = normalize(spot.position - position);
  // Calculate distance to light
  float light_distance = distance(spot.position, position);
  // Calculate attenuation value
  float attenuation = 1.0f/(spot.quadratic * pow(light_distance, 2.0f) + spot.linear * light_distance + spot.constant);
  // Calculate spot light intensity
  float intensity = pow(max(dot(-spot.direction, dir_to_light), 0.0f), spot.power);
  // Calculate light colour
  vec4 calculated_light = spot.light_colour * attenuation;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k = max(dot(normal, dir_to_light), 0.0f);
  vec4 diffuse_light = k * (mat.diffuse_reflection * calculated_light);
  vec3 half_vector = normalize(dir_to_light + view_dir);
  k = pow(max(dot(normal, half_vector), 0.0f), mat.shininess);
  vec4 specular_light = k * (mat.specular_reflection * calculated_light);
  vec4 col = (mat.emissive + diffuse_light) * tex_colour + specular_light;
  col.a = 1.0f;
  // *********************************
  return col;
}

void main() {

  colour = vec4(0.0, 0.0, 0.0, 1.0);
  // *********************************
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vertex_position);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord_frag);
  // Sum point lights
  for (int i = 0; i < 4; ++i){
    colour += calculate_point(points[i], mat, vertex_position, transformed_normal, view_dir, tex_colour);
  }
  // Sum spot lights
  for (int i = 0; i < 5; ++i){
    colour += calculate_spot(spots[i], mat, vertex_position, transformed_normal, view_dir, tex_colour);
  }
  colour.a = 1.0f;
  // *********************************
}