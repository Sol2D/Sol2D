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

#include <Sol2D/Rendering/Impl/Shader.h>
#include <Sol2D/Rendering/SDLException.h>

using namespace Sol2D;
using namespace Sol2D::Rendering::Impl;

namespace {

const char gc_entry_point[] = "main";

std::string getFileExt(SDL_GPUShaderFormat _format)
{
    switch(_format)
    {
    case SDL_GPU_SHADERFORMAT_SPIRV:
        return "spv";
    case SDL_GPU_SHADERFORMAT_MSL:
    case SDL_GPU_SHADERFORMAT_METALLIB:
        return "msl";
    case SDL_GPU_SHADERFORMAT_DXIL:
    case SDL_GPU_SHADERFORMAT_DXBC:
        return "dxil";
    default:
        throw std::runtime_error("Unsupported shader format");
    }
}

} // namespace

ShaderPtr ShaderLoader::loadStandard(
    SDL_GPUShaderStage _stage,
    SDL_GPUShaderFormat _format,
    const std::string & _name,
    const ShaderOptions & _options)
{
    std::filesystem::path path = std::filesystem::path("Shaders") / std::format("{}.{}", _name, getFileExt(_format));
    return loadFromFile(_stage, _format, path, _options);
}

ShaderPtr ShaderLoader::loadFromFile(
    SDL_GPUShaderStage _stage,
    SDL_GPUShaderFormat _format,
    const std::filesystem::path & _path,
    const ShaderOptions & _options)
{
    SDL_GPUShader * shader = nullptr;
    std::vector<uint8_t> code = mr_resource_manager.loadFileContent(_path);
    SDL_GPUShaderCreateInfo shader_create_info
    {
        .code_size = code.size(),
        .code = code.data(),
        .entrypoint = gc_entry_point,
        .format = _format,
        .stage = _stage,
        .num_samplers = _options.num_samplers,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = _options.num_uniform_buffers,
        .props = 0
    };
    shader = SDL_CreateGPUShader(mp_device, &shader_create_info);
    if(!shader)
    {
        throw SDLException(std::format("Unable to create shader from file \"{}\"", _path.string()));
    }
    return makeUniquePtr(mp_device, shader);
}
