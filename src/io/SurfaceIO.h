#pragma once

#include <filesystem>
#include <string>

#include "core/Mesh.h"

namespace cartan::io {

core::Mesh loadSurface(const std::filesystem::path &path);

std::string supportedExtensions();

} // namespace cartan::io