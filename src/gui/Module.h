#pragma once

#include "gui/Workbench.h"

namespace cartan::gui {

using ModuleInstaller = void (*)(Workbench &workbench);

bool registerModule(int order, ModuleInstaller install);
void installModules(Workbench &workbench);

} // namespace cartan::gui
