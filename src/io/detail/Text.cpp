// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#include "io/detail/Text.h"

#include <charconv>
#include <stdexcept>
#include <system_error>

namespace cartan::io::detail {

namespace {

constexpr std::string_view whitespace = " \t\r\n\v\f";

std::string_view dropPlus(std::string_view token) {
  if (token.size() > 1 && token.front() == '+') {
    token.remove_prefix(1);
  }

  return token;
}

} // namespace

bool LineReader::next(std::string_view &line) {
  if (m_offset >= m_text.size()) {
    return false;
  }

  std::size_t end = m_text.find('\n', m_offset);

  if (end == std::string_view::npos) {
    end = m_text.size();
  }

  line = m_text.substr(m_offset, end - m_offset);

  if (!line.empty() && line.back() == '\r') {
    line.remove_suffix(1);
  }

  m_offset = end == m_text.size() ? end : end + 1;
  ++m_lineNumber;

  return true;
}

bool Tokenizer::next(std::string_view &token) {
  const std::size_t begin = m_text.find_first_not_of(whitespace, m_offset);

  if (begin == std::string_view::npos) {
    m_offset = m_text.size();

    return false;
  }

  std::size_t end = m_text.find_first_of(whitespace, begin);

  if (end == std::string_view::npos) {
    end = m_text.size();
  }

  token = m_text.substr(begin, end - begin);
  m_offset = end;

  return true;
}

std::string_view trim(std::string_view text) {
  const std::size_t begin = text.find_first_not_of(whitespace);

  if (begin == std::string_view::npos) {
    return {};
  }

  const std::size_t end = text.find_last_not_of(whitespace);

  return text.substr(begin, end - begin + 1);
}

std::string_view stripComment(std::string_view line) {
  return line.substr(0, line.find('#'));
}

std::optional<double> parseDouble(std::string_view token) {
  token = dropPlus(token);
  double value = 0.0;
  const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), value);

  if (error != std::errc() || end != token.data() + token.size()) {
    return std::nullopt;
  }

  return value;
}

std::optional<std::int64_t> parseInteger(std::string_view token) {
  token = dropPlus(token);
  std::int64_t value = 0;
  const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), value);

  if (error != std::errc() || end != token.data() + token.size()) {
    return std::nullopt;
  }

  return value;
}

void failAtLine(std::size_t line, const std::string &message) {
  throw std::runtime_error("line " + std::to_string(line) + ": " + message);
}

} // namespace cartan::io::detail
