#pragma once

#include <filesystem>
#include <string>

namespace cartan::io {

std::string readTextFile(const std::filesystem::path &path);

} // namespace cartan::io