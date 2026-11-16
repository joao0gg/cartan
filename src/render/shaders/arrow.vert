#version 410 core

layout(location = 0) in vec2 a_offset;
layout(location = 1) in float a_z;
layout(location = 2) in float a_part;
layout(location = 3) in vec3 a_normal;
layout(location = 4) in vec3 a_base;
layout(location = 5) in vec3 a_vector;

#define HEAD_LENGTH 4.0
#define MAX_RADIUS_RATIO 0.1

uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_scale;
uniform float u_radius;

out vec3 v_viewPosition;
out vec3 v_viewNormal;

void main() {
  vec3 vector = a_vector * u_scale;
  float length = length(vector);
  vec3 direction = vector / length;

  vec3 helper = abs(direction.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
  vec3 side = normalize(cross(helper, direction));
  vec3 up = cross(direction, side);

  float radius = min(u_radius, length * MAX_RADIUS_RATIO);
  float head = HEAD_LENGTH * radius;
  float shaft = length - head;

  float z = a_part < 0.5 ? a_z * shaft : shaft + a_z * head;
  vec3 local = vec3(a_offset * radius, z);

  vec3 position = a_base + side * local.x + up * local.y + direction * local.z;
  vec3 normal = side * a_normal.x + up * a_normal.y + direction * a_normal.z;

  vec4 viewPosition = u_view * vec4(position, 1.0);
  v_viewPosition = viewPosition.xyz;
  v_viewNormal = mat3(u_view) * normal;

  gl_Position = u_projection * viewPosition;
}
