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
