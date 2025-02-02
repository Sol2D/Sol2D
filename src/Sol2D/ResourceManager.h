// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <Sol2D/Def.h>
#include <filesystem>
#include <vector>

namespace Sol2D {

class ResourceManager final
{
    S2_DISABLE_COPY_AND_MOVE(ResourceManager)

public:
    explicit ResourceManager();
    [[nodiscard]] std::vector<uint8_t> loadFileContent(const std::filesystem::path _resource_path) const;

private:
    const std::filesystem::path m_root_path;
};

} // namespace Sol2D
