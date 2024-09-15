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

#include <Sol2D/Lua/LuaSceneOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/LuaPointApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

bool Sol2D::Lua::tryGetSceneOptions(lua_State * _lua, int _idx, SceneOptions & _options)
{
    if(!lua_istable(_lua, _idx))
        return false;
    LuaTable table(_lua, _idx);

    {
        lua_Number scale_factor;
        if(table.tryGetNumber("scaleFactor", &scale_factor))
            _options.scale_factor = static_cast<float>(scale_factor);
    }

    if(table.tryGetValue("gravity"))
    {
        tryGetPoint(_lua, -1, _options.gravity);
        lua_pop(_lua, 1);
    }

    return true;
}
