#pragma once

#include "shell/Workbench.h"

namespace cartan::shell {

using ModuleInstaller = void (*)(Workbench &workbench);

bool registerModule(int order, ModuleInstaller install);
void installModules(Workbench &workbench);

} // namespace cartan::shell
