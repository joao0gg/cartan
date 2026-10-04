// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "modules/viewport/FpsOverlay.h"

#include <QTimer>

#include "modules/viewport/Viewport.h"

namespace cartan::viewport {

namespace {

constexpr int kFpsOverlayInterval = 1000; // ms
constexpr int kFpsOverlayMargin = 8;

} // namespace

FpsOverlay::FpsOverlay(Viewport *viewport) : QLabel("0 FPS", viewport) {
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setStyleSheet("color: white;");
  move(kFpsOverlayMargin, kFpsOverlayMargin);

  connect(viewport, &Viewport::frameRendered, this, [this]() {
    ++m_frameCount;
  });

  auto *timer = new QTimer(this);

  connect(timer, &QTimer::timeout, this, [this]() {
    setText(QString("%1 FPS").arg(m_frameCount * 1000 / kFpsOverlayInterval));
    adjustSize();
    m_frameCount = 0;
  });

  timer->start(kFpsOverlayInterval);
}

} // namespace cartan::viewport
