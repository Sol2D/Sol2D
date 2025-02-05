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

#include <Sol2D/ResourceManager.h>
#include <Sol2D/Exception.h>
#include <Sol2D/MediaLayer/MediaLayer.h>
#include <fstream>

using namespace Sol2D;

ResourceManager::ResourceManager() :
    m_root_path(SDL_GetBasePath())
{
}

std::vector<uint8_t> ResourceManager::loadFileContent(const std::filesystem::path _resource_path) const
{
    std::ifstream file(_resource_path.is_absolute() ? _resource_path : m_root_path / _resource_path);
    if(!file.is_open())
        throw IOException(std::format("Unable to open file \"{}\" to read", _resource_path.string()));
    file.seekg(0, std::ios_base::end);
    std::vector<uint8_t> content;
    content.reserve(file.tellg());
    file.seekg(0, std::ios_base::beg);
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return content;
}
