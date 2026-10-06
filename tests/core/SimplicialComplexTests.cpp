// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "core/SimplicialComplex.h"

namespace {

using cartan::core::Index;
using cartan::core::SimplicialComplex;
using cartan::core::SparseMatrix;

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

SimplicialComplex build(int dimension, Index vertexCount, const std::vector<Index> &vertices) {
  return SimplicialComplex::fromTopSimplices(dimension, vertexCount, vertices);
}

bool isZero(const SparseMatrix &matrix) {
  return matrix.squaredNorm() == 0.0;
}

bool ddIsZero(const SimplicialComplex &complex) {
  for (int k = 0; k + 1 < complex.dimension(); ++k) {
    if (!isZero(complex.d(k + 1) * complex.d(k))) {
      return false;
    }
  }

  return true;
}

bool cancelsOnClosedSurface(const SimplicialComplex &complex) {
  const Eigen::RowVectorXd ones = Eigen::RowVectorXd::Ones(complex.count(2));

  return (ones * complex.d(1)).norm() == 0.0;
}

int rank(const SimplicialComplex &complex, int k) {
  if (k < 0 || k >= complex.dimension()) {
    return 0;
  }

  const Eigen::MatrixXd dense = Eigen::MatrixXd(complex.d(k));

  return static_cast<int>(Eigen::FullPivLU<Eigen::MatrixXd>(dense).rank());
}

int betti(const SimplicialComplex &complex, int k) {
  return complex.count(k) - rank(complex, k) - rank(complex, k - 1);
}

Index edgeIndex(const SimplicialComplex &complex, Index a, Index b) {
  for (Index e = 0; e < complex.count(1); ++e) {
    const auto edge = complex.simplex(1, e);

    if (edge[0] == std::min(a, b) && edge[1] == std::max(a, b)) {
      return e;
    }
  }

  throw std::invalid_argument("no such edge");
}

std::vector<Index> torus(Index rows, Index columns) {
  std::vector<Index> vertices;

  for (Index i = 0; i < rows; ++i) {
    for (Index j = 0; j < columns; ++j) {
      const Index a = i * columns + j;
      const Index b = ((i + 1) % rows) * columns + j;
      const Index c = ((i + 1) % rows) * columns + (j + 1) % columns;
      const Index d = i * columns + (j + 1) % columns;
      vertices.insert(vertices.end(), {a, b, c, a, c, d});
    }
  }

  return vertices;
}

void testTriangle() {
  const auto complex = build(2, 3, {0, 1, 2});

  expect(complex.count(0) == 3 && complex.count(1) == 3 && complex.count(2) == 1,
         "triangle: simplex counts");
  expect(ddIsZero(complex), "triangle: dd = 0");
  expect(complex.d(0).nonZeros() == 6 && complex.d(1).nonZeros() == 3, "triangle: incidence sizes");
}

void testSphere() {
  const auto complex = build(2, 4, {0, 2, 1, 0, 1, 3, 0, 3, 2, 1, 2, 3});

  expect(complex.eulerCharacteristic() == 2, "sphere: Euler characteristic 2");
  expect(ddIsZero(complex), "sphere: dd = 0");
  expect(cancelsOnClosedSurface(complex), "sphere: orientation is kept from the input");
  expect(betti(complex, 0) == 1 && betti(complex, 1) == 0 && betti(complex, 2) == 1,
         "sphere: Betti numbers 1, 0, 1");
}

void testTorus() {
  const auto complex = build(2, 20, torus(4, 5));

  expect(complex.eulerCharacteristic() == 0, "torus: Euler characteristic 0");
  expect(ddIsZero(complex), "torus: dd = 0");
  expect(cancelsOnClosedSurface(complex), "torus: orientation is kept from the input");
  expect(betti(complex, 0) == 1 && betti(complex, 1) == 2 && betti(complex, 2) == 1,
         "torus: Betti numbers 1, 2, 1");
}

void testTetrahedra() {
  const auto one = build(3, 4, {0, 1, 2, 3});

  expect(one.count(0) == 4 && one.count(1) == 6 && one.count(2) == 4 && one.count(3) == 1,
         "tetrahedron: simplex counts");
  expect(one.eulerCharacteristic() == 1, "tetrahedron: Euler characteristic 1");
  expect(ddIsZero(one), "tetrahedron: dd = 0 in every degree");

  const auto two = build(3, 5, {0, 1, 2, 3, 1, 2, 3, 4});

  expect(two.count(1) == 9 && two.count(2) == 7 && two.count(3) == 2,
         "two tetrahedra: shared face counted once");
  expect(ddIsZero(two), "two tetrahedra: dd = 0");
  expect(betti(two, 0) == 1 && betti(two, 1) == 0 && betti(two, 2) == 0 && betti(two, 3) == 0,
         "two tetrahedra: contractible");
}

void testNonmanifold() {
  const auto bowtie = build(2, 5, {0, 1, 2, 0, 3, 4});

  expect(bowtie.count(1) == 6 && bowtie.eulerCharacteristic() == 1,
         "bowtie: triangles sharing only a vertex");
  expect(ddIsZero(bowtie), "bowtie: dd = 0");

  const auto fin = build(2, 5, {0, 1, 2, 0, 1, 3, 0, 1, 4});
  const SparseMatrix d1 = fin.d(1);

  expect(fin.count(1) == 7, "fin: three triangles on one edge");
  expect(d1.col(edgeIndex(fin, 0, 1)).nonZeros() == 3, "fin: the shared edge has three cofaces");
  expect(ddIsZero(fin), "fin: dd = 0");
}

void testOrderAndOrientation() {
  const auto complex = build(2, 4, {0, 2, 1});

  expect(complex.simplex(2, 0)[1] == 2, "top simplices keep their input vertex order");
  expect(complex.simplex(1, 0)[0] == 1 && complex.simplex(1, 0)[1] == 2,
         "edges are sorted and numbered in boundary order: omit vertex 0 first");
  expect(complex.count(0) == 4 && betti(complex, 0) == 2, "isolated vertices are kept");

  const auto flipped = build(2, 3, {0, 1, 2});
  bool negated = true;

  for (const auto &[a, b] : {std::pair{0, 1}, std::pair{1, 2}, std::pair{0, 2}}) {
    negated = negated && complex.d(1).coeff(0, edgeIndex(complex, a, b)) ==
                             -flipped.d(1).coeff(0, edgeIndex(flipped, a, b));
  }

  expect(negated, "reversing a triangle negates its row of d");
}

void testErrors() {
  expect(throws([] {
           build(2, 3, {0, 1, 1});
         }),
         "repeated vertex throws");
  expect(throws([] {
           build(2, 3, {0, 1, 2, 2, 1, 0});
         }),
         "duplicate simplex throws");
  expect(throws([] {
           build(2, 3, {0, 1, 3});
         }),
         "out-of-range vertex throws");
  expect(throws([] {
           build(2, 3, {0, 1});
         }),
         "incomplete simplex throws");
  expect(throws([] {
           build(4, 5, {0, 1, 2, 3, 4});
         }),
         "dimension above 3 throws");
  expect(throws([] {
           build(2, 3, {0, 1, 2}).d(2);
         }),
         "d beyond the top dimension throws");
}

} // namespace

int main() {
  testTriangle();
  testSphere();
  testTorus();
  testTetrahedra();
  testNonmanifold();
  testOrderAndOrientation();
  testErrors();

  return failures == 0 ? 0 : 1;
}
