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

#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/LuaStrings.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<BodyDefinition> & _definition) :
        definition(_definition)
    {
    }

    std::weak_ptr<BodyDefinition> definition;
};

using UserData = LuaUserData<Self, LuaTypeName::body_prototype>;

} // namespace

void Sol2D::Lua::pushBodyPrototype(lua_State * _lua, std::shared_ptr<BodyDefinition> _definition)
{
    UserData::pushUserData(_lua, _definition);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::tryGetBodyPrototype(lua_State * _lua, int _idx, LuaBodyPrototype & _prototype)
{
    Self * self = UserData::tryGetUserData(_lua, _idx);
    if(!self) return false;
    if(std::shared_ptr<BodyDefinition> definition = self->definition.lock())
    {
        _prototype.definition = definition;
        return true;
    }
    return false;
}
