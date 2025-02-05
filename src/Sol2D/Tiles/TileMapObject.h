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

#include <Sol2D/MediaLayer.h>
#include <string>
#include <cstdint>
#include <optional>
#include <vector>

namespace Sol2D::Tiles {

enum class TileMapObjectType
{
    Circle,
    Point,
    Polygon,
    Polyline,
    Text
};


struct TileMapObjectDef
{
    uint32_t id;
    uint32_t layer_id;
    const char * klass;
    const char * name;
};


class TileMapObject
{   
protected:
    TileMapObject(TileMapObjectType _type, const TileMapObjectDef & _def) :
        m_class(_def.klass ? _def.klass : std::string()),
        m_name(_def.name ? _def.name : std::string()),
        m_layer_id(_def.layer_id),
        m_type(_type),
        m_id(_def.id),
        m_position(.0f, .0f),
        m_is_visible(true)
    {
    }

public:
    S2_DEFAULT_COPY(TileMapObject)

    virtual ~TileMapObject() { }
    TileMapObjectType getObjectType() const { return m_type; }
    uint32_t getId() const { return m_id; }
    const std::string & getClass() const { return m_class; }
    void eraseClass() { setClass(std::string()); }
    void setClass(const char * _class) { setClass(_class ? _class : std::string()); }
    virtual void setClass(const std::string & _class) = 0;
    uint32_t getLayerId() const { return m_layer_id; }
    virtual void setLayerId(uint32_t _layer_id) = 0;
    void setName(const char * _name) { setName(_name ? _name : std::string()); }
    virtual void setName(const std::string & _name) = 0;
    const std::string & getName() const { return m_name; }
    const SDL_FPoint & getPosition() const { return m_position; }
    void setPosition(const SDL_FPoint & _position) { m_position = _position; }
    void setX(float _x) { m_position.x = _x; }
    void setY(float _y) { m_position.y = _y; }
    void setVisibility(bool _visible) { m_is_visible = _visible; }
    bool isVisible() const { return m_is_visible; }
    bool hasTileGid() const { return m_tile_gid.has_value(); }
    const std::optional<uint32_t> getTileGid() const { return m_tile_gid; }
    void setTileGid(uint32_t _gid) { m_tile_gid = _gid; }
    void eraseTileGid() { m_tile_gid.reset(); }

protected:
    std::string m_class;
    std::string m_name;
    uint32_t m_layer_id;

private:
    TileMapObjectType m_type;
    uint32_t m_id;
    SDL_FPoint m_position;
    bool m_is_visible;
    std::optional<uint32_t> m_tile_gid;
};


class TileMapObjectWithWidthAndHeight : public TileMapObject
{
public:
    TileMapObjectWithWidthAndHeight(TileMapObjectType _type, const TileMapObjectDef & _def):
        TileMapObject(_type, _def),
        m_width(.0f),
        m_height(.0f)
    {
    }

    float getWidth() const { return m_width; }
    void setWidth(float _width) { m_width = _width; }
    float getHeight() const { return m_height; }
    void setHeight(float _height) { m_height = _height; }

private:
    float m_width;
    float m_height;
};


class TileMapCircle : public TileMapObject
{
public:
    explicit TileMapCircle(const TileMapObjectDef & _def) :
        TileMapObject(TileMapObjectType::Circle, _def),
        m_radius(0)
    {
    }

    float getRadius() const { return m_radius; }
    void setRadius(float _radius) { m_radius = _radius; }

private:
    float m_radius;
};


class TileMapPoint : public TileMapObject
{
public:
    explicit TileMapPoint(const TileMapObjectDef & _def) : TileMapObject(TileMapObjectType::Point, _def) { }
};


class TileMapPolyX : public TileMapObjectWithWidthAndHeight
{
protected:
    TileMapPolyX(TileMapObjectType _type, const TileMapObjectDef & _def) :
        TileMapObjectWithWidthAndHeight(_type, _def)
    {
    }

public:
    const std::vector<SDL_FPoint> & getPoints() const
    {
        return m_points;
    }

