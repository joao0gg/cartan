#pragma once

#include <filesystem>

#include "io/meshdata.h"

namespace cartan::io {

MeshData readObj(const std::filesystem::path &path);

} // namespace cartan::io
