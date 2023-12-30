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

#include <Sol2D/Sprite.h>
#include <Sol2D/Box2D.h>
#include <Sol2D/Def.h>
#include <string>

namespace Sol2D {

class BodyPrototype;

class Body final
{
    DISABLE_COPY_AND_MOVE(Body)

public:
    Body(const BodyPrototype & _proto, Box2D & _box2d);
    void setActiveSprite(SpriteIndex _index);
    const Sprite * getActiveSprite() const;
    SpriteIndex getActiveSpriteIndex() const;
    void setPosition(int _x, int _y);
    SDL_FPoint getPosition() const;
    float getAngle() const;
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    void move(int _delta_x, int _delta_y); // TODO: applyForce
    void flip(bool _horizontal, bool _vertical);
    bool isFlippedHorizontally() const;
    bool isFlippedVertically() const;
    SDL_RendererFlip getFlip() const;
    void setLayer(const std::string & _layer);
    const std::string & getLayer() const;

private:
    void setPositionUnsafe(int _x, int _y);

private:
    const BodyPrototype & mr_proto;
    Box2D & mr_box2d;
    SpriteIndex m_active_sprite_index;
    uint32_t m_width; // TODO: from the proto
    uint32_t m_height; // TODO: from the proto
    SDL_RendererFlip m_flip;
    std::string m_layer;
    b2Body * mp_b2body;
};

inline void Body::setActiveSprite(SpriteIndex _index)
{
    m_active_sprite_index = _index;
}

inline SpriteIndex Body::getActiveSpriteIndex() const
{
    return m_active_sprite_index;
}

inline float Body::getAngle() const
{
    return mp_b2body->GetAngle();
}

inline SDL_RendererFlip Body::getFlip() const
{
    return m_flip;
}

inline void Body::setLayer(const std::string & _layer)
{
    m_layer = _layer;
}

inline const std::string & Body::getLayer() const
{
    return m_layer;
}

} // namespace Sol2D
