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

#include <Sol2D/Def.h>
#include <boost/container/slist.hpp>
#include <functional>

namespace Sol2D::World {

class ActionQueue final
{
    S2_DISABLE_COPY_AND_MOVE(ActionQueue)

public:
    ActionQueue(boost::container::slist<std::function<void()>> & _actions) :
        mr_actions(_actions)
    {
    }

    void enqueueAction(std::function<void()> _action)
    {
        mr_actions.push_front(_action);
    }

private:
    boost::container::slist<std::function<void()>> & mr_actions;
};

class ActionAccumulator final
{
    S2_DISABLE_COPY_AND_MOVE(ActionAccumulator)

public:
    ActionAccumulator() :
        m_queue(m_actions)
    {
    }

    void executeActions()
    {
        if(!m_actions.empty())
        {
            for(auto & action : m_actions)
                action();
        }
        m_actions.clear();
    }

    ActionQueue & getQueue()
    {
        return m_queue;
    }

private:
    boost::container::slist<std::function<void()>> m_actions;
    ActionQueue m_queue;
};

} // namespace Sol2D::World
