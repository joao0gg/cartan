#include "render/colormap.h"

#include <algorithm>
#include <cmath>
#include <span>

namespace cartan::render {

namespace {

constexpr glm::vec3 kBlue{0.0f, 0.0f, 1.0f};
constexpr glm::vec3 kCyan{0.0f, 1.0f, 1.0f};
constexpr glm::vec3 kGreen{0.0f, 1.0f, 0.0f};
constexpr glm::vec3 kYellow{1.0f, 1.0f, 0.0f};
constexpr glm::vec3 kRed{1.0f, 0.0f, 0.0f};
constexpr glm::vec3 kWhite{1.0f, 1.0f, 1.0f};
constexpr glm::vec3 kBlack{0.0f, 0.0f, 0.0f};

constexpr std::array kRainbow = {kBlue, kCyan, kGreen, kYellow, kRed};
constexpr std::array kBlueWhiteRed = {kBlue, kWhite, kRed};
constexpr std::array kGrayscale = {kBlack, kWhite};

std::span<const glm::vec3> stops(Colormap colormap) {
  switch (colormap) {
  case Colormap::Rainbow:
    return kRainbow;
  case Colormap::BlueWhiteRed:
    return kBlueWhiteRed;
  case Colormap::Grayscale:
    return kGrayscale;
  }

  return kGrayscale;
}

} // namespace

std::string_view colormapName(Colormap colormap) {
  switch (colormap) {
  case Colormap::Rainbow:
    return "Rainbow";
  case Colormap::BlueWhiteRed:
    return "Blue White Red";
  case Colormap::Grayscale:
    return "Grayscale";
  }

  return "";
}

glm::vec3 sampleColormap(Colormap colormap, float t) {
  const std::span<const glm::vec3> colors = stops(colormap);

  const float position = std::clamp(t, 0.0f, 1.0f) * float(colors.size() - 1);
  const auto index = std::min(static_cast<std::size_t>(position), colors.size() - 2);
  const float fraction = position - float(index);

  return colors[index] + (colors[index + 1] - colors[index]) * fraction;
}

} // namespace cartan::render
