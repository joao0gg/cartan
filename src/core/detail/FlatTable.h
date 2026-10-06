// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "core/Types.h"

namespace cartan::core::detail {

template <std::size_t N> class FlatTable {
public:
  using Key = std::array<Index, N>;

  explicit FlatTable(std::size_t expected) {
    allocate(std::bit_ceil(std::max<std::size_t>(16, expected * 2)));
  }

  std::pair<Index, bool> insert(const Key &key, Index value) {
    if ((m_size + 1) * 2 > m_keys.size()) {
      grow();
    }

    for (std::size_t slot = hash(key) & m_mask;; slot = (slot + 1) & m_mask) {
      if (m_keys[slot][0] < 0) {
        m_keys[slot] = key;
        m_values[slot] = value;
        ++m_size;

        return {value, true};
      }

      if (m_keys[slot] == key) {
        return {m_values[slot], false};
      }
    }
  }

private:
  static std::size_t hash(const Key &key) {
    std::uint64_t h = 0x9e3779b97f4a7c15ULL;

    for (const Index vertex : key) {
      h ^= static_cast<std::uint32_t>(vertex);
      h *= 0xbf58476d1ce4e5b9ULL;
      h ^= h >> 31;
    }

    return static_cast<std::size_t>(h);
  }

  void allocate(std::size_t capacity) {
    Key empty;
    empty.fill(-1);
    m_keys.assign(capacity, empty);
    m_values.assign(capacity, 0);
    m_mask = capacity - 1;
    m_size = 0;
  }

  void grow() {
    std::vector<Key> keys = std::move(m_keys);
    std::vector<Index> values = std::move(m_values);
    allocate(keys.size() * 2);

    for (std::size_t slot = 0; slot < keys.size(); ++slot) {
      if (keys[slot][0] >= 0) {
        insert(keys[slot], values[slot]);
      }
    }
  }

  std::vector<Key> m_keys;
  std::vector<Index> m_values;
  std::size_t m_mask = 0;
  std::size_t m_size = 0;
};

} // namespace cartan::core::detail
