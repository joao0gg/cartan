#include <cstdio>
#include <stdexcept>
#include <utility>
#include <vector>

#include "core/Mesh.h"

namespace {

using cartan::core::Mesh;

int failures = 0;

void expect(bool condition, const char *what) {
  if (!condition) {
    std::fprintf(stderr, "FAILED: %s\n", what);
    ++failures;
  }
}

Mesh tetrahedron() {
  return Mesh::fromTriangles({{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
                             {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}});
}

void testEulerCharacteristic() {
  const Mesh mesh = tetrahedron();

  const auto vertices = static_cast<long>(mesh.positions().size());
  const auto edges = static_cast<long>(mesh.edges().size());
  const auto faces = static_cast<long>(mesh.triangles().size());

  expect(vertices == 4, "tetrahedron has 4 vertices");
  expect(edges == 6, "tetrahedron has 6 edges");
  expect(faces == 4, "tetrahedron has 4 faces");
  expect(vertices - edges + faces == 2, "tetrahedron has Euler characteristic 2");
}

void testEdgeOrientation() {
  const Mesh mesh = tetrahedron();

  for (const auto &edge : mesh.edges()) {
    expect(edge[0] < edge[1], "edges run from lower to higher vertex index");
  }
}

void testBoundaryOfBoundaryIsZero() {
  const Mesh mesh = tetrahedron();
  const auto &edges = mesh.edges();

  for (std::size_t t = 0; t < mesh.triangles().size(); ++t) {
    std::vector<int> coefficients(mesh.positions().size(), 0);

    for (std::size_t k = 0; k < 3; ++k) {
      const auto &edge = edges[mesh.triangleEdges()[t][k]];
      const int sign = mesh.triangleEdgeSigns()[t][k];

      coefficients[edge[1]] += sign;
      coefficients[edge[0]] -= sign;
    }

    for (const int coefficient : coefficients) {
      expect(coefficient == 0, "boundary of triangle boundary vanishes on every vertex");
    }
  }
}

bool throwsInvalidArgument(std::vector<glm::dvec3> positions,
                           std::vector<Mesh::Triangle> triangles) {
  try {
    Mesh::fromTriangles(std::move(positions), std::move(triangles));
  }
  catch (const std::invalid_argument &) {
    return true;
  }

  return false;
}

void testInvalidInputThrows() {
  const std::vector<glm::dvec3> positions{{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};

  expect(throwsInvalidArgument(positions, {{0, 1, 3}}), "out-of-range index throws");
  expect(throwsInvalidArgument(positions, {{0, 1, 1}}), "repeated index throws");
  expect(throwsInvalidArgument(positions, {{2, 1, 2}}), "repeated first and last index throws");
}

} // namespace

int main() {
  testEulerCharacteristic();
  testEdgeOrientation();
  testBoundaryOfBoundaryIsZero();
  testInvalidInputThrows();

  return failures == 0 ? 0 : 1;
}
