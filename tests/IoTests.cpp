// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <locale>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "io/SurfaceIO.h"

namespace {

using cartan::io::SurfaceData;

int failures = 0;

void expect(bool condition, const char *what) {
  if (!condition) {
    std::fprintf(stderr, "FAILED: %s\n", what);
    ++failures;
  }
}

template <class Function> bool throws(Function function) {
  try {
    function();
  }
  catch (const std::exception &) {
    return true;
  }

  return false;
}

bool faceIs(const SurfaceData &surface, std::size_t f, std::vector<std::uint32_t> expected) {
  const auto face = surface.face(f);

  return std::ranges::equal(face, expected);
}

bool roundTrips(const SurfaceData &surface) {
  return cartan::io::parsePly(cartan::io::formatPly(surface)) == surface;
}

template <class T> void appendBytes(std::string &out, T value, std::endian order) {
  auto raw = std::bit_cast<std::array<char, sizeof(T)>>(value);

  if (order != std::endian::native) {
    std::ranges::reverse(raw);
  }

  out.append(raw.data(), raw.size());
}

constexpr std::string_view objText = "# comment\r\n"
                                     "mtllib scene.mtl\r\n"
                                     "v 0.1 0.2 0.3\r\n"
                                     "v 812345.6789012345 -1e-300 +2.5\r\n"
                                     "v 0.1 0.2 0.3\r\n"
                                     "v 1 1 1 0.5 0.5 0.5\r\n"
                                     "vn 0 0 1\r\n"
                                     "g part\r\n"
                                     "f 1 2 3\r\n"
                                     "f 1/1 2//1 3/1/1\r\n"
                                     "f -4 -3 -3 # trailing comment\r\n"
                                     "f 1 2 3 4\r\n";

void testObjIsFaithful() {
  const SurfaceData surface = cartan::io::parseObj(objText);

  expect(surface.positions.size() == 4, "OBJ: duplicate positions are not welded");
  expect(surface.positions[0].x == 0.1, "OBJ: 0.1 parses to the nearest double");
  expect(surface.positions[1].x == 812345.6789012345, "OBJ: large coordinate keeps every digit");
  expect(surface.positions[1].y == -1e-300, "OBJ: tiny coordinate is preserved");
  expect(surface.positions[1].z == 2.5, "OBJ: leading plus sign is accepted");
  expect(surface.positions[2] == surface.positions[0], "OBJ: duplicate keeps its own index");
  expect(surface.faceCount() == 4, "OBJ: every face is kept");
  expect(faceIs(surface, 0, {0, 1, 2}), "OBJ: plain references");
  expect(faceIs(surface, 1, {0, 1, 2}), "OBJ: slash references");
  expect(faceIs(surface, 2, {0, 1, 1}), "OBJ: relative references and repeated vertex kept");
  expect(faceIs(surface, 3, {0, 1, 2, 3}), "OBJ: quad is not triangulated");
  expect(roundTrips(surface), "OBJ: data survives a PLY round trip");
}

void testObjErrors() {
  expect(throws([] {
           cartan::io::parseObj("v 0 0 0\nf 1 1 2\n");
         }),
         "OBJ: reference to an undefined vertex throws");
  expect(throws([] {
           cartan::io::parseObj("v 0 0 0\nf 0 1 1\n");
         }),
         "OBJ: reference 0 throws");
  expect(throws([] {
           cartan::io::parseObj("v 0,5 0 0\n");
         }),
         "OBJ: decimal comma throws");
  expect(throws([] {
           cartan::io::parseObj("v 0 0\n");
         }),
         "OBJ: missing coordinate throws");
}

void testOff() {
  const SurfaceData surface = cartan::io::parseOff("OFF 4 2 0\n"
                                                   "# comment\n"
                                                   "0 0 0\n"
                                                   "1 0 0 255 0 0\n"
                                                   "\n"
                                                   "0 1 0\n"
                                                   "0 0 1\n"
                                                   "3 0 1 2 255 255 255\n"
                                                   "4 0 1 2 3\n");

  expect(surface.positions.size() == 4, "OFF: counts on the header line");
  expect(surface.positions[1].x == 1.0, "OFF: vertex color is ignored");
  expect(faceIs(surface, 0, {0, 1, 2}), "OFF: face color is ignored");
  expect(faceIs(surface, 1, {0, 1, 2, 3}), "OFF: quad is not triangulated");
  expect(roundTrips(surface), "OFF: data survives a PLY round trip");

  const SurfaceData separate = cartan::io::parseOff("COFF\n3 1 3\n0 0 0 1 1 1 1\n1 0 0 1 1 1 1\n"
                                                    "0 1 0 1 1 1 1\n3 2 1 0\n");
  expect(faceIs(separate, 0, {2, 1, 0}), "OFF: counts on their own line, COFF header");

  const SurfaceData headerless = cartan::io::parseOff("3 1 0\n0 0 0\n1 0 0\n0 1 0\n3 0 1 2\n");
  expect(headerless.faceCount() == 1, "OFF: the header keyword is optional");

  expect(throws([] {
           cartan::io::parseOff("OFF\n3 1 0\n0 0 0\n1 0 0\n");
         }),
         "OFF: truncated file throws");
  expect(throws([] {
           cartan::io::parseOff("OFF\n3 1 0\n0 0 0\n1 0 0\n0 1 0\n3 0 1 3\n");
         }),
         "OFF: out-of-range index throws");
  expect(throws([] {
           cartan::io::parseOff("4OFF\n1 0 0\n0 0 0 1\n");
         }),
         "OFF: four-dimensional variant throws");
}

void testAsciiPly() {
  const SurfaceData surface = cartan::io::parsePly("ply\r\n"
                                                   "format ascii 1.0\r\n"
                                                   "comment made by hand\r\n"
                                                   "element vertex 3\r\n"
                                                   "property float x\r\n"
                                                   "property float y\r\n"
                                                   "property float nx\r\n"
                                                   "property float z\r\n"
                                                   "property uchar red\r\n"
                                                   "element face 1\r\n"
                                                   "property uchar flags\r\n"
                                                   "property list uchar int vertex_indices\r\n"
                                                   "end_header\r\n"
                                                   "0.1 0.2 9 0.3 255\r\n"
                                                   "1 0 9 0 255\r\n"
                                                   "0 1 9 0 255\r\n"
                                                   "7 3 2 1 0\r\n");

  expect(surface.positions.size() == 3, "ASCII PLY: vertex count");
  expect(surface.positions[0] == glm::dvec3(0.1, 0.2, 0.3),
         "ASCII PLY: text is parsed at full precision, extra properties skipped");
  expect(faceIs(surface, 0, {2, 1, 0}), "ASCII PLY: face order and winding kept");
  expect(roundTrips(surface), "ASCII PLY: data survives a PLY round trip");
}

void testBinaryPly(std::endian order) {
  const bool little = order == std::endian::little;
  std::string bytes = "ply\n";
  bytes += little ? "format binary_little_endian 1.0\n" : "format binary_big_endian 1.0\n";
  bytes += "element vertex 3\n"
           "property float x\nproperty double y\nproperty float z\n"
           "element face 2\n"
           "property list uchar int vertex_indices\n"
           "property list uchar float texcoord\n"
           "end_header\n";

  const std::array<float, 3> xs{0.1f, 1.0f, 0.0f};
  const std::array<double, 3> ys{0.2, 0.0, 812345.6789012345};
  const std::array<float, 3> zs{0.3f, 0.0f, 0.0f};

  for (std::size_t v = 0; v < 3; ++v) {
    appendBytes(bytes, xs[v], order);
    appendBytes(bytes, ys[v], order);
    appendBytes(bytes, zs[v], order);
  }

  for (int f = 0; f < 2; ++f) {
    appendBytes(bytes, std::uint8_t{3}, order);
    appendBytes(bytes, std::int32_t{0}, order);
    appendBytes(bytes, std::int32_t{f == 0 ? 1 : 2}, order);
    appendBytes(bytes, std::int32_t{f == 0 ? 2 : 1}, order);
    appendBytes(bytes, std::uint8_t{2}, order);
    appendBytes(bytes, 0.5f, order);
    appendBytes(bytes, 0.5f, order);
  }

  const SurfaceData surface = cartan::io::parsePly(bytes);

  expect(surface.positions[0].x == static_cast<double>(0.1f),
         "binary PLY: float32 widens exactly to double");
  expect(surface.positions[2].y == 812345.6789012345, "binary PLY: float64 is read exactly");
  expect(faceIs(surface, 1, {0, 2, 1}), "binary PLY: second face, texcoord list skipped");
  expect(roundTrips(surface), "binary PLY: data survives a PLY round trip");

  bytes.pop_back();
  expect(throws([&] {
           cartan::io::parsePly(bytes);
         }),
         "binary PLY: truncated data throws");
}

void testPlyErrors() {
  expect(throws([] {
           cartan::io::parsePly("plx\n");
         }),
         "PLY: bad magic number throws");
  expect(throws([] {
           cartan::io::parsePly("ply\nformat ascii 1.0\nelement vertex 1\nproperty float x\n"
                                "property float y\nproperty float z\nelement face 1\n"
                                "property list uchar int vertex_indices\nend_header\n"
                                "0 0 0\n3 0 0 1\n");
         }),
         "PLY: out-of-range index throws");
  expect(throws([] {
           cartan::io::parsePly("ply\nformat ascii 1.0\nelement vertex 99999999999\n"
                                "property float x\nproperty float y\nproperty float z\n"
                                "end_header\n0 0 0\n");
         }),
         "PLY: absurd element count throws instead of allocating");
}

void testLargePolygonRoundTrip() {
  SurfaceData surface;
  std::vector<std::uint32_t> face;

  for (std::uint32_t v = 0; v < 300; ++v) {
    surface.positions.emplace_back(v, 0.0, 0.0);
    face.push_back(v);
  }

  surface.addFace(face);
  expect(roundTrips(surface), "writer switches to wide counts for faces over 255 vertices");
}

void testFileRoundTrip() {
  const SurfaceData surface = cartan::io::parseObj(objText);
  const auto path = std::filesystem::temp_directory_path() / "cartan_io_tests.ply";

  cartan::io::writePly(path, surface);
  expect(cartan::io::readSurface(path) == surface, "file round trip through writePly/readSurface");
  std::filesystem::remove(path);

  expect(throws([] {
           cartan::io::readSurface("mesh.stl");
         }),
         "unsupported extension throws");
}

void testTriangleMeshConversion() {
  const SurfaceData triangles = cartan::io::parseObj("v 0 0 0\nv 1 0 0\nv 0 1 0\nv 0 0 1\n"
                                                     "f 1 3 2\nf 1 2 4\n");
  const auto mesh = cartan::io::toTriangleMesh(triangles);

  expect(mesh.positions() == triangles.positions, "conversion keeps positions");
  expect(mesh.triangles()[0] == cartan::core::Mesh::Triangle{0, 2, 1}, "conversion keeps order");

  const SurfaceData quad = cartan::io::parseObj("v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\nf 1 2 3 4\n");
  expect(throws([&] {
           cartan::io::toTriangleMesh(quad);
         }),
         "conversion refuses to triangulate");
}

void testLocaleIndependence() {
  for (const char *name : {"pt_BR.UTF-8", "de_DE.UTF-8"}) {
    try {
      std::locale::global(std::locale(name));
    }
    catch (const std::runtime_error &) {
      continue;
    }

    const SurfaceData surface = cartan::io::parseObj("v 0.5 1.5 2.5\n");
    expect(surface.positions[0].x == 0.5, "parsing ignores a decimal-comma locale");
    std::locale::global(std::locale::classic());
  }
}

} // namespace

int main() {
  testObjIsFaithful();
  testObjErrors();
  testOff();
  testAsciiPly();
  testBinaryPly(std::endian::little);
  testBinaryPly(std::endian::big);
  testPlyErrors();
  testLargePolygonRoundTrip();
  testFileRoundTrip();
  testTriangleMeshConversion();
  testLocaleIndependence();

  return failures == 0 ? 0 : 1;
}
