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

namespace Sol2D::Lua {

struct LuaTypeName
{
    static const char lib[];
    static const char store_manager[];
    static const char window[];
    static const char view[];
    static const char scene[];
    static const char body[];
    static const char body_shape[];
    static const char tile_map_object_type[];
    static const char keyboard[];
    static const char scancode[];
    static const char store[];
    static const char dimension_unit_type[];
    static const char sprite[];
    static const char sprite_sheet[];
    static const char graphics_pack[];
    static const char body_type[];
    static const char body_shape_type[];
    static const char body_prototype[];
    static const char font[];
    static const char form[];
    static const char vertical_text_alignment[];
    static const char horizontal_text_alignment[];
    static const char widget_state[];
    static const char button[];
    static const char label[];
    static const char sound_effect[];
    static const char music[];
};

} // namespace Sol2D::Lua
