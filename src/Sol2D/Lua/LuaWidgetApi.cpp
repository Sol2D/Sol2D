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

#include <Sol2D/Lua/LuaWidgetApi.h>
#include <Sol2D/Lua/LuaFontApi.h>
#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <Sol2D/Lua/LuaTextAlignmentApi.h>
#include <Sol2D/Lua/LuaWidgetPaddingApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>
#include <Sol2D/Object.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;
using namespace Sol2D::Forms;

namespace {

const char gc_message_color_required[] = "color required";
const char gc_message_alignment_required[] = "alignment required";

const char gc_click_companion_key[] = "lua_click_observer";

const uint16_t gc_event_click = 0;

class LuaButtonClickObserver : public ObjectCompanion, public ButtonClickObserver
{
public:
    LuaButtonClickObserver(lua_State * _lua, const Workspace & _workspace) :
        mp_lua(_lua),
        mr_worksapce(_workspace)
    {
    }

    ~LuaButtonClickObserver() override
    {
        LuaCallbackStorage storage(mp_lua);
        storage.destroyCallbacks(this);
    }

    void onClick() override
    {
        LuaCallbackStorage storage(mp_lua);
        storage.execute(mr_worksapce, this, gc_event_click, 0);
    }

private:
    lua_State * mp_lua;
    const Workspace & mr_worksapce;
};

template<typename WidgetT>
struct Self : LuaSelfBase
{
    explicit Self(std::shared_ptr<WidgetT> & _widget) :
        widget(_widget)
    {
    }

    std::shared_ptr<WidgetT> getWidget(lua_State * _lua) const
    {
        std::shared_ptr<WidgetT> ptr = widget.lock();
        if(!ptr)
            luaL_error(_lua, "the widget is destroyed");
        return ptr;
    }

    std::weak_ptr<WidgetT> widget;
};

struct LabelSelf : Self<Label>
{
    explicit LabelSelf(std::shared_ptr<Label> & _label) :
        Self<Label>(_label)
    {
    }
};

using LabelUserData = LuaUserData<LabelSelf, LuaTypeName::label>;

struct ButtonSelf : Self<Button>
{
public:
    ButtonSelf(std::shared_ptr<Button> & _button, const Workspace & _workspace) :
        Self<Button>(_button),
        mr_workspace(_workspace)
    {
    }

