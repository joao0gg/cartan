#include "render/gl/program.h"

#include <stdexcept>
#include <string>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace cartan::render::gl {

namespace {

GLuint compile(GLenum type, std::string_view source) {
  const GLuint shader = glCreateShader(type);
  const char *data = source.data();
  const auto size = static_cast<GLint>(source.size());

  glShaderSource(shader, 1, &data, &size);
  glCompileShader(shader);

  GLint ok = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (ok == GL_FALSE) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    std::string log(length, '\0');
    glGetShaderInfoLog(shader, length, nullptr, log.data());
    glDeleteShader(shader);

    throw std::runtime_error("Shader compilation failed: " + log);
  }

  return shader;
}

} // namespace

Program::Program(std::string_view vertex, std::string_view fragment) {
  const GLuint vertexShader = compile(GL_VERTEX_SHADER, vertex);
  const GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, fragment);

  m_id = glCreateProgram();
  glAttachShader(m_id, vertexShader);
  glAttachShader(m_id, fragmentShader);
  glLinkProgram(m_id);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLint ok = GL_FALSE;
  glGetProgramiv(m_id, GL_LINK_STATUS, &ok);

  if (ok == GL_FALSE) {
    GLint length = 0;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);

    std::string log(length, '\0');
    glGetProgramInfoLog(m_id, length, nullptr, log.data());
    glDeleteProgram(m_id);

    throw std::runtime_error("Program linking failed: " + log);
  }
}

Program::~Program() {
  glDeleteProgram(m_id);
}

void Program::use() const {
  glUseProgram(m_id);
}

void Program::set(const char *name, int value) const {
  glUniform1i(glGetUniformLocation(m_id, name), value);
}

void Program::set(const char *name, float value) const {
  glUniform1f(glGetUniformLocation(m_id, name), value);
}

void Program::set(const char *name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value));
}

void Program::set(const char *name, const glm::mat4 &value) const {
  glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value));
}

} // namespace cartan::render::gl
