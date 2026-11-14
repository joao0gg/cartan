#include "app/outline.h"

namespace cartan::app {

Outline::Outline(Context & /*context*/, QWidget *parent) : QTreeWidget(parent) {
  setHeaderHidden(true);
}

} // namespace cartan::app
