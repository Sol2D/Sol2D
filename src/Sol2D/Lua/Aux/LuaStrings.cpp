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

#include <Sol2D/Lua/Aux/LuaStrings.h>

using namespace Sol2D::Lua;

const char LuaTypeName::nil[] = "nil";
const char LuaTypeName::boolean[] = "boolean";
const char LuaTypeName::integer[] = "integer";
const char LuaTypeName::number[] = "number";
const char LuaTypeName::string[] = "string";
const char LuaTypeName::function[] = "function";
const char LuaTypeName::point[] = "sol.Point";
const char LuaTypeName::size[] = "sol.Size";
const char LuaTypeName::color[] = "sol.Color";
const char LuaTypeName::dimension[] = "sol.Dimension";
const char LuaTypeName::lib[] = "sol.Library";
const char LuaTypeName::store_manager[] = "sol.StoreManager";
const char LuaTypeName::window[] = "sol.Window";
const char LuaTypeName::view[] = "sol.View";
const char LuaTypeName::scene[] = "sol.Scene";
const char LuaTypeName::body[] = "sol.Body";
const char LuaTypeName::body_definition[] = "sol.BodyDefinition";
const char LuaTypeName::body_options[] = "sol.BodyOptions";
const char LuaTypeName::body_shape[] = "sol.BodyShape";
const char LuaTypeName::tile_map_object_type[] = "sol.TileMapObjectType";
const char LuaTypeName::keyboard[] = "sol.Keyboard";
const char LuaTypeName::scancode[] = "sol.Scancode";
const char LuaTypeName::mouse[] = "sol.Mouse";
const char LuaTypeName::store[] = "sol.Store";
const char LuaTypeName::dimension_unit_type[] = "sol.DimensionUnit";
const char LuaTypeName::sprite_definition[] = "sol.SpriteDefinition";
const char LuaTypeName::sprite[] = "sol.Sprite";
const char LuaTypeName::sprite_sheet_options[] = "sol.SpriteSheetOptions";
const char LuaTypeName::sprite_sheet[] = "sol.SpriteSheet";
const char LuaTypeName::graphics_pack[] = "sol.GraphicsPack";
const char LuaTypeName::body_type[] = "sol.BodyType";
const char LuaTypeName::body_shape_type[] = "sol.BodyShapeType";
const char LuaTypeName::body_prototype[] = "sol.BodyPrototype";
const char LuaTypeName::distance_joint_definition[] = "sol.DistanceJointDefinition";
const char LuaTypeName::motor_joint_definition[] = "sol.MotorJointDefinition";
const char LuaTypeName::mouse_joint_definition[] = "sol.MouseJointDefinition";
const char LuaTypeName::prismatic_joint_definition[] = "sol.PrismaticJointDefinition";
const char LuaTypeName::revolute_joint_definition[] = "sol.RevoluteJointDefinition";
const char LuaTypeName::weld_joint_definition[] = "sol.WeldJointDefinition";
const char LuaTypeName::wheel_joint_definition[] = "sol.WheelJointDefinition";
const char LuaTypeName::distance_joint[] = "sol.DistanceJoint";
const char LuaTypeName::motor_joint[] = "sol.MotorJoint";
const char LuaTypeName::mouse_joint[] = "sol.MouseJoint";
const char LuaTypeName::prismatic_joint[] = "sol.PrismaticJoint";
const char LuaTypeName::revolute_joint[] = "sol.RevoluteJoint";
const char LuaTypeName::weld_joint[] = "sol.WeldJoint";
const char LuaTypeName::wheel_joint[] = "sol.WheelJoint";
const char LuaTypeName::font[] = "sol.Font";
const char LuaTypeName::node[] = "sol.Node";
const char LuaTypeName::ui[] = "sol.UI";
const char LuaTypeName::ui_form[] = "sol.UIForm";
const char LuaTypeName::vertical_text_alignment[] = "sol.VerticalTextAlignment";
const char LuaTypeName::horizontal_text_alignment[] = "sol.HorizontalTextAlignment";
const char LuaTypeName::sound_effect[] = "sol.SoundEffect";
const char LuaTypeName::music[] = "sol.Music";
const char LuaTypeName::style[] = "sol.Style";
const char LuaTypeName::content_alignment[] = "sol.ContentAlignment";
const char LuaTypeName::content_justification[] = "sol.ContentJustification";
const char LuaTypeName::item_alignment[] = "sol.ItemAlignment";
const char LuaTypeName::display_mode[] = "sol.DisplayMode";
const char LuaTypeName::flex_direction[] = "sol.FlexDirection";
const char LuaTypeName::flex_wrap[] = "sol.FlexWrap";
const char LuaTypeName::edge[] = "sol.Edge";
const char LuaTypeName::gap_gutter[] = "sol.GapGutter";
const char LuaTypeName::position_type[] = "sol.PositionType";
const char LuaTypeName::position_unit[] = "sol.PositionUnit";
const char LuaTypeName::position[] = "sol.Position";
const char LuaTypeName::position_map[] = "sol.PositionMap";
const char LuaTypeName::dimension_unit[] = "sol.DimensionUnit";
const char LuaTypeName::dimension_limit_unit[] = "sol.DimensionLimitUnit";
const char LuaMessage::store_is_destroyed[] = "the store is invalid or has been destroyed";
const char LuaMessage::scene_is_destroyed[] = "the scene is invalid or has been destroyed";
const char LuaMessage::body_is_destroyed[] = "the body is invalid or has been destroyed";
const char LuaMessage::sprite_is_destroyed[] = "the sprite is invalid or has been destroyed";
const char LuaMessage::sprite_sheet_is_destroyed[] = "the sprite sheet is invalid or has been destroyed";
const char LuaMessage::view_is_destroyed[] = "the view is invalid or has been destroyed";
const char LuaMessage::ui_is_destroyed[] = "the UI is invalid or has been destroyed";
const char LuaMessage::node_is_destroyed[] = "the node is invalid or has been destroyed";
const char LuaMessage::music_is_destroyed[] = "the music is invalid or has been destroyed";
const char LuaMessage::sound_effect_is_destroyed[] = "the sound effect is invalid or has been destroyed";
const char LuaMessage::body_with_id_not_found_template[] = "body with ID %I does not exist";
const char LuaMessage::one_of_types_expected[] = "one of the following types is expected: ";
