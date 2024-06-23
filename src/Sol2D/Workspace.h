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
    Workspace();

public:
    static std::unique_ptr<Workspace> load(const std::filesystem::path & _config_path);

    const std::string & getApplicationName() const
    {
        return m_application_name;
    }

    const std::filesystem::path & getMainScriptPath() const
    {
        return m_main_script_path;
    }

    const std::filesystem::path & getScriptsRootPath() const
    {
        return m_scripts_directory;
    }

    uint16_t getFrameRate() const
    {
        return m_frame_rate;
    }

    std::filesystem::path getResourceFullPath(const std::filesystem::path & _resource_path) const
    {
        return getFullPath(m_resources_directory, _resource_path);
    }

    std::filesystem::path getScriptFullPath(const std::filesystem::path & _script_path) const
    {
        return getFullPath(m_scripts_directory, _script_path);
    }

    spdlog::logger & getMainLogger() const
    {
        return *m_main_logger_ptr;
    }

    spdlog::logger & getLuaLogger() const
    {
        return *m_lua_logger_ptr;
    }

private:
    static std::filesystem::path getFullPath(const std::filesystem::path & _dir, const std::filesystem::path & _path)
    {
        if(_path.is_absolute() || _dir.empty())
            return _path;
        return _dir / _path;
    }

private:
    std::string m_application_name;
    std::filesystem::path m_main_script_path;
    std::filesystem::path m_scripts_directory;
    std::filesystem::path m_resources_directory;
    uint16_t m_frame_rate;
    std::shared_ptr<spdlog::logger> m_main_logger_ptr;
    std::shared_ptr<spdlog::logger> m_lua_logger_ptr;
};

} // namespace Sol2D
