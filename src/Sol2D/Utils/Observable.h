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

#include <boost/container/slist.hpp>
#include <functional>
#include <algorithm>

namespace Sol2D::Utils {

template<typename Observer>
class Observable
{
public:
    virtual ~Observable() { }
    void addObserver(Observer & _observer);
    bool removeObserver(Observer & _observer);

protected:
    void callObservers(std::function<void(Observer &)> _callback);

private:
    boost::container::slist<Observer *> m_observers;
};

template<typename Observer>
inline void Observable<Observer>::addObserver(Observer & _observer)
{
    m_observers.push_front(&_observer);
}

template<typename Observer>
bool Observable<Observer>::removeObserver(Observer & _observer)
{
    auto it = std::find(m_observers.begin(), m_observers.end(), &_observer);
    if(it != m_observers.end())
    {
        m_observers.erase(it);
        return true;
    }
    return false;
}

template<typename Observer>
inline void Observable<Observer>::callObservers(std::function<void(Observer &)> _callback)
{
    for(auto & observer : m_observers)
        _callback(*observer);
}

} // namespace Sol2D::Utils
