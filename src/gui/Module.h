// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include "gui/Workbench.h"

namespace cartan::gui {

using ModuleInstaller = void (*)(Workbench &workbench);

bool registerModule(int order, ModuleInstaller install);
void installModules(Workbench &workbench);

} // namespace cartan::gui
