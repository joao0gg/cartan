#version 410 core

#define AMBIENT 0.25

uniform vec3 u_color;

in vec3 v_viewPosition;
in vec3 v_viewNormal;

out vec4 o_color;

void main() {
  vec3 normal = normalize(v_viewNormal);
  vec3 toCamera = normalize(-v_viewPosition);

  float diffuse = abs(dot(normal, toCamera));

  o_color = vec4(u_color * (AMBIENT + (1.0 - AMBIENT) * diffuse), 1.0);
}
