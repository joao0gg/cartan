// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

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
