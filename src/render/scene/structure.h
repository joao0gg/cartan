#pragma once

#include <cstdint>
#include <string>

#include "render/scene/bounds.h"

namespace cartan::render {

class Structure {
public:
  explicit Structure(std::string name);
  virtual ~Structure() = default;

  Structure(const Structure &) = delete;
  Structure &operator=(const Structure &) = delete;

  const std::string &name() const;
  std::uint64_t id() const;

  virtual Bounds bounds() const = 0;

  bool visible = true;

private:
  std::string m_name;
  std::uint64_t m_id;
};

} // namespace cartan::render
