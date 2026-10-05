// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>

#include "core/Mesh.h"

namespace cartan::io {

struct SurfaceData {
  std::vector<glm::dvec3> positions;

  std::vector<std::size_t> faceOffsets{0};
  std::vector<std::uint32_t> faceVertices;

  std::size_t faceCount() const {
    return faceOffsets.size() - 1;
  }

  std::span<const std::uint32_t> face(std::size_t f) const;
  void addFace(std::span<const std::uint32_t> vertices);

  bool operator==(const SurfaceData &) const = default;
};

SurfaceData readSurface(const std::filesystem::path &path);

SurfaceData parsePly(std::string_view bytes);
SurfaceData parseObj(std::string_view text);
SurfaceData parseOff(std::string_view text);

std::string formatPly(const SurfaceData &surface);
void writePly(const std::filesystem::path &path, const SurfaceData &surface);

core::Mesh toTriangleMesh(const SurfaceData &surface);

core::Mesh loadSurface(const std::filesystem::path &path);

std::string supportedExtensions();

} // namespace cartan::io
