#version 410 core

uniform mat4 u_inverseViewProjection;

out vec3 v_near;
out vec3 v_far;

vec3 unproject(vec2 point, float depth) {
  vec4 world = u_inverseViewProjection * vec4(point, depth, 1.0);
  return world.xyz / world.w;
}

void main() {
  vec2 point = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;

  v_near = unproject(point, -1.0);
  v_far = unproject(point, 1.0);

  gl_Position = vec4(point, 0.0, 1.0);
}
