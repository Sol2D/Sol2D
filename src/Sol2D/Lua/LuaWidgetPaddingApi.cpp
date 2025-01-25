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

#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/LuaWidgetPaddingApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Forms;
using namespace Sol2D::Lua;

bool Sol2D::Lua::tryGetWidgetPadding(lua_State * _lua, int _idx, Forms::WidgetPadding & _padding)
{
    if(lua_istable(_lua, _idx))
    {
        LuaTable table(_lua, _idx);
        table.tryGetDimension("top", _padding.top);
        table.tryGetDimension("right", _padding.right);
        table.tryGetDimension("bottom", _padding.bottom);
        table.tryGetDimension("left", _padding.left);
        return true;
    }
    else
    {
        std::optional<Dimension<float>> dimension;
        if(tryGetDimension<float>(_lua, _idx, dimension))
        {
            _padding.top = dimension.value();
            _padding.bottom = dimension.value();
            _padding.right = dimension.value();
            _padding.left = dimension.value();
            return true;
        }
    }
    return false;
}
