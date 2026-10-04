// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "gui/Module.h"
#include "modules/viewport/FpsOverlay.h"
#include "modules/viewport/Viewport.h"

namespace {

void install(cartan::gui::Workbench &workbench) {
  auto *viewport = new cartan::viewport::Viewport(workbench.scene());
  new cartan::viewport::FpsOverlay(viewport);

  workbench.setCentralWidget(viewport);
}

const bool registered = cartan::gui::registerModule(20, install);

} // namespace
