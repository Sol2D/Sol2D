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

#include <Sol2D/Workspace.h>
#include <tinyxml2.h>
#include <spdlog/sinks/stdout_sinks.h>

using namespace Sol2D;
using namespace tinyxml2;

namespace fs = std::filesystem;

const char * Workspace::s_default_dirname = "workspace";
const char * Workspace::s_manifest_filename = "manifest.xml";

Workspace::Workspace(const std::filesystem::path & _path) :
    m_path(_path),
    m_frame_rate(60)
{
    m_main_logger_ptr = spdlog::stdout_logger_mt("engine");
    m_main_logger_ptr->set_level(spdlog::level::off);
    m_lua_logger_ptr = spdlog::stdout_logger_mt("application");
    m_main_logger_ptr->set_level(spdlog::level::off);
}

std::unique_ptr<Workspace> Workspace::load(const std::filesystem::path & _path)
{
    std::unique_ptr<Workspace> workspace(new Workspace(_path));
    fs::path manifest_path = workspace->m_path / s_manifest_filename;
    XMLDocument doc;
    if(doc.LoadFile(manifest_path.c_str()) != XML_SUCCESS)
    {
        return nullptr;
    }
    const XMLElement * xroot = doc.RootElement();
    if(const XMLElement * xengine = xroot->FirstChildElement("engine"))
    {
        if(uint32_t frame_rate = xengine->UnsignedAttribute("frame_rate", 0))
        {
            if(frame_rate > 0 && frame_rate < UINT16_MAX)
                workspace->m_frame_rate = static_cast<uint16_t>(frame_rate);
        }
        if(const char * log_level = xengine->Attribute("log_level"))
            workspace->m_main_logger_ptr->set_level(spdlog::level::from_str(log_level));
    }
    if(const XMLElement * xapp = xroot->FirstChildElement("application"))
    {
        if(const char * app_name = xapp->Attribute("name"))
            workspace->m_application_name = app_name;
        if(const char * log_level = xapp->Attribute("log_level"))
            workspace->m_lua_logger_ptr->set_level(spdlog::level::from_str(log_level));
        if(const char * script = xapp->Attribute("script"))
            workspace->m_main_script_path = workspace->toAbsolutePath(script);
    }
    return workspace;
}
