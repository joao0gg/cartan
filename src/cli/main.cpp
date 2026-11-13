#include <iostream>
#include <string_view>

#include "engine/version.h"

int main(int argc, char **argv) {
  if (argc == 2 && std::string_view(argv[1]) == "--version") {
    std::cout << "cartan " << cartan::engine::version() << '\n';
    return 0;
  }

  std::cerr << "usage: cartan-cli --version\n";

  return 1;
}
