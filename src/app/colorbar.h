#pragma once

#include <QRect>

class QPainter;

namespace cartan::render {
class Scene;
struct Theme;
} // namespace cartan::render

namespace cartan::app {

struct ColorBarStyle {
  int width = 18;
  int height = 200;
};

inline constexpr int kMinColorBarWidth = 4;
inline constexpr int kMaxColorBarWidth = 200;
inline constexpr int kMinColorBarHeight = 40;
inline constexpr int kMaxColorBarHeight = 2000;

void drawColorBars(QPainter &painter, const QRect &area, const render::Scene &scene,
                   const render::Theme &theme, const ColorBarStyle &style);

} // namespace cartan::app
