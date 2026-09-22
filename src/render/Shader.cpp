#include "render/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#define SHADER_LOG_SIZE 1024
namespace cartan::render {
namespace {

bool compiled(GL &gl, unsigned int shader, std::string &error) {
  int ok = 0;

  gl.glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (!ok) {
    char log[SHADER_LOG_SIZE] = {};
    gl.glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    error = log;
  }

  return ok;
}

bool linked(GL &gl, unsigned int program, std::string &error) {
  int ok = 0;

  gl.glGetProgramiv(program, GL_LINK_STATUS, &ok);

  if (!ok) {
    char log[SHADER_LOG_SIZE] = {};
    gl.glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    error = log;
  }

  return ok;
}

unsigned int compile(GL &gl, unsigned int stage, const char *source, std::string &error) {
  const unsigned int shader = gl.glCreateShader(stage);
  gl.glShaderSource(shader, 1, &source, nullptr);
  gl.glCompileShader(shader);

  if (compiled(gl, shader, error)) {
    return shader;
  }

  gl.glDeleteShader(shader);
  return 0;
}

} // namespace

bool Shader::build(GL &gl, const char *vertexSource, const char *fragmentSource,
                   std::string &error) {
  const unsigned int vertex = compile(gl, GL_VERTEX_SHADER, vertexSource, error);
  const unsigned int fragment = vertex ? compile(gl, GL_FRAGMENT_SHADER, fragmentSource, error) : 0;

  if (vertex && fragment) {
    m_program = gl.glCreateProgram();
    gl.glAttachShader(m_program, vertex);
    gl.glAttachShader(m_program, fragment);
    gl.glLinkProgram(m_program);

    if (!linked(gl, m_program, error)) {
      destroy(gl);
    }
  }

  gl.glDeleteShader(vertex);
  gl.glDeleteShader(fragment);

  return m_program != 0;
}

void Shader::destroy(GL &gl) {
  if (m_program != 0) {
    gl.glDeleteProgram(m_program);
    m_program = 0;
  }
}

void Shader::bind(GL &gl) const {
  gl.glUseProgram(m_program);
}

void Shader::setMat4(GL &gl, const char *name, const glm::mat4 &value) const {
  gl.glUniformMatrix4fv(gl.glGetUniformLocation(m_program, name), 1, GL_FALSE,
                        glm::value_ptr(value));
}

void Shader::setMat3(GL &gl, const char *name, const glm::mat3 &value) const {
  gl.glUniformMatrix3fv(gl.glGetUniformLocation(m_program, name), 1, GL_FALSE,
                        glm::value_ptr(value));
}

void Shader::setVec3(GL &gl, const char *name, const glm::vec3 &value) const {
  gl.glUniform3fv(gl.glGetUniformLocation(m_program, name), 1, glm::value_ptr(value));
}

} // namespace cartan::render