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

#include <Sol2D/Lua/LuaSceneOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D::World;
using namespace Sol2D::Lua;

bool Sol2D::Lua::tryGetSceneOptions(lua_State * _lua, int _idx, SceneOptions & _options)
{
    LuaTable table(_lua, _idx);
    if(!table.isValid())
        return false;    
    table.tryGetNumber("metersPerPixel", &_options.meters_per_pixel);
    table.tryGetPoint("gravity", _options.gravity);
    return true;
}