    uint32_t subscribeOnClick(lua_State * _lua, int _callback_idx);
    void unsubscribeOnClick(lua_State * _lua, int _subscription_id);

private:
    const Workspace & mr_workspace;
};

uint32_t ButtonSelf::subscribeOnClick(lua_State * _lua, int _callback_idx)
{
    std::shared_ptr<Button> button = getWidget(_lua);
    ObjectCompanion * owner = button->getCompanion(gc_click_companion_key);
    if(owner == nullptr)
    {
        LuaButtonClickObserver * observer = new LuaButtonClickObserver(_lua, mr_workspace);
        button->addCompanion(gc_click_companion_key, std::unique_ptr<ObjectCompanion>(observer));
        button->addObserver(*observer);
        owner = observer;
    }
    return LuaCallbackStorage(_lua).addCallback(owner, gc_event_click, _callback_idx);
}

void ButtonSelf::unsubscribeOnClick(lua_State * _lua, int _subscription_id)
{
    std::shared_ptr<Button> button = getWidget(_lua);
    ObjectCompanion * owner = button->getCompanion(gc_click_companion_key);
    if(owner == nullptr)
        return;
    if(LuaCallbackStorage(_lua).removeCallback(owner, gc_event_click, _subscription_id) == 0)
    {
        button->removeObserver(*static_cast<LuaButtonClickObserver *>(owner));
        button->removeCompanion(gc_click_companion_key);
    }
}

using ButtonUserData = LuaUserData<ButtonSelf, LuaTypeName::button>;

template<typename T, size_t left_len, size_t right_len>
constexpr std::array<T, left_len + right_len> operator + (std::array<T, left_len> _left, std::array<T, right_len> _right)
{
    std::array<T, left_len + right_len> result;
    std::copy(_right.begin(), _right.end(), std::copy(_left.begin(), _left.end(), result.begin()));
    return result;
}

WidgetState getWidgetState(lua_State * _lua, int _idx)
{
    if(lua_isinteger(_lua, _idx))
    {
        switch(lua_tointeger(_lua, _idx))
        {
        case static_cast<lua_Integer>(WidgetState::Focused):
            return WidgetState::Focused;
        case static_cast<lua_Integer>(WidgetState::Activated):
            return WidgetState::Activated;
        }
    }
    return WidgetState::Default;
}

// 1 self
// 2 dimension
template<typename UserDataT>
int luaApi_SetX(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    std::optional<Dimension<float>> dimension = tryGetDimension<float>(_lua, 2);
    luaL_argcheck(_lua, dimension.has_value(), 2, "the X value required");
    self->getWidget(_lua)->setX(dimension.value());
    return 0;
}

// 1 self
// 2 dimension
template<typename UserDataT>
int luaApi_SetY(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    std::optional<Dimension<float>> dimension = tryGetDimension<float>(_lua, 2);
    luaL_argcheck(_lua, dimension.has_value(), 2, "the Y value required");
    self->getWidget(_lua)->setY(dimension.value());
    return 0;
}

// 1 self
// 2 dimension
template<typename UserDataT>
int luaApi_SetWidth(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    std::optional<Dimension<float>> dimension = tryGetDimension<float>(_lua, 2);
    luaL_argcheck(_lua, dimension.has_value(), 2, "the width value required");
    self->getWidget(_lua)->setWidth(dimension.value());
    return 0;
}

// 1 self
// 2 dimension
template<typename UserDataT>
int luaApi_SetHeight(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    std::optional<Dimension<float>> dimension = tryGetDimension<float>(_lua, 2);
    luaL_argcheck(_lua, dimension.has_value(), 2, "the height value required");
    self->getWidget(_lua)->setHeight(dimension.value());
    return 0;
}

// 1 self
// 2 text
template<typename UserDataT>
int luaApi_SetText(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    const char * text = nullptr;
    if(lua_gettop(_lua) >= 2 && lua_isstring(_lua, 2))
        text = lua_tostring(_lua, 2);
    self->getWidget(_lua)->setText(text ? std::string(text) : std::string());
    return 0;
}

// 1 self
// 2 font
template<typename UserDataT>
int luaApi_SetFont(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    std::shared_ptr<TTF_Font> font = tryGetFont(_lua, 2);
    luaL_argcheck(_lua, font != nullptr, 2, "font required");
    self->getWidget(_lua)->font = font;
    return 0;
}

// 1 self
// 2 color
// 3 widget state?
template<typename UserDataT>
int luaApi_SetForegroundColor(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    Color color;
    luaL_argcheck(_lua, tryGetColor(_lua, 2, color), 2, gc_message_color_required);
    self->getWidget(_lua)->foreground_color.setValue(getWidgetState(_lua, 3), color);
    return 0;
}

// 1 self
// 2 color
// 3 widget state?
template<typename UserDataT>
int luaApi_SetBackgroundColor(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    Color color;
    luaL_argcheck(_lua, tryGetColor(_lua, 2, color), 2, gc_message_color_required);
    self->getWidget(_lua)->background_color.setValue(getWidgetState(_lua, 3), color);
    return 0;
}

// 1 self
// 2 color
// 3 widget state?
template<typename UserDataT>
int luaApi_SetBorderColor(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    Color color;
    luaL_argcheck(_lua, tryGetColor(_lua, 2, color), 2, gc_message_color_required);
    self->getWidget(_lua)->border_color.setValue(getWidgetState(_lua, 3), color);
    return 0;
}

// 1 self
// 2 width
// 3 widget state?
template<typename UserDataT>
int luaApi_SetBorderWidth(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isnumber(_lua, 2), 2, "width required");
    self->getWidget(_lua)->border_width.setValue(getWidgetState(_lua, 3), static_cast<float>(lua_tonumber(_lua, 2)));
    return 0;
}

// 1 self
// 2 alignment
// 3 widget state?
template<typename UserDataT>
int luaApi_SetVerticalTextAlignment(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    VerticalTextAlignment alignment;
    luaL_argcheck(_lua, tryGetVerticalTextAlignment(_lua, 2, &alignment), 2, gc_message_alignment_required);
    self->getWidget(_lua)->vertical_text_alignment.setValue(getWidgetState(_lua, 3), alignment);
    return 0;
}

