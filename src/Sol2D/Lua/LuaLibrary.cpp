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

#include <Sol2D/Lua/LuaLibrary.h>
#include <Sol2D/Lua/LuaWindowApi.h>
#include <Sol2D/Lua/LuaStoreManagerApi.h>
#include <Sol2D/Lua/LuaScancodeApi.h>
#include <Sol2D/Lua/LuaBodyTypeApi.h>
#include <Sol2D/Lua/LuaBodyShapeTypeApi.h>
#include <Sol2D/Lua/LuaKeyboardApi.h>
#include <Sol2D/Lua/LuaMouseApi.h>
#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/LuaLayoutingApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaScript.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>
#include <sstream>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char g_key_path[] = "path";

bool addPackagePath(lua_State * _lua, const std::filesystem::path & _path)
{
    if(_path.empty())
    {
        return true;
    }

    bool result = false;
    if(lua_getglobal(_lua, "package") == LUA_TTABLE)
    {
        LuaTableApi table(_lua, -1);
        std::string search_paths;
        if(table.tryGetString(g_key_path, search_paths))
        {
            std::stringstream search_paths_stream(search_paths);
            search_paths_stream << search_paths << LUA_PATH_SEP << (_path / LUA_PATH_MARK ".lua").string()
                                << LUA_PATH_SEP << (_path / LUA_PATH_MARK / "init.lua").string();
            table.setStringValue(g_key_path, search_paths_stream.str().c_str());
            result = true;
        }
    }
    lua_pop(_lua, 1);
    return result;
}

} // namespace

LuaLibrary::LuaLibrary(
    const Workspace & _workspace,
    StoreManager & _store_manager,
    Window & _window,
    Renderer & _renderer,
    MIX_Mixer & _mixer
) :
    m_lua(luaL_newstate()),
    m_workspace(_workspace)
{
    luaL_openlibs(m_lua);
    if(!addPackagePath(m_lua, _workspace.getScriptsRootPath()))
    {
        _workspace.getMainLogger().warn("Unable to add Lua package paths");
    }
    lua_newuserdata(m_lua, 1);
    if(pushMetatable(m_lua, LuaTypeName::lib) == MetatablePushResult::Created)
    {
        pushWindowApi(m_lua, _window);
        lua_setfield(m_lua, -2, "window");
        pushKeyboardApi(m_lua);
        lua_setfield(m_lua, -2, "keyboard");
        pushMouseApi(m_lua);
        lua_setfield(m_lua, -2, "mouse");
        pushStoreManagerApi(m_lua, _workspace, _renderer, _mixer, _store_manager);
        lua_setfield(m_lua, -2, "stores");
        pushScancodeEnum(m_lua);
        lua_setfield(m_lua, -2, "Scancode");
        pushBodyTypeEnum(m_lua);
        lua_setfield(m_lua, -2, "BodyType");
        pushBodyShapeTypeEnum(m_lua);
        lua_setfield(m_lua, -2, "BodyShapeType");
        pushTileMapObjectTypeEnum(m_lua);
        lua_setfield(m_lua, -2, "TileMapObjectType");
        // pushWidgetStateEnum(m_lua); // TODO: Layouting: restore
        // lua_setfield(m_lua, -2, "WidgetState");
        pushContentAlignmentEnum(m_lua);
        lua_setfield(m_lua, -2, "ContentAlignment");
        pushContentJustificationEnum(m_lua);
        lua_setfield(m_lua, -2, "ContentJustification");
        pushItemAlignmentEnum(m_lua);
        lua_setfield(m_lua, -2, "ItemAlignment");
        pushDisplayModeEnum(m_lua);
        lua_setfield(m_lua, -2, "DisplayMode");
        pushFlexDirectionEnum(m_lua);
        lua_setfield(m_lua, -2, "FlexDirection");
        pushFlexWrapEnum(m_lua);
        lua_setfield(m_lua, -2, "FlexWrap");
        pushEdgeEnum(m_lua);
        lua_setfield(m_lua, -2, "Edge");
        pushGapGutterEnum(m_lua);
        lua_setfield(m_lua, -2, "GapGutter");
        pushPositionTypeEnum(m_lua);
        lua_setfield(m_lua, -2, "PositionType");
        pushPositionUnitEnum(m_lua);
        lua_setfield(m_lua, -2, "PositionUnit");
        pushDimensionUnitEnum(m_lua);
        lua_setfield(m_lua, -2, "DimensionUnit");
        pushDimensionLimitUnitEnum(m_lua);
        lua_setfield(m_lua, -2, "DimensionLimitUnit");
    }
    lua_setmetatable(m_lua, -2);
    lua_setglobal(m_lua, "sol");
}

LuaLibrary::~LuaLibrary()
{
    lua_close(m_lua);
}

void LuaLibrary::executeMainScript()
{
    executeScript(m_lua, m_workspace, m_workspace.getMainScriptPath());
}
