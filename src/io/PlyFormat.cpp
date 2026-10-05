// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "io/SurfaceIO.h"
#include "io/detail/Text.h"

namespace cartan::io {

namespace {

enum class ScalarType { Int8, UInt8, Int16, UInt16, Int32, UInt32, Float32, Float64 };

std::optional<ScalarType> scalarType(std::string_view name) {
  if (name == "char" || name == "int8") {
    return ScalarType::Int8;
  }
  if (name == "uchar" || name == "uint8") {
    return ScalarType::UInt8;
  }
  if (name == "short" || name == "int16") {
    return ScalarType::Int16;
  }
  if (name == "ushort" || name == "uint16") {
    return ScalarType::UInt16;
  }
  if (name == "int" || name == "int32") {
    return ScalarType::Int32;
  }
  if (name == "uint" || name == "uint32") {
    return ScalarType::UInt32;
  }
  if (name == "float" || name == "float32") {
    return ScalarType::Float32;
  }
  if (name == "double" || name == "float64") {
    return ScalarType::Float64;
  }

  return std::nullopt;
}

bool isInteger(ScalarType type) {
  return type != ScalarType::Float32 && type != ScalarType::Float64;
}

struct Property {
  std::string name;
  ScalarType type = ScalarType::Float32;
  bool isList = false;
  ScalarType countType = ScalarType::UInt8;
};

struct Element {
  std::string name;
  std::uint64_t count = 0;
  std::vector<Property> properties;
};

enum class Encoding { Ascii, LittleEndian, BigEndian };

struct Header {
  Encoding encoding = Encoding::Ascii;
  std::vector<Element> elements;
  std::size_t bodyOffset = 0;
};

ScalarType requireType(std::string_view name, std::size_t line) {
  const auto type = scalarType(name);

  if (!type) {
    detail::failAtLine(line, "unknown property type \"" + std::string(name) + "\"");
  }

  return *type;
}

std::string_view requireToken(detail::Tokenizer &tokens, std::size_t line, const char *what) {
  std::string_view token;

  if (!tokens.next(token)) {
    detail::failAtLine(line, std::string("missing ") + what);
  }

  return token;
}

Header parseHeader(std::string_view bytes) {
  detail::LineReader lines(bytes);
  std::string_view line;

  if (!lines.next(line) || detail::trim(line) != "ply") {
    throw std::runtime_error("missing \"ply\" magic number");
  }

  Header header;
  bool hasFormat = false;

  while (true) {
    if (!lines.next(line)) {
      throw std::runtime_error("header has no end_header line");
    }

    const std::size_t number = lines.lineNumber();
    detail::Tokenizer tokens(line);
    std::string_view keyword;

    if (!tokens.next(keyword) || keyword == "comment" || keyword == "obj_info") {
      continue;
    }

    if (keyword == "end_header") {
      header.bodyOffset = lines.offset();
      break;
    }

    if (keyword == "format") {
      const std::string_view encoding = requireToken(tokens, number, "format encoding");
      const std::string_view version = requireToken(tokens, number, "format version");

      if (encoding == "ascii") {
        header.encoding = Encoding::Ascii;
      }
      else if (encoding == "binary_little_endian") {
        header.encoding = Encoding::LittleEndian;
      }
      else if (encoding == "binary_big_endian") {
        header.encoding = Encoding::BigEndian;
      }
      else {
        detail::failAtLine(number, "unknown format \"" + std::string(encoding) + "\"");
      }

      if (version != "1.0") {
        detail::failAtLine(number, "unsupported version \"" + std::string(version) + "\"");
      }

      hasFormat = true;
    }
    else if (keyword == "element") {
      Element element;
      element.name = requireToken(tokens, number, "element name");
      const auto count = detail::parseInteger(requireToken(tokens, number, "element count"));

      if (!count || *count < 0) {
        detail::failAtLine(number, "invalid element count");
      }

      element.count = static_cast<std::uint64_t>(*count);
      header.elements.push_back(std::move(element));
    }
    else if (keyword == "property") {
      if (header.elements.empty()) {
        detail::failAtLine(number, "property declared before any element");
      }

      Property property;
      const std::string_view type = requireToken(tokens, number, "property type");

      if (type == "list") {
        property.isList = true;
        property.countType = requireType(requireToken(tokens, number, "list count type"), number);
        property.type = requireType(requireToken(tokens, number, "list item type"), number);

        if (!isInteger(property.countType)) {
          detail::failAtLine(number, "list count type must be an integer type");
        }
      }
      else {
        property.type = requireType(type, number);
      }

      property.name = requireToken(tokens, number, "property name");
      header.elements.back().properties.push_back(std::move(property));
    }
    else {
      detail::failAtLine(number, "unknown header keyword \"" + std::string(keyword) + "\"");
    }
  }

  if (!hasFormat) {
    throw std::runtime_error("header has no format line");
  }

  return header;
}

class AsciiReader {
public:
  explicit AsciiReader(std::string_view body) : m_tokens(body) {
  }

