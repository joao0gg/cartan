#include "modules/viewport/FpsOverlay.h"

#include <QTimer>

#include "modules/viewport/Viewport.h"

namespace cartan::viewport {

FpsOverlay::FpsOverlay(Viewport *viewport) : QLabel("0 FPS", viewport) {
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setStyleSheet("color: white;");
  move(FPS_OVERLAY_MARGIN, FPS_OVERLAY_MARGIN);

  connect(viewport, &Viewport::frameRendered, this, [this]() {
    ++m_frameCount;
  });

  auto *timer = new QTimer(this);

  connect(timer, &QTimer::timeout, this, [this]() {
    setText(QString("%1 FPS").arg(m_frameCount * 1000 / FPS_OVERLAY_INTERVAL));
    adjustSize();
    m_frameCount = 0;
  });

  timer->start(FPS_OVERLAY_INTERVAL);
}

} // namespace cartan::viewport
