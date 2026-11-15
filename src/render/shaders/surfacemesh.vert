#version 410 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_viewPosition;
out vec3 v_viewNormal;
out vec3 v_barycentric;

void main() {
  int corner = gl_VertexID % 3;
  v_barycentric = vec3(corner == 0, corner == 1, corner == 2);

  vec4 viewPosition = u_view * vec4(a_position, 1.0);
  v_viewPosition = viewPosition.xyz;
  v_viewNormal = mat3(u_view) * a_normal;

  gl_Position = u_projection * viewPosition;
}
