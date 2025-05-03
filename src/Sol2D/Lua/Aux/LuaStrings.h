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

#include <vector>
#include <sstream>

namespace Sol2D::Lua {

struct LuaTypeName
{
    static const char nil[];
    static const char boolean[];
    static const char integer[];
    static const char number[];
    static const char string[];
    static const char function[];
    static const char lib[];
    static const char point[];
    static const char size[];
    static const char color[];
    static const char store_manager[];
    static const char window[];
    static const char area[];
    static const char view[];
    static const char scene[];
    static const char body[];
    static const char body_definition[];
    static const char body_options[];
    static const char body_shape[];
    static const char tile_map_object_type[];
    static const char keyboard[];
    static const char scancode[];
    static const char mouse[];
    static const char store[];
    static const char dimension[];
    static const char dimension_unit_type[];
    static const char sprite[];
    static const char sprite_definition[];
    static const char sprite_sheet[];
    static const char sprite_sheet_options[];
    static const char graphics_pack[];
    static const char body_type[];
    static const char body_shape_type[];
    static const char body_prototype[];
    static const char distance_joint_definition[];
    static const char motor_joint_definition[];
    static const char mouse_joint_definition[];
    static const char prismatic_joint_definition[];
    static const char revolute_joint_definition[];
    static const char weld_joint_definition[];
    static const char wheel_joint_definition[];
    static const char distance_joint[];
    static const char motor_joint[];
    static const char mouse_joint[];
    static const char prismatic_joint[];
    static const char revolute_joint[];
    static const char weld_joint[];
    static const char wheel_joint[];
    static const char font[];
    static const char form[];
    static const char node[];
    static const char ui[];
    static const char ui_form[];
    static const char vertical_text_alignment[];
    static const char horizontal_text_alignment[];
    static const char widget_state[];
    static const char widget_padding[];
    static const char button[];
    static const char label[];
    static const char sound_effect[];
    static const char music[];
    static const char style[];

    template<typename... T>
    static std::string joinTypes(const T... _type);
};

template<typename... T>
std::string LuaTypeName::joinTypes(const T... _type)
{
    std::stringstream ss;
    bool is_first = true;
    (..., [&ss, &is_first, &_type]() {
        if(is_first)
            is_first = false;
        else
            ss << ", ";
        ss << _type;
    });
    return ss.str();
}

constexpr std::vector<const char *> getAllJointTypes()
{
    return std::vector<const char *> {
        LuaTypeName::distance_joint,
        LuaTypeName::motor_joint,
        LuaTypeName::mouse_joint,
        LuaTypeName::prismatic_joint,
        LuaTypeName::revolute_joint,
        LuaTypeName::weld_joint,
        LuaTypeName::wheel_joint
    };
}

struct LuaMessage
{
    static const char store_is_destroyed[];
    static const char scene_is_destroyed[];
    static const char body_is_destroyed[];
    static const char sprite_is_destroyed[];
    static const char sprite_sheet_is_destroyed[];
    static const char view_is_destroyed[];
    static const char ui_is_destroyed[];
    static const char form_is_destroyed[];
    static const char node_is_destroyed[];
    static const char widget_is_destroyed[];
    static const char music_is_destroyed[];
    static const char sound_effect_is_destroyed[];
    static const char body_with_id_not_found_template[];

    template<std::input_iterator Iterator>
    static std::string formatOneOfTypesExpected(const Iterator & _begin, const Iterator & _end);

private:
    static const char one_of_types_expected[];
};

template<std::input_iterator Iterator>
std::string LuaMessage::formatOneOfTypesExpected(const Iterator & _begin, const Iterator & _end)
{
    std::stringstream ss;
    ss << one_of_types_expected;
    for(Iterator it = _begin; it != _end; ++it)
    {
        if(it != _begin)
            ss << ", ";
        ss << *it;
    }
    return ss.str();
}

} // namespace Sol2D::Lua
