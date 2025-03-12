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

#include <Sol2D/Lua/LuaUIApi.h>
#include <Sol2D/Lua/LuaUIFormApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
public:
    explicit Self(std::shared_ptr<UI> & _ui) :
        ui(_ui)
    {
    }

    std::shared_ptr<UI> getUI(lua_State * _lua) const
    {
        std::shared_ptr<UI> ptr = ui.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::ui_is_destroyed);
        return ptr;
    }

    std::weak_ptr<UI> ui;
};

using UserData = LuaUserData<Self, LuaTypeName::ui>;

// 1 self
// 2 title
int luaApi_AddForm(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * title = argToStringOrError(_lua, 2);
    std::shared_ptr<UI> ui = self->getUI(_lua);
    pushUIFormApi(_lua, ui, ui->addForm(title));
    return 1;
}

} // namespace

void Sol2D::Lua::pushUIApi(lua_State * _lua, std::shared_ptr<UI> _ui)
{
    UserData::pushUserData(_lua, _ui);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "addForm", luaApi_AddForm },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<UI> Sol2D::Lua::tryGetUI(lua_State * _lua, int _idx)
{
    if(const Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getUI(_lua);
    return nullptr;
}
