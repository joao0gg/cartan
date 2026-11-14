#include "io/obj.h"

#include <cmath>
#include <fstream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

namespace cartan::io {

namespace {

constexpr std::size_t kMaxVertices = 50'000'000;
constexpr std::size_t kMaxTriangles = 100'000'000;

class Parser {
public:
  explicit Parser(MeshData &mesh) : m_mesh(mesh) {
  }

  void parseLine(const std::string &line, std::size_t number) {
    m_number = number;

    std::istringstream stream(line.substr(0, line.find('#')));
    stream.imbue(std::locale::classic());

    std::string keyword;

    if (!(stream >> keyword)) {
      return;
    }

    if (keyword == "v") {
      parseVertex(stream);
    } else if (keyword == "f") {
      parseFace(stream);
    }
  }

private:
  [[noreturn]] void fail(const std::string &message) const {
    throw std::runtime_error("line " + std::to_string(m_number) + ": " + message);
  }

  void parseVertex(std::istringstream &stream) {
    glm::vec3 position;

    if (!(stream >> position.x >> position.y >> position.z)) {
      fail("vertex needs 3 numbers");
    }

    if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z)) {
      fail("vertex is not finite");
    }

    if (m_mesh.positions.size() >= kMaxVertices) {
      fail("too many vertices");
    }

    m_mesh.positions.push_back(position);
  }

  void parseFace(std::istringstream &stream) {
    m_corners.clear();

    std::string token;

    while (stream >> token) {
      m_corners.push_back(parseIndex(token));
    }

    if (m_corners.size() < 3) {
      fail("face needs at least 3 vertices");
    }

    for (std::size_t i = 1; i + 1 < m_corners.size(); ++i) {
      if (m_mesh.triangles.size() >= kMaxTriangles) {
        fail("too many triangles");
      }

      m_mesh.triangles.push_back({m_corners[0], m_corners[i], m_corners[i + 1]});
    }
  }

  std::uint32_t parseIndex(const std::string &token) const {
    const std::string text = token.substr(0, token.find('/'));

    std::size_t used = 0;
    long long index = 0;

    try {
      index = std::stoll(text, &used);
    } catch (const std::exception &) {
      fail("bad index \"" + token + "\"");
    }

    if (used != text.size()) {
      fail("bad index \"" + token + "\"");
    }

    const auto count = static_cast<long long>(m_mesh.positions.size());
    const long long resolved = index < 0 ? count + index : index - 1;

    if (index == 0 || resolved < 0 || resolved >= count) {
      fail("index " + text + " is out of range");
    }

    return static_cast<std::uint32_t>(resolved);
  }

  MeshData &m_mesh;
  std::vector<std::uint32_t> m_corners;
  std::size_t m_number = 0;
};

} // namespace

MeshData readObj(const std::filesystem::path &path) {
  std::ifstream file(path);

  if (!file) {
    throw std::runtime_error("cannot open file");
  }

  MeshData mesh;
  Parser parser(mesh);

  std::string line;
  std::size_t number = 0;

  while (std::getline(file, line)) {
    parser.parseLine(line, ++number);
  }

  if (file.bad()) {
    throw std::runtime_error("read error");
  }

  return mesh;
}

} // namespace cartan::io