    std::vector<SDL_FPoint> & getPoints()
    {
        return m_points;
    }

    void addPoint(const SDL_FPoint & _point)
    {
        m_points.push_back(_point);
    }

    void rotate(float _angle_rad)
    {
        Rotation rotation(_angle_rad, Rotation::AngleUnit::Radian);
        for(size_t i = 0; i < m_points.size(); ++i)
            m_points[i] = rotation.rotateVector(m_points[i]);
    }

private:
    std::vector<SDL_FPoint> m_points;
};


class TileMapPolygon : public TileMapPolyX
{
public:
    explicit TileMapPolygon(const TileMapObjectDef & _def) :
        TileMapPolyX(TileMapObjectType::Polygon, _def)
    {
    }
};


class TileMapPolyline : public TileMapPolyX
{
public:
    explicit TileMapPolyline(const TileMapObjectDef & _def) :
        TileMapPolyX(TileMapObjectType::Polyline, _def)
    {
    }
};


class TileMapText : public TileMapObjectWithWidthAndHeight
{
public:
    enum class HAlignment
    {
        Left,
        Right,
        Center,
        Justify
    };

    enum class VAlignment
    {
        Top,
        Bottom,
        Center
    };

public:
    explicit TileMapText(const TileMapObjectDef & _def) :
        TileMapObjectWithWidthAndHeight(TileMapObjectType::Text, _def),
        m_font_family("sans-serif"),
        m_font_size(16u),
        m_is_word_wrap_enabled(false),
        m_color{},
        m_is_bold(false),
        m_is_italic(false),
        m_is_underlined(false),
        m_is_struck_out(false),
        m_is_kerning_enabled(true),
        m_h_alignment(HAlignment::Left),
        m_v_alignment(VAlignment::Top)
    {
    }

    const std::string & getFontFamily() const { return m_font_family; }
    void setFontFamily(const std::string & _family) { m_font_family = _family; }
    uint16_t getFontSize() const { return m_font_size; }
    void setFontSize(uint16_t _size) { m_font_size = _size; }
    bool isWordWraEnabled() const { return m_is_word_wrap_enabled; }
    void ebableWordWrap(bool _enabled) { m_is_word_wrap_enabled = _enabled; }
    const SDL_FColor & getColor() const { return m_color; }
    void setColor(const SDL_FColor  & _color) { m_color = _color; }
    bool isBold() const { return m_is_bold; }
    void setBold(bool _bold) { m_is_bold = _bold; }
    bool isItalic() const { return m_is_italic; }
    void setItalic(bool _italic) { m_is_italic = _italic; }
    bool isUnderlined() const { return m_is_underlined; }
    void setUnderlined(bool _underlined) { m_is_underlined = _underlined; }
    bool isStruckOut() const { return m_is_struck_out; }
    void setStruckOut(bool _struck_out) { m_is_struck_out = _struck_out; }
    bool isKerningEnabled() const { return m_is_kerning_enabled; }
    void enableKerning(bool _enabled) { m_is_kerning_enabled = _enabled; }
    HAlignment getHorizontalAlignment() const { return m_h_alignment; }
    void setHorizontalAlignment(HAlignment _alignment) { m_h_alignment = _alignment; }
    VAlignment getVerticalAlignment() const { return m_v_alignment; }
    void setVerticalAlignment(VAlignment _alignment) { m_v_alignment = _alignment; }

private:
    std::string m_font_family;
    uint16_t m_font_size;
    bool m_is_word_wrap_enabled;
    SDL_FColor m_color;
    bool m_is_bold;
    bool m_is_italic;
    bool m_is_underlined;
    bool m_is_struck_out;
    bool m_is_kerning_enabled;
    HAlignment m_h_alignment;
    VAlignment m_v_alignment;
};

} // namespace Sol2D::Tiles