// 1 self
// 2 alignment
// 3 widget state?
template<typename UserDataT>
int luaApi_SetHorizontalTextAlignment(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    HorizontalTextAlignment alignment;
    luaL_argcheck(_lua, tryGetHorizontalTextAlignment(_lua, 2, &alignment), 2, gc_message_alignment_required);
    self->getWidget(_lua)->horizontal_text_alignment.setValue(getWidgetState(_lua, 3), alignment);
    return 0;
}

// 1 self
// 2 padding
// 3 widget state?
template<typename UserDataT>
int luaApi_SetPadding(lua_State * _lua)
{
    auto * self = UserDataT::getUserData(_lua, 1);
    WidgetPadding padding;
    luaL_argcheck(_lua, tryGetWidgetPadding(_lua, 2, padding), 2, "padding required");
    self->getWidget(_lua)->padding.setValue(getWidgetState(_lua, 3), padding);
    return 0;
}

// 1 self
// 2 callback
int luaApi_ButtonSubscribeOnClick(lua_State * _lua)
{
    ButtonSelf * self = ButtonUserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, "callback required");
    uint32_t id = self->subscribeOnClick(_lua, 2);
    lua_pushinteger(_lua, static_cast<lua_Integer>(id));
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_ButtonUnsubscribeOnClick(lua_State * _lua)
{
    ButtonSelf * self = ButtonUserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "subscription ID required");
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnClick(_lua, subscription_id);
    return 0;
}

template<typename UserDataT>
constexpr std::array<luaL_Reg, 10> gc_widget_funcs =
{
    {
        { "__gc", UserDataT::luaGC },
        { "setX", luaApi_SetX<UserDataT> },
        { "setY", luaApi_SetY<UserDataT> },
        { "setWidth", luaApi_SetWidth<UserDataT> },
        { "setHeight", luaApi_SetHeight<UserDataT> },
        { "setBackgroundColor", luaApi_SetBackgroundColor<UserDataT> },
        { "setForegroundColor", luaApi_SetForegroundColor<UserDataT> },
        { "setBorderColor", luaApi_SetBorderColor<UserDataT> },
        { "setBorderWidth", luaApi_SetBorderWidth<UserDataT> },
        { "setPadding", luaApi_SetPadding<UserDataT> }
    }
};

template<typename UserDataT>
constexpr std::array<luaL_Reg, 4> gc_label_funcs =
{
    {
        { "setFont", luaApi_SetFont<UserDataT> },
        { "setText", luaApi_SetText<UserDataT> },
        { "setVerticalTextAlignment", luaApi_SetVerticalTextAlignment<UserDataT> },
        { "setHorizontalTextAlignment", luaApi_SetHorizontalTextAlignment<UserDataT> }
    }
};

constexpr std::array<luaL_Reg, 2> gc_button_funcs =
{
    {
        { "subscribeOnClick", luaApi_ButtonSubscribeOnClick },
        { "unsubscribeOnClick", luaApi_ButtonUnsubscribeOnClick }
    }
};

constexpr std::array<luaL_Reg, 1> gc_null_funcs =
{
    {
        { nullptr, nullptr }
    }
};

} // namespace

void Sol2D::Lua::pushWidgetStateEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::widget_state) == MetatablePushResult::Created)
    {
        LuaTable table(_lua);
        table.setIntegerValue("DEFAULT", static_cast<lua_Integer>(WidgetState::Default));
        table.setIntegerValue("FOCUSED", static_cast<lua_Integer>(WidgetState::Focused));
        table.setIntegerValue("ACTIVATED", static_cast<lua_Integer>(WidgetState::Activated));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushLabelApi(lua_State * _lua, std::shared_ptr<Label> _label)
{
    LabelUserData::pushUserData(_lua, _label);
    if(LabelUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs =
            gc_widget_funcs<LabelUserData> +
            gc_label_funcs<LabelUserData> +
            gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushButtonApi(lua_State * _lua, std::shared_ptr<Button> _button, const Workspace & _workspace)
{
    ButtonUserData::pushUserData(_lua, _button, _workspace);
    if(ButtonUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs =
            gc_widget_funcs<ButtonUserData> +
            gc_label_funcs<ButtonUserData> +
            gc_button_funcs +
            gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}
