#include "io/ShaderIO.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace cartan::io {

std::string readTextFile(const std::filesystem::path &path) {
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("cannot open " + path.string());
  }

  std::ostringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

} // namespace cartan::io