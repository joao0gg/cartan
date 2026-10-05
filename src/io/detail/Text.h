// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cartan::io::detail {

class LineReader {
public:
  explicit LineReader(std::string_view text) : m_text(text) {
  }

  bool next(std::string_view &line);

  std::size_t lineNumber() const {
    return m_lineNumber;
  }

  // byte offset just past the terminator of the curr line
  std::size_t offset() const {
    return m_offset;
  }

private:
  std::string_view m_text;
  std::size_t m_offset = 0;
  std::size_t m_lineNumber = 0;
};

class Tokenizer {
public:
  explicit Tokenizer(std::string_view text) : m_text(text) {
  }

  bool next(std::string_view &token);

private:
  std::string_view m_text;
  std::size_t m_offset = 0;
};

std::string_view trim(std::string_view text);

// drops everything after first '#'
std::string_view stripComment(std::string_view line);

std::optional<double> parseDouble(std::string_view token);
std::optional<std::int64_t> parseInteger(std::string_view token);

[[noreturn]] void failAtLine(std::size_t line, const std::string &message);

} // namespace cartan::io::detail
