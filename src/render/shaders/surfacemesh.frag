#version 410 core

#define AMBIENT 0.25
#define BACKFACE_FACTOR 0.6

uniform vec3 u_color;
uniform int u_smooth;
uniform int u_edges;
uniform float u_edgeWidth;
uniform vec3 u_edgeColor;

in vec3 v_viewPosition;
in vec3 v_viewNormal;
in vec3 v_barycentric;

out vec4 o_color;

void main() {
  vec3 normal = u_smooth != 0 ? normalize(v_viewNormal)
                              : normalize(cross(dFdx(v_viewPosition), dFdy(v_viewPosition)));
  vec3 toCamera = normalize(-v_viewPosition);

  float diffuse = abs(dot(normal, toCamera));
  vec3 color = u_color * (AMBIENT + (1.0 - AMBIENT) * diffuse);

  if (!gl_FrontFacing) {
    color *= BACKFACE_FACTOR;
  }

  if (u_edges != 0) {
    vec3 width = fwidth(v_barycentric) * u_edgeWidth;
    vec3 edge = smoothstep(vec3(0.0), width, v_barycentric);
    color = mix(u_edgeColor, color, min(min(edge.x, edge.y), edge.z));
  }

  o_color = vec4(color, 1.0);
}
