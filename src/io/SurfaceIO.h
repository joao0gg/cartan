// Copyright (C) 2026 João Gabriel Abrahão Franco
// SPDX-License-Identifier: AGPL-3.0-or-later
// Additional terms apply under AGPL-3.0 Section 7(b); see NOTICE.txt.

#pragma once

#include <filesystem>
#include <string>

#include "core/Mesh.h"

namespace cartan::io {

core::Mesh loadSurface(const std::filesystem::path &path);

std::string supportedExtensions();

} // namespace cartan::io