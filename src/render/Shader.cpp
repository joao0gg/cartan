#include "render/Shader.h"

#include <utility>

#include <glm/gtc/type_ptr.hpp>

namespace cartan::render {

namespace {

constexpr int kShaderLogSize = 1024;

bool compiled(GL &gl, unsigned int shader, std::string &error) {
  int ok = 0;

  gl.glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (!ok) {
    char log[kShaderLogSize] = {};
    gl.glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    error = log;
  }

  return ok;
}

bool linked(GL &gl, unsigned int program, std::string &error) {
  int ok = 0;

  gl.glGetProgramiv(program, GL_LINK_STATUS, &ok);

  if (!ok) {
    char log[kShaderLogSize] = {};
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

std::optional<Shader> Shader::build(GL &gl, const char *vertexSource, const char *fragmentSource,
                                    std::string &error) {
  const unsigned int vertex = compile(gl, GL_VERTEX_SHADER, vertexSource, error);
  const unsigned int fragment = vertex ? compile(gl, GL_FRAGMENT_SHADER, fragmentSource, error) : 0;

  std::optional<Shader> shader;

  if (vertex && fragment) {
    const unsigned int program = gl.glCreateProgram();
    gl.glAttachShader(program, vertex);
    gl.glAttachShader(program, fragment);
    gl.glLinkProgram(program);

    if (linked(gl, program, error)) {
      shader.emplace(Shader(gl, program));
    }
    else {
      gl.glDeleteProgram(program);
    }
  }

  gl.glDeleteShader(vertex);
  gl.glDeleteShader(fragment);

  return shader;
}

Shader::Shader(GL &gl, unsigned int program) : m_gl(&gl), m_program(program) {
}

Shader::~Shader() {
  release();
}

Shader::Shader(Shader &&other) noexcept
    : m_gl(std::exchange(other.m_gl, nullptr)), m_program(std::exchange(other.m_program, 0)) {
}

Shader &Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    release();

    m_gl = std::exchange(other.m_gl, nullptr);
    m_program = std::exchange(other.m_program, 0);
  }

  return *this;
}

void Shader::release() {
  if (m_gl != nullptr && m_program != 0) {
    m_gl->glDeleteProgram(m_program);
  }

  m_program = 0;
}

void Shader::bind() const {
  m_gl->glUseProgram(m_program);
}

void Shader::setMat4(const char *name, const glm::mat4 &value) const {
  m_gl->glUniformMatrix4fv(m_gl->glGetUniformLocation(m_program, name), 1, GL_FALSE,
                           glm::value_ptr(value));
}

void Shader::setMat3(const char *name, const glm::mat3 &value) const {
  m_gl->glUniformMatrix3fv(m_gl->glGetUniformLocation(m_program, name), 1, GL_FALSE,
                           glm::value_ptr(value));
}

void Shader::setVec3(const char *name, const glm::vec3 &value) const {
  m_gl->glUniform3fv(m_gl->glGetUniformLocation(m_program, name), 1, glm::value_ptr(value));
}

} // namespace cartan::render
