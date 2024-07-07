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

#include <Sol2D/Forms/Button.h>
#include <Sol2D/Lua/Aux/LuaForward.h>
#include <Sol2D/Workspace.h>

namespace Sol2D::Lua {

void pushWidgetStateEnum(lua_State * _lua);

void pushLabelApi(lua_State * _lua, Forms::Label & _label);

void pushButtonApi(lua_State * _lua, Forms::Button & _button, const Workspace & _workspace);

} // namespace Sol2D::Lua
