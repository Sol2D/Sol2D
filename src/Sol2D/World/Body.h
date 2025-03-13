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

#include <Sol2D/World/BodyShape.h>
#include <Sol2D/World/ActionQueue.h>
#include <Sol2D/Utils/PreHashedMap.h>
#include <Sol2D/Utils/SequentialId.h>
#include <optional>

namespace Sol2D::World {

class Body final
{
    S2_DISABLE_COPY_AND_MOVE(Body)

public:
    explicit Body(b2BodyId _b2_body_id, ActionQueue & _action_queue) :
        m_gid(s_sequential_id.getNext()),
        m_b2_body_id(_b2_body_id),
        m_action_queue(_action_queue)
    {
    }

    ~Body()
    {
        for(const auto & shape : m_shapes)
            delete shape.second;
    }

    uint64_t getGid() const
    {
        return m_gid;
    }

    void setPosition(const SDL_FPoint & _position)
    {
        m_action_queue.enqueueAction([this, _position]() {
            if(B2_IS_NON_NULL(m_b2_body_id))
                b2Body_SetTransform(m_b2_body_id, toBox2D(_position), b2Body_GetRotation(m_b2_body_id));
        });
    }

    std::optional<SDL_FPoint> getPosition() const
    {
        if(B2_IS_NON_NULL(m_b2_body_id))
            return toSDL(b2Body_GetPosition(m_b2_body_id));
        return std::nullopt;
    }

    std::optional<float> getMass() const
    {
        if(B2_IS_NULL(m_b2_body_id))
            return std::nullopt;
        return b2Body_GetMass(m_b2_body_id);
    }

    SDL_FPoint getLinearVelocity() const
    {
        return toSDL(b2Body_GetLinearVelocity(m_b2_body_id));
    }

    bool setLinearVelocity(const SDL_FPoint & _velocity) const
    {
        if(B2_IS_NULL(m_b2_body_id))
            return false;
        b2Body_SetLinearVelocity(m_b2_body_id, toBox2D(_velocity));
        return true;
    }

    void applyForceToCenter(const SDL_FPoint & _force)
    {
        m_action_queue.enqueueAction([this, _force]() {
            if(B2_IS_NON_NULL(m_b2_body_id))
                b2Body_ApplyForceToCenter(m_b2_body_id, toBox2D(_force), true); // TODO: what is wake?
        });
    }

    void applyImpulseToCenter(const SDL_FPoint & _impulse)
    {
        m_action_queue.enqueueAction([this, _impulse]() {
            if(B2_IS_NON_NULL(m_b2_body_id))
                b2Body_ApplyLinearImpulseToCenter(m_b2_body_id, toBox2D(_impulse), true); // TODO: what is wake?
        });
    }

    BodyShape & createShape(const std::string & _key, std::optional<uint32_t> _tile_map_object_id = std::nullopt)
    {
        BodyShape * shape = new BodyShape(_key, _tile_map_object_id);
        m_shapes.insert(std::make_pair(_key, shape));
        return *shape;
    }

    BodyShape * findShape(const Utils::PreHashedKey<std::string> & _key)
    {
        auto it = m_shapes.find(_key);
        return it == m_shapes.end() ? nullptr : it->second;
    }

    void setLayer(const std::optional<std::string> & _layer)
    {
        m_layer = _layer;
    }

    const std::optional<std::string> & getLayer() const
    {
        return m_layer;
    }

private:
    static Utils::SequentialId<uint64_t> s_sequential_id;
    uint64_t m_gid;
    b2BodyId m_b2_body_id;
    ActionQueue & m_action_queue;
    Utils::PreHashedMap<std::string, BodyShape *> m_shapes;
    std::optional<std::string> m_layer;
};

} // namespace Sol2D::World
