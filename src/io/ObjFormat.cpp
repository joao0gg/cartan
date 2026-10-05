// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <cstdint>
#include <string>
#include <vector>

#include "io/SurfaceIO.h"
#include "io/detail/Text.h"

namespace cartan::io {

SurfaceData parseObj(std::string_view text) {
  SurfaceData surface;
  detail::LineReader lines(text);
  std::string_view line;
  std::vector<std::uint32_t> face;

  while (lines.next(line)) {
    detail::Tokenizer tokens(detail::stripComment(line));
    std::string_view keyword;

    if (!tokens.next(keyword)) {
      continue;
    }

    if (keyword == "v") {
      glm::dvec3 position{0.0};

      for (int axis = 0; axis < 3; ++axis) {
        std::string_view token;

        if (!tokens.next(token)) {
          detail::failAtLine(lines.lineNumber(), "vertex has fewer than 3 coordinates");
        }

        const auto value = detail::parseDouble(token);

        if (!value) {
          detail::failAtLine(lines.lineNumber(),
                             "invalid coordinate \"" + std::string(token) + "\"");
        }

        position[axis] = *value;
      }

      surface.positions.push_back(position);
    }
    else if (keyword == "f") {
      face.clear();
      std::string_view token;

      while (tokens.next(token)) {
        const auto reference = detail::parseInteger(token.substr(0, token.find('/')));

        if (!reference || *reference == 0) {
          detail::failAtLine(lines.lineNumber(),
                             "invalid vertex reference \"" + std::string(token) + "\"");
        }

        const auto defined = static_cast<std::int64_t>(surface.positions.size());
        const std::int64_t index = *reference > 0 ? *reference - 1 : defined + *reference;

        if (index < 0 || index >= defined) {
          detail::failAtLine(lines.lineNumber(), "vertex reference " + std::to_string(*reference) +
                                                     " is out of range (" +
                                                     std::to_string(defined) +
                                                     " vertices defined so far)");
        }

        face.push_back(static_cast<std::uint32_t>(index));
      }

      if (face.empty()) {
        detail::failAtLine(lines.lineNumber(), "face has no vertices");
      }

      surface.addFace(face);
    }
  }

  return surface;
}

} // namespace cartan::io
