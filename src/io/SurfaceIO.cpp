#include "io/SurfaceIO.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace cartan::io {

namespace {

using PositionKey = std::array<float, 3>;

struct PositionHash {
  std::size_t operator()(const PositionKey &key) const {
    std::size_t hash = 0;

    for (const float value : key) {
      hash = hash * 1000003u ^ std::hash<float>{}(value);
    }

    return hash;
  }
};

} // namespace

core::Mesh loadSurface(const std::filesystem::path &path) {
  Assimp::Importer importer;
  // clang-format off
  const aiScene *scene = importer.ReadFile(path.string(),
                                           aiProcess_Triangulate 
                                           | aiProcess_JoinIdenticalVertices 
                                           | aiProcess_FindDegenerates 
                                           | aiProcess_SortByPType
                                           | aiProcess_ImproveCacheLocality);
  // clang-format on

  if (scene == nullptr || scene->mRootNode == nullptr) {
    throw std::runtime_error(importer.GetErrorString());
  }

  std::vector<glm::dvec3> positions;
  std::vector<core::Mesh::Triangle> triangles;
  std::unordered_map<PositionKey, std::uint32_t, PositionHash> welded;
  std::vector<std::uint32_t> remap;

  for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
    const aiMesh *mesh = scene->mMeshes[m];

    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) == 0) {
      continue;
    }

    remap.resize(mesh->mNumVertices);

    for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
      const aiVector3D &vertex = mesh->mVertices[v];
      const PositionKey key{vertex.x, vertex.y, vertex.z};
      const auto [entry,
                  inserted] = welded.try_emplace(key, static_cast<std::uint32_t>(positions.size()));

      if (inserted) {
        positions.emplace_back(vertex.x, vertex.y, vertex.z);
      }

      remap[v] = entry->second;
    }

    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
      const aiFace &face = mesh->mFaces[f];

      if (face.mNumIndices != 3) {
        continue;
      }

      const core::Mesh::Triangle triangle{remap[face.mIndices[0]], remap[face.mIndices[1]],
                                          remap[face.mIndices[2]]};

      if (triangle[0] == triangle[1] || triangle[1] == triangle[2] || triangle[2] == triangle[0]) {
        continue;
      }

      triangles.push_back(triangle);
    }
  }

  if (triangles.empty()) {
    throw std::runtime_error("file contains no triangles");
  }

  return core::Mesh::fromTriangles(std::move(positions), std::move(triangles));
}

std::string supportedExtensions() {
  Assimp::Importer importer;
  std::string list;
  importer.GetExtensionList(list);
  std::replace(list.begin(), list.end(), ';', ' ');

  return list;
}

} // namespace cartan::io