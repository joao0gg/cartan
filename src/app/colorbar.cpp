#include "app/colorbar.h"

#include <algorithm>

#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>

#include "render/colormap.h"
#include "render/scene/scene.h"
#include "render/theme.h"

namespace cartan::app {

namespace {

constexpr int kMargin = 16;
constexpr int kLabelGap = 6;
constexpr int kSpacing = 24;
constexpr int kGradientStops = 32;
constexpr int kLabelCount = 5;

QColor textColor(const render::Theme &theme) {
  const glm::vec3 &c = theme.background;
  const float luminance = 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;

  return luminance > 0.5f ? Qt::black : Qt::white;
}

float labelFraction(int i) {
  return float(i) / float(kLabelCount - 1);
}

QString labelText(float min, float max, int i) {
  return QString::number(max - (max - min) * labelFraction(i), 'g', 4);
}

QLinearGradient gradient(const QRect &bar, render::Colormap colormap) {
  QLinearGradient result(bar.bottomLeft(), bar.topLeft());

  for (int i = 0; i < kGradientStops; ++i) {
    const float t = float(i) / float(kGradientStops - 1);
    const glm::vec3 color = render::sampleColormap(colormap, t);

    result.setColorAt(t, QColor::fromRgbF(color.r, color.g, color.b));
  }

  return result;
}

int drawColorBar(QPainter &painter, const QRect &area, int top,
                 const render::ScalarQuantity &quantity) {
  const render::ColorBarOptions &style = quantity.options.colorBar;
  const QFontMetrics metrics = painter.fontMetrics();
  const auto [min, max] = quantity.range();
  const QString title = QString::fromStdString(quantity.name());

  int labelWidth = 0;

  for (int i = 0; i < kLabelCount; ++i) {
    labelWidth = std::max(labelWidth, metrics.horizontalAdvance(labelText(min, max, i)));
  }

  const int width =
      std::max(style.width + kLabelGap + labelWidth, metrics.horizontalAdvance(title));
  const int left = area.right() - kMargin - width;
  const QRect bar(left, top + metrics.height() * 3 / 2 + kLabelGap, style.width, style.height);

  if (bar.bottom() > area.bottom() - kMargin) {
    return -1;
  }

  painter.drawText(left, top + metrics.ascent(), title);

  painter.fillRect(bar, gradient(bar, quantity.options.colormap));
  painter.drawRect(bar.adjusted(0, 0, -1, -1));

  for (int i = 0; i < kLabelCount; ++i) {
    const int y = bar.top() + int(labelFraction(i) * float(bar.height() - 1));

    painter.drawLine(bar.right(), y, bar.right() + kLabelGap / 2, y);
    painter.drawText(bar.right() + kLabelGap, y + metrics.ascent() / 2 - 1, labelText(min, max, i));
  }

  return bar.bottom() + metrics.height() / 2 - top;
}

} // namespace

void drawColorBars(QPainter &painter, const QRect &area, const render::Scene &scene,
                   const render::Theme &theme) {
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setPen(textColor(theme));

  int top = area.top() + kMargin;

  for (const auto &structure : scene.structures()) {
    const auto *mesh = dynamic_cast<const render::SurfaceMesh *>(structure.get());

    if (mesh == nullptr || !mesh->visible || mesh->activeQuantity() == nullptr ||
        !mesh->activeQuantity()->options.colorBar.visible) {
      continue;
    }

    const int height = drawColorBar(painter, area, top, *mesh->activeQuantity());

    if (height < 0) {
      break;
    }

    top += height + kSpacing;
  }
}

} // namespace cartan::app