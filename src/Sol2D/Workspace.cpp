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

Workspace::Workspace() :
    m_frame_rate(60)
{
    m_main_logger_ptr = spdlog::stdout_logger_mt("engine");
    m_main_logger_ptr->set_level(spdlog::level::off);
    m_lua_logger_ptr = spdlog::stdout_logger_mt("application");
    m_main_logger_ptr->set_level(spdlog::level::off);
}

std::unique_ptr<Workspace> Workspace::load(const std::filesystem::path & _config_path)
{
    XMLDocument doc;
    if(doc.LoadFile(_config_path.c_str()) != XML_SUCCESS)
        return nullptr;
    const XMLElement * xroot = doc.RootElement();
    std::unique_ptr<Workspace> workspace(new Workspace());
    fs::path base_dir = _config_path.parent_path();
    if(const char * root_dir = xroot->Attribute("directory"))
    {
        fs::path root_dir_path(root_dir);
        if(root_dir_path.is_absolute())
            base_dir = root_dir_path;
        else
            base_dir /= root_dir_path;
    }
    workspace->m_scripts_directory = base_dir;
    workspace->m_resources_directory = base_dir;
    if(const XMLElement * xengine = xroot->FirstChildElement("engine"))
    {
        if(const XMLElement * xgraphics = xengine->FirstChildElement("graphics"))
        {
            if(uint32_t frame_rate = xgraphics->UnsignedAttribute("fps", 0))
            {
                if(frame_rate > 0 && frame_rate < UINT16_MAX)
                    workspace->m_frame_rate = static_cast<uint16_t>(frame_rate);
            }
        }
        if(const XMLElement * xlogging = xengine->FirstChildElement("logging"))
        {
            if(const char * log_level = xlogging->Attribute("level"))
                workspace->m_main_logger_ptr->set_level(spdlog::level::from_str(log_level));
        }
    }
    if(const XMLElement * xapp = xroot->FirstChildElement("application"))
    {
        if(const char * app_name = xapp->Attribute("name"))
            workspace->m_application_name = app_name;
        if(const XMLElement * xscripts = xapp->FirstChildElement("scripts"))
        {
            if(const char * entry = xscripts->Attribute("entry"))
            {
                fs::path entry_path(entry);
                if(entry_path.is_absolute())
                    workspace->m_main_script_path = entry_path;
                else
                    workspace->m_main_script_path = base_dir / entry_path;
                workspace->m_scripts_directory = workspace->m_main_script_path.parent_path();
            }
        }
        if(const XMLElement * xresources = xapp->FirstChildElement("resources"))
        {
            if(const char * directory = xresources->Attribute("directory"))
            {
                fs::path directory_path(directory);
                if(directory_path.is_absolute())
                    workspace->m_resources_directory = directory_path;
                else
                    workspace->m_resources_directory /= directory_path;
            }
        }
        if(const XMLElement * xlogging = xapp->FirstChildElement("logging"))
        {
            if(const char * log_level = xlogging->Attribute("level"))
                workspace->m_lua_logger_ptr->set_level(spdlog::level::from_str(log_level));
        }
    }
    return workspace;
}
