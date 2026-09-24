#include "shell/Module.h"

#include <algorithm>
#include <vector>

namespace cartan::shell {

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

} // namespace cartan::shell
