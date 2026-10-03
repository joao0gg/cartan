#pragma once

#include <QLabel>

namespace cartan::viewport {

class Viewport;

class FpsOverlay : public QLabel {
public:
  explicit FpsOverlay(Viewport *viewport);

private:
  int m_frameCount = 0;
};

} // namespace cartan::viewport
