#include "render/scene/structure.h"

#include <atomic>
#include <utility>

namespace cartan::render {

namespace {

std::atomic<std::uint64_t> nextId{1};

} // namespace

Structure::Structure(std::string name) : m_name(std::move(name)), m_id(nextId++) {
}

const std::string &Structure::name() const {
  return m_name;
}

std::uint64_t Structure::id() const {
  return m_id;
}

} // namespace cartan::render
