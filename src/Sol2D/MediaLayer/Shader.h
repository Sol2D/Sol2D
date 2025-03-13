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

#include <Sol2D/ResourceManager.h>
#include <Sol2D/MediaLayer/MediaLayer.h>
#include <memory>
#include <filesystem>

namespace Sol2D {

class ShaderDeleter
{
public:
    explicit ShaderDeleter(SDL_GPUDevice * _device) :
        m_device(_device)
    {
    }

    void operator() (SDL_GPUShader * _shader) noexcept
    {
        if(_shader)
        {
            SDL_ReleaseGPUShader(m_device, _shader);
        }
    }

private:
    SDL_GPUDevice * m_device;
};

using ShaderPtr = std::unique_ptr<SDL_GPUShader, ShaderDeleter>;

inline ShaderPtr makeUniquePtr(SDL_GPUDevice * _device, SDL_GPUShader * _shader)
{
    return ShaderPtr(_shader, ShaderDeleter(_device));
}

struct ShaderOptions
{
    uint8_t num_samplers;
    uint8_t num_uniform_buffers;
};

class ShaderLoader
{
    S2_DISABLE_COPY_AND_MOVE(ShaderLoader)

public:
    ShaderLoader(SDL_GPUDevice * _device, const ResourceManager & _resource_manager) :
        m_device(_device),
        m_resource_manager(_resource_manager)
    {
    }

    ShaderPtr loadStandard(
        SDL_GPUShaderStage _stage,
        SDL_GPUShaderFormat _format,
        const std::string & _name,
        const ShaderOptions & _options = {}
    );

    ShaderPtr loadFromFile(
        SDL_GPUShaderStage _stage,
        SDL_GPUShaderFormat _format,
        const std::filesystem::path & _path,
        const ShaderOptions & _options = {}
    );

private:
    SDL_GPUDevice * m_device;
    const ResourceManager & m_resource_manager;
};

} // namespace Sol2D
