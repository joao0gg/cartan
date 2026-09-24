#include "io/SurfaceIO.h"

#include <algorithm>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace cartan::io {

core::Mesh loadSurface(const std::filesystem::path &path) {
  Assimp::Importer importer;
  // clang-format off
  const aiScene *scene = importer.ReadFile(path.string(),
                                           aiProcess_Triangulate 
                                           | aiProcess_GenSmoothNormals 
                                           | aiProcess_JoinIdenticalVertices 
                                           | aiProcess_FindDegenerates 
                                           | aiProcess_SortByPType
                                           | aiProcess_ImproveCacheLocality);
  // clang-format on

  if (scene == nullptr || scene->mRootNode == nullptr) {
    throw std::runtime_error(importer.GetErrorString());
  }

  core::Mesh out;
  for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
    const aiMesh *mesh = scene->mMeshes[m];

    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) == 0) {
      continue;
    }

    const auto base = static_cast<std::uint32_t>(out.vertices.size());

    for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
      core::Mesh::Vertex vertex;
      vertex.position = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};

      if (mesh->HasNormals()) {
        vertex.normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};
      }

      out.vertices.push_back(vertex);
    }

    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
      const aiFace &face = mesh->mFaces[f];

      if (face.mNumIndices != 3) {
        continue;
      }

      out.indices.push_back(base + face.mIndices[0]);
      out.indices.push_back(base + face.mIndices[1]);
      out.indices.push_back(base + face.mIndices[2]);
    }
  }

  if (out.empty()) {
    throw std::runtime_error("file contains no triangles");
  }

  out.computeBounds();
  return out;
}

std::string supportedExtensions() {
  Assimp::Importer importer;
  std::string list;
  importer.GetExtensionList(list);
  std::replace(list.begin(), list.end(), ';', ' ');

  return list;
}

} // namespace cartan::io