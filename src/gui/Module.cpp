// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "gui/Module.h"

#include <algorithm>
#include <vector>

namespace cartan::gui {

namespace {

struct Entry {
  int order = 0;
  ModuleInstaller install = nullptr;
};

std::vector<Entry> &registry() {
  static std::vector<Entry> entries;

  return entries;
}

} // namespace

bool registerModule(int order, ModuleInstaller install) {
  registry().push_back({order, install});

  return true;
}

void installModules(Workbench &workbench) {
  auto entries = registry();
  std::ranges::stable_sort(entries, {}, &Entry::order);

  for (const auto &entry : entries) {
    entry.install(workbench);
  }
}

} // namespace cartan::gui
