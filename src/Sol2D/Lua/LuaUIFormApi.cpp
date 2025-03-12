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

#include <Sol2D/Lua/LuaUIFormApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<UI> & _ui, uint32_t _form_id) :
        ui(_ui),
        form_id(_form_id)
    {
    }

    UIForm * getForm(lua_State * _lua) const
    {
        std::shared_ptr<UI> ui_ptr = ui.lock();
        if(!ui_ptr)
            luaL_error(_lua, LuaMessage::ui_is_destroyed);
        return ui_ptr->getForm(form_id);
    }

    std::weak_ptr<UI> ui;
    uint32_t form_id;
};

using UserData = LuaUserData<Self, LuaTypeName::ui_form>;

// 1 self
// 2 text
int luaApi_AddLabel(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    self->getForm(_lua)->addLabel(argToStringOrError(_lua, 2));
    return 0;
}

} // namespace

void Sol2D::Lua::pushUIFormApi(lua_State * _lua, std::shared_ptr<UI> _ui, const UIForm & _form)
{
    UserData::pushUserData(_lua, _ui, _form.getId());
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            {"__gc",     UserData::luaGC},
            {"addLabel", luaApi_AddLabel},
            {nullptr,    nullptr        }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

UIForm * Sol2D::Lua::tryGetUIForm(lua_State * _lua, int _idx)
{
    if(const Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getForm(_lua);
    return nullptr;
}
