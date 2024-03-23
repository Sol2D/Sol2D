// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
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
#include <spdlog/spdlog.h>
#include <filesystem>
#include <memory>

namespace Sol2D {

class Workspace final
{
    S2_DISABLE_COPY_AND_MOVE(Workspace)

private:
    explicit Workspace(const std::filesystem::path & _path);

public:
    static std::unique_ptr<Workspace> load(const std::filesystem::path & _path);

    const std::string & getApplicationName() const
    {
        return m_application_name;
    }

    const std::filesystem::path & getMainScriptPath() const
    {
        return m_main_script_path;
    }

    uint16_t getFrameRate() const
    {
        return m_frame_rate;
    }

    std::filesystem::path toAbsolutePath(const std::filesystem::path & _path) const
    {
        return _path.is_absolute() ? _path : m_path / _path;
    }

    spdlog::logger & getMainLogger() const
    {
        return *m_main_logger_ptr;
    }

    spdlog::logger & getLuaLogger() const
    {
        return *m_lua_logger_ptr;
    }

public:
    static const char * s_default_dirname;

private:
    const std::filesystem::path m_path;
    static const char * s_manifest_filename;
    std::string m_application_name;
    std::filesystem::path m_main_script_path;
    uint16_t m_frame_rate;
    std::shared_ptr<spdlog::logger> m_main_logger_ptr;
    std::shared_ptr<spdlog::logger> m_lua_logger_ptr;
};

} // namespace Sol2D
