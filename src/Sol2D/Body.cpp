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

#include <Sol2D/Body.h>
#include <Sol2D/BodyPrototype.h>

using namespace Sol2D;

Body::Body(const BodyPrototype & _proto, Box2D & _box2d) :
    mr_proto(_proto),
    mr_box2d(_box2d),
    m_active_sprite_index(INVALID_SPRITE_INDEX),
    m_width(48), // TODO: from proto
    m_height(48), // TODO: from proto
    m_flip(SDL_FLIP_NONE),
    mp_b2body(_box2d.createDynamicBody(*this))
{
}

const Sprite * Body::getActiveSprite() const
{
    if(m_active_sprite_index == INVALID_SPRITE_INDEX)
        return nullptr;
    const std::vector<Sprite> & sprites = mr_proto.getSprites();
    if(m_active_sprite_index >= sprites.size())
        return nullptr;
    return &sprites[m_active_sprite_index];
}

void Body::move(int _delta_x, int _delta_y) // TODO: refactor using a force vector
{
    mp_b2body->SetLinearVelocity(b2Vec2 {0, 0});
    const float base_force = 1500.f; // TODO: from b2FrictionJoint?
    const float force_multiplier = 900.0f;

    float force_x = _delta_x * force_multiplier;
    if(force_x > 0) force_x += base_force;
    else if(force_x < 0) force_x -= base_force;

    float force_y = _delta_y * force_multiplier;
    if(force_y > 0) force_y += base_force;
    else if(force_y < 0) force_y -= base_force;

    mp_b2body->ApplyForceToCenter(b2Vec2(force_x, force_y), true);
}

void Body::setPosition(int _x, int _y)
{
    if(mr_box2d.isLocked())
        mr_box2d.defer([this, _x, _y]() { setPositionUnsafe(_x, _y); });
    else
        setPositionUnsafe(_x, _y);
}

void Body::setPositionUnsafe(int _x, int _y)
{
    float b2_x = (static_cast<float>(_x) + static_cast<float>(m_width) / 2) / mr_box2d.getScaleFactor();
    float b2_y = (static_cast<float>(_y) + static_cast<float>(m_height) / 2) / mr_box2d.getScaleFactor();
    mp_b2body->SetTransform(b2Vec2(b2_x, b2_y), mp_b2body->GetAngle());
}

SDL_FPoint Body::getPosition() const
{
    const b2Vec2 & pos = mp_b2body->GetPosition();
    return SDL_FPoint {
        pos.x * mr_box2d.getScaleFactor() - static_cast<float>(m_width) / 2,
        pos.y * mr_box2d.getScaleFactor() - static_cast<float>(m_height) / 2
    };
}

void Body::flip(bool _horizontal, bool _vertical)
{
    if(_horizontal)
       m_flip = static_cast<SDL_RendererFlip>(m_flip | SDL_FLIP_HORIZONTAL);
    else
        m_flip = static_cast<SDL_RendererFlip>(m_flip & ~SDL_FLIP_HORIZONTAL);
    if(_vertical)
       m_flip = static_cast<SDL_RendererFlip>(m_flip | SDL_FLIP_VERTICAL);
    else
        m_flip = static_cast<SDL_RendererFlip>(m_flip & ~SDL_FLIP_VERTICAL);
}

bool Body::isFlippedHorizontally() const
{
    return (m_flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL;
}

bool Body::isFlippedVertically() const
{
    return (m_flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_VERTICAL;
}
