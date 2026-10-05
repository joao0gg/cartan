// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "io/SurfaceIO.h"
#include "io/detail/Text.h"

namespace cartan::io {

namespace {

bool isSupportedHeader(std::string_view token) {
  static constexpr std::array<std::string_view, 8> headers{"OFF",   "COFF",   "NOFF",   "CNOFF",
                                                           "STOFF", "STCOFF", "STNOFF", "STCNOFF"};

  return std::ranges::find(headers, token) != headers.end();
}

bool nextContentLine(detail::LineReader &lines, std::string_view &line) {
  std::string_view raw;

  while (lines.next(raw)) {
    line = detail::trim(detail::stripComment(raw));

    if (!line.empty()) {
      return true;
    }
  }

  return false;
}

std::int64_t readCount(detail::Tokenizer &tokens, std::size_t lineNumber, const char *what) {
  std::string_view token;

  if (!tokens.next(token)) {
    detail::failAtLine(lineNumber, std::string("missing ") + what);
  }

  const auto value = detail::parseInteger(token);

  if (!value || *value < 0) {
    detail::failAtLine(lineNumber,
                       std::string("invalid ") + what + " \"" + std::string(token) + "\"");
  }

  return *value;
}

} // namespace

SurfaceData parseOff(std::string_view text) {
  detail::LineReader lines(text);
  std::string_view line;

  if (!nextContentLine(lines, line)) {
    throw std::runtime_error("file is empty");
  }

  std::string_view countsLine = line;
  std::size_t countsLineNumber = lines.lineNumber();
  {
    detail::Tokenizer tokens(line);
    std::string_view first;
    tokens.next(first);

    if (isSupportedHeader(first)) {
      std::string_view rest = detail::trim(line.substr(first.size()));

      if (rest.starts_with("BINARY")) {
        detail::failAtLine(lines.lineNumber(), "binary OFF is not supported");
      }

      if (rest.empty()) {
        if (!nextContentLine(lines, line)) {
          detail::failAtLine(lines.lineNumber(), "missing vertex and face counts");
        }

        rest = line;
      }

      countsLine = rest;
      countsLineNumber = lines.lineNumber();
    }
    else if (first.ends_with("OFF")) {
      detail::failAtLine(lines.lineNumber(),
                         "unsupported OFF variant \"" + std::string(first) + "\"");
    }
  }

  detail::Tokenizer counts(countsLine);
  const std::int64_t vertexCount = readCount(counts, countsLineNumber, "vertex count");
  const std::int64_t faceCount = readCount(counts, countsLineNumber, "face count");

  SurfaceData surface;
  surface.positions.reserve(
      static_cast<std::size_t>(std::min<std::int64_t>(vertexCount, std::ssize(text))));

  for (std::int64_t v = 0; v < vertexCount; ++v) {
    if (!nextContentLine(lines, line)) {
      throw std::runtime_error("file ends after " + std::to_string(v) + " of " +
                               std::to_string(vertexCount) + " vertices");
    }

    detail::Tokenizer tokens(line);
    glm::dvec3 position{0.0};

    for (int axis = 0; axis < 3; ++axis) {
      std::string_view token;

      if (!tokens.next(token)) {
        detail::failAtLine(lines.lineNumber(), "vertex has fewer than 3 coordinates");
      }

      const auto value = detail::parseDouble(token);

      if (!value) {
        detail::failAtLine(lines.lineNumber(), "invalid coordinate \"" + std::string(token) + "\"");
      }

      position[axis] = *value;
    }

    surface.positions.push_back(position);
  }

  std::vector<std::uint32_t> face;

  for (std::int64_t f = 0; f < faceCount; ++f) {
    if (!nextContentLine(lines, line)) {
      throw std::runtime_error("file ends after " + std::to_string(f) + " of " +
                               std::to_string(faceCount) + " faces");
    }

    detail::Tokenizer tokens(line);
    const std::int64_t size = readCount(tokens, lines.lineNumber(), "face size");

    if (size == 0) {
      detail::failAtLine(lines.lineNumber(), "face has no vertices");
    }

    face.clear();

    for (std::int64_t k = 0; k < size; ++k) {
      std::string_view token;

      if (!tokens.next(token)) {
        detail::failAtLine(lines.lineNumber(), "face has fewer indices than its size");
      }

      const auto index = detail::parseInteger(token);

      if (!index || *index < 0 || *index >= vertexCount) {
        detail::failAtLine(lines.lineNumber(),
                           "invalid or out-of-range vertex index \"" + std::string(token) + "\"");
      }

      face.push_back(static_cast<std::uint32_t>(*index));
    }

    surface.addFace(face);
  }

  return surface;
}

} // namespace cartan::io
