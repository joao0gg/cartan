#pragma once

#include <QLabel>

#define FPS_OVERLAY_INTERVAL 1000 // ms
#define FPS_OVERLAY_MARGIN 8

namespace cartan::viewport {

class Viewport;

class FpsOverlay : public QLabel {
public:
  explicit FpsOverlay(Viewport *viewport);

private:
  int m_frameCount = 0;
};

} // namespace cartan::viewport
