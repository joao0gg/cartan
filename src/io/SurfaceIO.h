#pragma once

#include <filesystem>
#include <string>

#include "render/RenderMesh.h"

namespace cartan::io {

render::RenderMesh loadSurface(const std::filesystem::path &path);

std::string supportedExtensions();

} // namespace cartan::io