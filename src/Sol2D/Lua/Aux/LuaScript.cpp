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

#include <Sol2D/Lua/Aux/LuaScript.h>

void Sol2D::Lua::Aux::executeScript(
    lua_State * _lua,
    const Workspace & _workspace,
    const std::filesystem::path & _path)
{
    std::filesystem::path path = _workspace.getScriptFullPath(_path);
    _workspace.getMainLogger().info("Executing script \"{0}\"", path.c_str());
    if(luaL_loadfile(_lua, path.c_str()) == LUA_OK)
    {
        if(lua_pcall(_lua, 0, LUA_MULTRET, 0) == LUA_OK)
            _workspace.getMainLogger().info("Script \"{0}\" executed successfully", path.c_str());
        else
            _workspace.getMainLogger().error(lua_tostring(_lua, -1));
    }
    else
    {
        _workspace.getMainLogger().error("Unable to load file \"{0}\"", path.c_str());
    }
}

void Sol2D::Lua::Aux::executeScriptWithContext(
    lua_State * _lua,
    const Workspace & _workspace,
    const std::filesystem::path & _path)
{
    const char global_name[] = "self";
    lua_setglobal(_lua, global_name);
    executeScript(_lua, _workspace, _path);
    lua_pushnil(_lua);
    lua_setglobal(_lua, global_name);
}
