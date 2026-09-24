#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vNormal;
out vec3 vViewPosition;

void main() {
  vec4 viewPosition = uModelView * vec4(aPosition, 1.0);

  vNormal = uNormalMatrix * aNormal;
  vViewPosition = viewPosition.xyz;

  gl_Position = uProjection * viewPosition;
}