  double number(ScalarType type) {
    const std::string_view token = next();

    if (isInteger(type)) {
      return static_cast<double>(integerFrom(token));
    }

    const auto value = detail::parseDouble(token);

    if (!value) {
      throw std::runtime_error("invalid number \"" + std::string(token) + "\" in data");
    }

    return *value;
  }

  std::int64_t integer(ScalarType) {
    return integerFrom(next());
  }

  void skip(ScalarType) {
    next();
  }

private:
  std::string_view next() {
    std::string_view token;

    if (!m_tokens.next(token)) {
      throw std::runtime_error("data ends before all elements were read");
    }

    return token;
  }

  static std::int64_t integerFrom(std::string_view token) {
    const auto value = detail::parseInteger(token);

    if (!value) {
      throw std::runtime_error("invalid integer \"" + std::string(token) + "\" in data");
    }

    return *value;
  }

  detail::Tokenizer m_tokens;
};

class BinaryReader {
public:
  BinaryReader(std::string_view body, bool swap) : m_body(body), m_swap(swap) {
  }

  double number(ScalarType type) {
    switch (type) {
    case ScalarType::Float32:
      return static_cast<double>(read<float>());
    case ScalarType::Float64:
      return read<double>();
    default:
      return static_cast<double>(integer(type));
    }
  }

  std::int64_t integer(ScalarType type) {
    switch (type) {
    case ScalarType::Int8:
      return read<std::int8_t>();
    case ScalarType::UInt8:
      return read<std::uint8_t>();
    case ScalarType::Int16:
      return read<std::int16_t>();
    case ScalarType::UInt16:
      return read<std::uint16_t>();
    case ScalarType::Int32:
      return read<std::int32_t>();
    case ScalarType::UInt32:
      return read<std::uint32_t>();
    default:
      throw std::runtime_error("expected an integer type in data");
    }
  }

  void skip(ScalarType type) {
    if (isInteger(type)) {
      integer(type);
    }
    else {
      number(type);
    }
  }

private:
  template <class T> T read() {
    if (m_body.size() - m_offset < sizeof(T)) {
      throw std::runtime_error("data ends before all elements were read");
    }

    std::array<char, sizeof(T)> raw;
    std::memcpy(raw.data(), m_body.data() + m_offset, sizeof(T));
    m_offset += sizeof(T);

    if (m_swap) {
      std::ranges::reverse(raw);
    }

    return std::bit_cast<T>(raw);
  }

