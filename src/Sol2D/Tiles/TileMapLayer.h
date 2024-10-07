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

#include <Sol2D/Color.h>
#include <Sol2D/Def.h>
#include <string>
#include <optional>
#include <cstdint>

namespace Sol2D::Tiles {

enum class TileMapLayerType
{
    Tile,
    Object,
    Image,
    Group
};

class TileMapLayer
{
    S2_DISABLE_COPY_AND_MOVE(TileMapLayer)

public:
    TileMapLayer(const TileMapLayer * _parent, const uint32_t _id, const std::string & _name, TileMapLayerType _type) :
        mp_parent(_parent),
        m_id(_id),
        m_name(_name),
        m_type(_type),
        m_is_visible(true),
        m_opacity(1.0f),
        m_offset_x(0),
        m_offset_y(0),
        m_parallax_x(1.0f),
        m_parallax_y(1.0f)
    {
    }

    virtual ~TileMapLayer() { }
    const TileMapLayer * getParent() const { return mp_parent; }
    uint32_t getId() const { return m_id; }
    const std::string & getName() const { return m_name; }
    TileMapLayerType getType() const { return m_type; }
    void setClass(const char * _class) { m_class = _class ? _class : std::string(); }
    const std::string & getClass() const { return m_class; }
    void setVisibility(bool _visible) { m_is_visible = _visible; }
    bool isVisible() const { return m_is_visible; }
    void setOpacity(float _opacity) { m_opacity = _opacity; }
    float getOpacity() const { return m_opacity; }
    void setOffsetX(float _offset) { m_offset_x = _offset; }
    float getOffsetX() const { return m_offset_x; }
    void setOffsetY(float _offset) { m_offset_y = _offset; }
    float getOffsetY() const { return m_offset_y; }
    void setParallaxX(float _parallax) { m_parallax_x = _parallax; }
    float getParallaxX() const { return m_parallax_x; }
    void setParallaxY(float _parallax) { m_parallax_y = _parallax; }
    float getParallaxY() const { return m_parallax_y; }
    void setTintColor(const Color & _color) { m_tint_color = _color; }
    void removeTintColor() { m_tint_color.reset(); }
    const std::optional<Color> & getTintColor() const { return m_tint_color; }

private:
    const TileMapLayer * mp_parent;
    uint32_t m_id;
    const std::string m_name;
    TileMapLayerType m_type;
    std::string m_class;
    bool m_is_visible;
    float m_opacity;
    float m_offset_x;
    float m_offset_y;
    float m_parallax_x; // TODO: To render parallax https://doc.mapeditor.org/en/stable/manual/layers/#parallax-factor
    float m_parallax_y;
    std::optional<Color> m_tint_color; // TODO: To render tint color. https://doc.mapeditor.org/en/stable/manual/layers/#tint-color
};

} // namespace Sol2D::Tiles
