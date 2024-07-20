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

#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>

namespace Sol2D::Lua::Aux {

template<typename Observable, typename Observer>
concept ObservableConcept = requires(Observable _observable, Observer & _observer)
{
    _observable.addObserver(_observer);
    _observable.removeObserver(_observer);
};

template<typename Observer, ObservableConcept<Observer> Observable>
struct LuaCallbackSubscribable
{
private:
    S2_DEFAULT_COPY_AND_MOVE(LuaCallbackSubscribable)

    struct ObserverWrapper
    {
        Observer * observer;
        int link_counter;
    };

public:
    LuaCallbackSubscribable(lua_State * _lua, std::weak_ptr<Observable> _observable);
    virtual ~LuaCallbackSubscribable();
    uint32_t subscribe(uint16_t _event_id, int _callback_idx);
    void unsubscribe(uint16_t _event_id, int _subscription_id);

protected:
    std::shared_ptr<Observable> getObservable() const;

protected:
    virtual Observer * createObserver(lua_State * _lua) = 0;

private:
    lua_State * mp_lua;
    std::weak_ptr<Observable> m_observable;
    ObserverWrapper m_observer_wrapper;
};

template<typename Observer, ObservableConcept<Observer> Observable>
inline LuaCallbackSubscribable<Observer, Observable>::LuaCallbackSubscribable(
    lua_State * _lua,
    std::weak_ptr<Observable> _observable
) :
    mp_lua(_lua),
    m_observable(_observable),
    m_observer_wrapper{nullptr, 0}
{
}

template<typename Observer, ObservableConcept<Observer> Observable>
LuaCallbackSubscribable<Observer, Observable>::~LuaCallbackSubscribable()
{
    LuaCallbackStorage(mp_lua).destroyCallbacks(this);
    if(m_observer_wrapper.observer)
    {
        if(auto observable = getObservable())
            observable->removeObserver(*m_observer_wrapper.observer);
        delete m_observer_wrapper.observer;
    }
}

template<typename Observer, ObservableConcept<Observer> Observable>
inline std::shared_ptr<Observable> LuaCallbackSubscribable<Observer, Observable>::getObservable() const
{
    return m_observable.lock();
}

template<typename Observer, ObservableConcept<Observer> Observable>
uint32_t LuaCallbackSubscribable<Observer, Observable>::subscribe(uint16_t _event_id, int _callback_idx)
{
    LuaCallbackStorage callback_storage(mp_lua);
    uint32_t subscription_id = callback_storage.addCallback(this, _event_id, _callback_idx);
    if(m_observer_wrapper.observer)
    {
        ++m_observer_wrapper.link_counter;
    }
    else
    {
        m_observer_wrapper.observer = createObserver(mp_lua);
        m_observer_wrapper.link_counter = 1;
        if(auto observable = getObservable())
            observable->addObserver(*m_observer_wrapper.observer);
    }
    return subscription_id;
}

template<typename Observer, ObservableConcept<Observer> Observable>
void LuaCallbackSubscribable<Observer, Observable>::unsubscribe(uint16_t _event_id, int _subscription_id)
{
    if(m_observer_wrapper.observer && --m_observer_wrapper.link_counter == 0)
    {
        if(auto observable = getObservable())
            observable->removeObserver(*m_observer_wrapper.observer);
        delete m_observer_wrapper.observer;
        m_observer_wrapper.observer = nullptr;
    }
    LuaCallbackStorage callback_storage(mp_lua);
    callback_storage.removeCallback(this, _event_id, _subscription_id);
}

} // namespace Sol2D::Lua::Aux