  std::string_view m_body;
  bool m_swap = false;
  std::size_t m_offset = 0;
};

constexpr int noAxis = -1;

template <class Reader>
void readBody(Reader &reader, const Header &header, std::size_t bodySize, SurfaceData &surface) {
  bool hasVertices = false;
  bool hasFaces = false;
  std::vector<std::uint32_t> face;

  for (const Element &element : header.elements) {
    const bool isVertex = element.name == "vertex";
    const bool isFace = element.name == "face";
    std::vector<int> axisOf(element.properties.size(), noAxis);
    std::size_t indexList = element.properties.size();

    // Every stored element takes at least one byte, so a larger count cannot be honest.
    if (!element.properties.empty() && element.count > bodySize) {
      throw std::runtime_error("element \"" + element.name + "\" count exceeds the file size");
    }

    if (isVertex) {
      if (hasVertices) {
        throw std::runtime_error("more than one vertex element");
      }

      std::array<bool, 3> found{};

      for (std::size_t p = 0; p < element.properties.size(); ++p) {
        const Property &property = element.properties[p];
        const std::string_view name = property.name;
        const int axis = name == "x" ? 0 : name == "y" ? 1 : name == "z" ? 2 : noAxis;

        if (axis != noAxis && !property.isList) {
          axisOf[p] = axis;
          found[axis] = true;
        }
      }

      if (!found[0] || !found[1] || !found[2]) {
        throw std::runtime_error("vertex element lacks an x, y or z property");
      }

      surface.positions.reserve(element.count);
      hasVertices = true;
    }

    if (isFace) {
      if (hasFaces) {
        throw std::runtime_error("more than one face element");
      }

      for (std::size_t p = 0; p < element.properties.size(); ++p) {
        const Property &property = element.properties[p];

        if (property.isList &&
            (property.name == "vertex_indices" || property.name == "vertex_index")) {
          indexList = p;
          break;
        }
      }

      if (indexList == element.properties.size()) {
        throw std::runtime_error("face element lacks a vertex_indices list");
      }

      if (!isInteger(element.properties[indexList].type)) {
        throw std::runtime_error("vertex_indices must have an integer item type");
      }

      surface.faceOffsets.reserve(element.count + 1);
      hasFaces = true;
    }

    for (std::uint64_t i = 0; i < element.count; ++i) {
      glm::dvec3 position{0.0};

      for (std::size_t p = 0; p < element.properties.size(); ++p) {
        const Property &property = element.properties[p];

        if (property.isList) {
          const std::int64_t size = reader.integer(property.countType);

          if (size < 0) {
            throw std::runtime_error("negative list length in element \"" + element.name + "\"");
          }

          if (p == indexList) {
            face.clear();

            for (std::int64_t k = 0; k < size; ++k) {
              const std::int64_t index = reader.integer(property.type);

              if (index < 0 || index > std::numeric_limits<std::uint32_t>::max()) {
                throw std::runtime_error("face " + std::to_string(i) +
                                         " has an invalid vertex index " + std::to_string(index));
              }

              face.push_back(static_cast<std::uint32_t>(index));
            }
          }
          else {
            for (std::int64_t k = 0; k < size; ++k) {
              reader.skip(property.type);
            }
          }
        }
        else if (axisOf[p] != noAxis) {
          position[axisOf[p]] = reader.number(property.type);
        }
        else {
          reader.skip(property.type);
        }
      }

      if (isVertex) {
        surface.positions.push_back(position);
      }
      else if (isFace) {
        surface.addFace(face);
      }
    }
  }
}

template <class T> void appendLittleEndian(std::string &out, T value) {
  auto raw = std::bit_cast<std::array<char, sizeof(T)>>(value);

  if constexpr (std::endian::native == std::endian::big) {
    std::ranges::reverse(raw);
  }

  out.append(raw.data(), raw.size());
}

} // namespace

SurfaceData parsePly(std::string_view bytes) {
  const Header header = parseHeader(bytes);
  const std::string_view body = bytes.substr(header.bodyOffset);
  SurfaceData surface;

  if (header.encoding == Encoding::Ascii) {
    AsciiReader reader(body);
    readBody(reader, header, body.size(), surface);
  }
  else {
    const bool fileIsLittle = header.encoding == Encoding::LittleEndian;
    const bool hostIsLittle = std::endian::native == std::endian::little;
    BinaryReader reader(body, fileIsLittle != hostIsLittle);
    readBody(reader, header, body.size(), surface);
  }

  for (std::size_t f = 0; f < surface.faceCount(); ++f) {
    for (const std::uint32_t vertex : surface.face(f)) {
      if (vertex >= surface.positions.size()) {
        throw std::runtime_error("face " + std::to_string(f) + " references vertex " +
                                 std::to_string(vertex) + ", but the file has " +
                                 std::to_string(surface.positions.size()) + " vertices");
      }
    }
  }

  return surface;
}

std::string formatPly(const SurfaceData &surface) {
  std::size_t largestFace = 0;

  for (std::size_t f = 0; f < surface.faceCount(); ++f) {
    largestFace = std::max(largestFace, surface.face(f).size());
  }

  const bool wideCounts = largestFace > std::numeric_limits<std::uint8_t>::max();

  std::string out = "ply\n"
                    "format binary_little_endian 1.0\n"
                    "comment Written by Cartan\n";
  out += "element vertex " + std::to_string(surface.positions.size()) + "\n";
  out += "property double x\nproperty double y\nproperty double z\n";
  out += "element face " + std::to_string(surface.faceCount()) + "\n";
  out += wideCounts ? "property list uint uint vertex_indices\n"
                    : "property list uchar uint vertex_indices\n";
  out += "end_header\n";

  out.reserve(out.size() + surface.positions.size() * 3 * sizeof(double) +
              surface.faceCount() * sizeof(std::uint32_t) +
              surface.faceVertices.size() * sizeof(std::uint32_t));

  for (const glm::dvec3 &position : surface.positions) {
    appendLittleEndian(out, position.x);
    appendLittleEndian(out, position.y);
    appendLittleEndian(out, position.z);
  }

  for (std::size_t f = 0; f < surface.faceCount(); ++f) {
    const auto face = surface.face(f);

    if (wideCounts) {
      appendLittleEndian(out, static_cast<std::uint32_t>(face.size()));
    }
    else {
      appendLittleEndian(out, static_cast<std::uint8_t>(face.size()));
    }

    for (const std::uint32_t vertex : face) {
      appendLittleEndian(out, vertex);
    }
  }

  return out;
}

void writePly(const std::filesystem::path &path, const SurfaceData &surface) {
  const std::string bytes = formatPly(surface);
  std::ofstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("cannot open " + path.string() + " for writing");
  }

  file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));

  if (!file) {
    throw std::runtime_error("failed to write " + path.string());
  }
}

} // namespace cartan::io
