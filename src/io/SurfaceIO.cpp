// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "io/SurfaceIO.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cartan::io {

namespace {

std::string readFile(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file) {
    throw std::runtime_error("cannot open " + path.string());
  }

  const std::streamsize size = file.tellg();
  std::string bytes(static_cast<std::size_t>(size), '\0');
  file.seekg(0);
  file.read(bytes.data(), size);

  if (!file) {
    throw std::runtime_error("failed to read " + path.string());
  }

  return bytes;
}

std::string lowercaseExtension(const std::filesystem::path &path) {
  std::string extension = path.extension().string();
  std::ranges::transform(extension, extension.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });

  return extension;
}

} // namespace

std::span<const std::uint32_t> SurfaceData::face(std::size_t f) const {
  return std::span(faceVertices).subspan(faceOffsets[f], faceOffsets[f + 1] - faceOffsets[f]);
}

void SurfaceData::addFace(std::span<const std::uint32_t> vertices) {
  faceVertices.insert(faceVertices.end(), vertices.begin(), vertices.end());
  faceOffsets.push_back(faceVertices.size());
}

SurfaceData readSurface(const std::filesystem::path &path) {
  const std::string extension = lowercaseExtension(path);

  if (extension != ".ply" && extension != ".obj" && extension != ".off") {
    throw std::runtime_error("unsupported file extension \"" + extension + "\"");
  }

  const std::string bytes = readFile(path);

  if (extension == ".ply") {
    return parsePly(bytes);
  }

  if (extension == ".obj") {
    return parseObj(bytes);
  }

  return parseOff(bytes);
}

core::Mesh toTriangleMesh(const SurfaceData &surface) {
  std::vector<core::Mesh::Triangle> triangles;
  triangles.reserve(surface.faceCount());

  for (std::size_t f = 0; f < surface.faceCount(); ++f) {
    const auto face = surface.face(f);

    if (face.size() != 3) {
      throw std::invalid_argument("face " + std::to_string(f) + " has " +
                                  std::to_string(face.size()) +
                                  " vertices; a triangle mesh needs triangles only");
    }

    triangles.push_back({face[0], face[1], face[2]});
  }

  return core::Mesh::fromTriangles(surface.positions, std::move(triangles));
}

core::Mesh loadSurface(const std::filesystem::path &path) {
  const SurfaceData surface = readSurface(path);

  if (surface.faceCount() == 0) {
    throw std::runtime_error("file contains no faces");
  }

  return toTriangleMesh(surface);
}

std::string supportedExtensions() {
  return "*.ply *.obj *.off";
}

} // namespace cartan::io
