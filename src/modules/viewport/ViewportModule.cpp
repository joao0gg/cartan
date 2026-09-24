#include "modules/viewport/FpsOverlay.h"
#include "modules/viewport/Viewport.h"
#include "shell/Module.h"

namespace {

void install(cartan::shell::Workbench &workbench) {
  auto *viewport = new cartan::viewport::Viewport(workbench.scene());
  new cartan::viewport::FpsOverlay(viewport);

  workbench.setCentralWidget(viewport);
}

const bool registered = cartan::shell::registerModule(20, install);

} // namespace
