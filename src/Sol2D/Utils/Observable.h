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

#include <boost/container/slist.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <atomic>

namespace Sol2D::Utils {

template<typename Method>
concept ObserverMethodConcept = std::is_member_function_pointer<Method>::value;

template<typename Observer>
class Observable
{
private:
    using ObserverList = boost::container::slist<Observer *>;

public:
    Observable();

    virtual ~Observable()
    {
    }

    void addObserver(Observer & _observer);
    void removeObserver(Observer & _observer);

protected:
    template<ObserverMethodConcept Method, typename... Args>
    void callObservers(Method _method, Args... _args);
    void forEachObserver(std::function<bool(Observer &)> _callback);

private:
    void modifyObserverCollection(std::function<void(ObserverList &)> _callback);

private:
    std::atomic<std::shared_ptr<ObserverList>> m_observers;
};

template<typename Observer>
Observable<Observer>::Observable()
{
    m_observers.store(std::shared_ptr<ObserverList>(new ObserverList), std::memory_order::relaxed);
}

template<typename Observer>
void Observable<Observer>::addObserver(Observer & _observer)
{
    modifyObserverCollection([&_observer](ObserverList & observers) { observers.push_front(&_observer); });
}

template<typename Observer>
void Observable<Observer>::removeObserver(Observer & _observer)
{
    modifyObserverCollection([&_observer](ObserverList & observers) {
        auto it = std::find(observers.begin(), observers.end(), &_observer);
        if(it != observers.end())
            observers.erase(it);
    });
}

template<typename Observer>
void Observable<Observer>::modifyObserverCollection(std::function<void(ObserverList &)> _callback)
{
    while(true)
    {
        auto origin = m_observers.load(std::memory_order::acquire);
        std::shared_ptr<ObserverList> copy = std::make_shared<ObserverList>(*origin);
        _callback(*copy);
        if(m_observers.compare_exchange_strong(origin, copy, std::memory_order::release, std::memory_order::relaxed))
            return;
    }
}

template<typename Observer>
template<ObserverMethodConcept Method, typename... Args>
void Observable<Observer>::callObservers(Method _method, Args... _args)
{
    auto observers = m_observers.load(std::memory_order::acquire);
    for(auto & observer : *observers)
        (observer->*_method)(_args...);
}

template<typename Observer>
void Observable<Observer>::forEachObserver(std::function<bool(Observer &)> _callback)
{
    auto observers = m_observers.load(std::memory_order::acquire);
    for(auto & observer : *observers)
    {
        if(!_callback(*observer))
            return;
    }
}

} // namespace Sol2D::Utils
