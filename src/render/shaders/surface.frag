// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#version 330 core

in vec3 vNormal;
in vec3 vViewPosition;

uniform vec3 uColor;

out vec4 fragColor;

void main() {
  vec3 normal = normalize(vNormal);

  if (!gl_FrontFacing) {
    normal = -normal;
  }

  vec3 toEye = normalize(-vViewPosition);
  float diffuse = max(dot(normal, toEye), 0.0);
  float specular = pow(diffuse, 32.0) * 0.2;

  fragColor = vec4(uColor * (0.25 + 0.75 * diffuse) + vec3(specular), 1.0);
}
