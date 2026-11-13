#version 410 core

#define MINOR_COLOR vec3(0.0)
#define MAJOR_COLOR vec3(0.0)
#define X_AXIS_COLOR vec3(1.0, 0.0, 0.0)
#define Z_AXIS_COLOR vec3(0.0, 0.0, 1.0)

#define MINOR_ALPHA 0.1
#define MAJOR_ALPHA 0.25

uniform mat4 u_viewProjection;
uniform vec3 u_cameraPosition;
uniform float u_cellSize;
uniform float u_fadeDistance;

in vec3 v_near;
in vec3 v_far;

out vec4 o_color;

float lines(vec2 coord) {
  vec2 width = fwidth(coord);
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / width;
  float line = 1.0 - min(min(grid.x, grid.y), 1.0);
  float fade = 1.0 - smoothstep(0.1, 0.3, max(width.x, width.y));

  return line * fade;
}

void main() {
  float height = v_far.y - v_near.y;

  if (abs(height) < 1e-6) {
    discard;
  }

  float t = -v_near.y / height;

  if (t <= 0.0 || t >= 1.0) {
    discard;
  }

  vec3 point = v_near + t * (v_far - v_near);
  vec4 clip = u_viewProjection * vec4(point, 1.0);
  gl_FragDepth = clip.z / clip.w * 0.5 + 0.5;

  vec2 coord = point.xz / u_cellSize;
  vec2 width = fwidth(coord);

  float minor = lines(coord) * MINOR_ALPHA;
  float major = lines(coord / 10.0) * MAJOR_ALPHA;

  vec3 color = major > minor ? MAJOR_COLOR : MINOR_COLOR;
  float alpha = max(minor, major);

  if (abs(coord.y) < width.y) {
    color = X_AXIS_COLOR;
    alpha = 1.0;
  }

  if (abs(coord.x) < width.x) {
    color = Z_AXIS_COLOR;
    alpha = 1.0;
  }

  alpha *= 1.0 - smoothstep(0.0, u_fadeDistance, length(point - u_cameraPosition));

  if (alpha <= 0.0) {
    discard;
  }

  o_color = vec4(color, alpha);
}