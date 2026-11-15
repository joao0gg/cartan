#pragma once

#include <array>
#include <string_view>

#include <glm/vec3.hpp>

namespace cartan::render {

enum class Colormap {
  Rainbow,
  BlueWhiteRed,
  Grayscale,
};

inline constexpr std::array kColormaps = {
    Colormap::Rainbow,
    Colormap::BlueWhiteRed,
    Colormap::Grayscale,
};

std::string_view colormapName(Colormap colormap);
glm::vec3 sampleColormap(Colormap colormap, float t);

} // namespace cartan::render
