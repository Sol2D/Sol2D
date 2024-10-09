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

#include <Sol2D/Lua/LuaLibrary.h>
#include <Sol2D/Lua/LuaWindowApi.h>
#include <Sol2D/Lua/LuaStoreManagerApi.h>
#include <Sol2D/Lua/LuaHeartbeatApi.h>
#include <Sol2D/Lua/LuaScancodeApi.h>
#include <Sol2D/Lua/LuaBodyTypeApi.h>
#include <Sol2D/Lua/LuaBodyShapeTypeApi.h>
#include <Sol2D/Lua/LuaKeyboardApi.h>
#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/LuaFragmentApi.h>
#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/LuaWidgetApi.h>
#include <Sol2D/Lua/LuaTextAlignmentApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaScript.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <sstream>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_key_path[] = "path";

bool addPackagePath(lua_State * _lua, const std::filesystem::path & _path)
{
    if(_path.empty())
    {
        return true;
    }

    bool result = false;
    if(lua_getglobal(_lua, "package") == LUA_TTABLE)
    {
        LuaTable table(_lua, -1);
        std::string search_paths;
        if(table.tryGetString(gc_key_path, search_paths))
        {
            std::stringstream search_paths_stream(search_paths);
            search_paths_stream << search_paths <<
                LUA_PATH_SEP << (_path / LUA_PATH_MARK ".lua").string() <<
                LUA_PATH_SEP << (_path / LUA_PATH_MARK / "init.lua").string();
            table.setStringValue(gc_key_path, search_paths_stream.str().c_str());
            result = true;
        }
    }
    lua_pop(_lua, 1);
    return result;
}

} // namespace name

LuaLibrary::LuaLibrary(
    const Workspace & _workspace,
    StoreManager & _store_manager,
    Window & _window,
    SDL_Renderer & _renderer
) :
    mp_lua(luaL_newstate()),
    mr_workspace(_workspace)
{
    luaL_openlibs(mp_lua);
    if(!addPackagePath(mp_lua, _workspace.getScriptsRootPath()))
    {
        _workspace.getMainLogger().warn("Unable to add Lua package paths");
    }
    lua_newuserdata(mp_lua, 1);
    if(pushMetatable(mp_lua, LuaTypeName::lib) == MetatablePushResult::Created)
    {
        pushWindowApi(mp_lua, _window);
        lua_setfield(mp_lua, -2, "window");
        pushHeartbeatApi(mp_lua);
        lua_setfield(mp_lua, -2, "heartbeat");
        pushKeyboardApiOntoStack(mp_lua);
        lua_setfield(mp_lua, -2, "keyboard");
        pushStoreManagerApi(mp_lua, _workspace, _renderer, _store_manager);
        lua_setfield(mp_lua, -2, "stores");
        pushScancodeEnum(mp_lua);
        lua_setfield(mp_lua, -2, "Scancode");
        pushBodyTypeEnum(mp_lua);
        lua_setfield(mp_lua, -2, "BodyType");
        pushBodyShapeTypeEnum(mp_lua);
        lua_setfield(mp_lua, -2, "BodyShapeType");
        pushTileMapObjectTypeEnum(mp_lua);
        lua_setfield(mp_lua, -2, "TileMapObjectType");
        pushDimensionUnitEnum(mp_lua);
        lua_setfield(mp_lua, -2, "DimensionUnit");
        pushWidgetStateEnum(mp_lua);
        lua_setfield(mp_lua, -2, "WidgetState");
        pushVerticalTextAlignmentEmum(mp_lua);
        lua_setfield(mp_lua, -2, "VerticalTextAlignment");
        pushHorizontalTextAlignmentEmum(mp_lua);
        lua_setfield(mp_lua, -2, "HorizontalTextAlignment");
    }
    lua_setmetatable(mp_lua, -2);
    lua_setglobal(mp_lua, "sol");
}

LuaLibrary::~LuaLibrary()
{
    lua_close(mp_lua);
}

void LuaLibrary::executeMainScript()
{
    executeScript(mp_lua, mr_workspace, mr_workspace.getMainScriptPath());
}

void LuaLibrary::step(const RenderState & _state)
{
    S2_UNUSED(_state)

    doHeartbeat(mp_lua, mr_workspace);
}
