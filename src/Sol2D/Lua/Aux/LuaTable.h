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

#pragma once

#include <Sol2D/Utils/Variant.h>
#include <lua.hpp>
#include <string>
#include <optional>

namespace Sol2D::Lua {

template<const char * field_key, typename Type>
struct LuaTableField
{
    static constexpr std::string key = field_key;
    using ValueType = Type;
    std::optional<Type> value;
};

template<typename... Fields>
class LuaTable
{
public:
    using IsLuaTable = void;

    LuaTable(lua_State * _lua, int _idx);

    template <const char * field_key>
    auto tryGetValue() const;

    template <const char * field_key, typename Default>
    auto getValueOr(Default _default_value) const;

private:
    template<typename... VariantTypes>
    class VariantReader
    {
    public:
        explicit VariantReader(LuaTable<Fields...> & _table) :
            m_table(_table)
        {
        }

        std::optional<std::variant<VariantTypes...>> tryReadValue()
        {
            return tryReadType<VariantTypes...>();
        }

    private:
        template<typename Arg, typename... Rest>
        std::optional<std::variant<VariantTypes...>> tryReadType()
        {
            if(auto value = m_table.tryReadValueFromTop<Arg>())
                return value.value();
            if constexpr(sizeof...(Rest) > 0)
                return tryReadType<Rest...>();
            return std::nullopt;
        }

    private:
        LuaTable<Fields...> & m_table;
    };

    template<typename First, typename... Rest>
    void loadValues(int _idx);

    template<const char * field_key, typename First, typename... Rest>
    auto getValue() const;

    template<typename T>
    std::optional<T> tryReadField(int _idx, const std::string & _key);

    template<typename T>
    std::optional<T> tryReadValueFromTop();

    template<typename... VariantTypes>
    std::optional<std::variant<VariantTypes...>> tryReadVariantFromTop(const std::variant<VariantTypes...> &)
    {
        VariantReader<VariantTypes...> reader(*this);
        return reader.tryReadValue();
    }

private:
    lua_State * m_lua;
    std::tuple<Fields...> m_fields;
};

template<typename... Fields>
inline LuaTable<Fields...>::LuaTable(lua_State * _lua, int _idx) :
    m_lua(_lua)
{
    loadValues<Fields...>(_idx);
}

template<typename... Fields>
template<const char * field_key>
inline auto LuaTable<Fields...>::tryGetValue() const
{
    return getValue<field_key, Fields...>();
}

template<typename... Fields>
template<const char * field_key, typename Default>
inline auto LuaTable<Fields...>::getValueOr(Default _default_value) const
{
    return tryGetValue<field_key>().value_or(_default_value);
}

template<typename... Fields>
template<typename First, typename... Rest>
inline void LuaTable<Fields...>::loadValues(int _idx)
{
    std::get<First>(m_fields).value = tryReadField<typename First::ValueType>(_idx, First::key);
    if constexpr(sizeof...(Rest) > 0)
        loadValues<Rest...>(_idx);
}

template<typename... Fields>
template<typename T>
inline std::optional<T> LuaTable<Fields...>::tryReadField(int _idx, const std::string & _key)
{
    lua_getfield(m_lua, _idx, _key.c_str());
    auto result = tryReadValueFromTop<T>();
    lua_pop(m_lua, 1);
    return result;
}

template<typename... Fields>
template<typename T>
std::optional<T> LuaTable<Fields...>::tryReadValueFromTop()
{
    int type = lua_type(m_lua, -1);
    if constexpr(std::is_same_v<T, bool>)
    {
        if(type == LUA_TBOOLEAN)
            return lua_toboolean(m_lua, -1) != 0;
        if(type == LUA_TNUMBER)
            return lua_tonumber(m_lua, -1) != 0.0;
    }
    else if constexpr(std::is_integral_v<T> || std::is_enum_v<T>)
    {
        if(type == LUA_TNUMBER)
            return static_cast<T>(lua_tointeger(m_lua, -1));
    }
    else if constexpr(std::is_floating_point_v<T>)
    {
        if(type == LUA_TNUMBER)
            return static_cast<T>(lua_tonumber(m_lua, -1));
    }
    else if constexpr(std::is_same_v<T, std::string>)
    {
        if(type == LUA_TSTRING)
        {
            const char * value = lua_tostring(m_lua, -1);
            return value ? std::optional<std::string>(std::string(value)) : std::nullopt;
        }
    }
    else if constexpr(Sol2D::Utils::is_variant_v<T>)
    {
        return tryReadVariantFromTop(T());
    }
    else if constexpr(requires { typename T::IsLuaTable; })
    {
        if(type == LUA_TTABLE)
            return T(m_lua, -1);
    }
    return std::nullopt;
}

template<typename... Fields>
template<const char * field_key, typename First, typename... Rest>
inline auto LuaTable<Fields...>::getValue() const
{
    if constexpr(First::key == field_key)
    {
        return std::get<First>(m_fields).value;
    }
    else
    {
        if constexpr(sizeof...(Rest) > 0)
            return getValue<field_key, Rest...>();
        else
        return std::nullopt;
    }
}

} // namespace Sol2D::Lua

