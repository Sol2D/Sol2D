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

#include <Sol2D/Forms/WidgetState.h>
#include <Sol2D/Utils/Observable.h>
#include <Sol2D/Def.h>
#include <unordered_map>

namespace Sol2D::Forms {

template<typename T>
concept WidgetPropertyValueConcept = std::copyable<T> && std::default_initializable<T>;

template<WidgetPropertyValueConcept T>
class WidgetProperty;

template<WidgetPropertyValueConcept PropertyType>
class WidgetPropertyObserver
{
public:
    virtual void onPropertyChanged(const WidgetProperty<PropertyType> & _property, WidgetState _state) = 0;
};

template<WidgetPropertyValueConcept T>
class WidgetProperty : public Utils::Observable<WidgetPropertyObserver<T>>
{
public:
    S2_DEFAULT_COPY_AND_MOVE(WidgetProperty)

    WidgetProperty(const T & _value) :
        m_values(1)
    {
        m_values[WidgetState::Default] = _value;
    }

    void operator = (const T & _value)
    {
        setValue(_value);
    }

    void setValue(const T & _value)
    {
        m_values[WidgetState::Default] = _value;
        raisePropertyChanged(WidgetState::Default);
    }

    void operator = (T && _value)
    {
        setValue(std::forward(_value));
    }

    void setValue(T && _value)
    {
        m_values[WidgetState::Default] = std::forward(_value);
        raisePropertyChanged(WidgetState::Default);
    }

    void setValue(WidgetState _state, const T & _value)
    {
        m_values[_state] = _value;
        raisePropertyChanged(_state);
    }

    void setValue(WidgetState _state, T && _value)
    {
        m_values[_state] = std::move(_value);
        raisePropertyChanged(_state);
    }

    bool unsetValue(WidgetState _state)
    {
        if(_state == WidgetState::Default)
            return false;
        m_values.erase(_state);
        raisePropertyChanged(_state);
        return true;
    }

    const T & get(WidgetState _state) const
    {
        return getValue(_state);
    }

    T & get(WidgetState _state)
    {
        return getValue(_state);
    }

    const T & get() const
    {
        return getValue(WidgetState::Default);
    }

    T & get()
    {
        return getValue(WidgetState::Default);
    }

    const T & operator [] (WidgetState _state) const
    {
        return getValue(_state);
    }

    T & operator [] (WidgetState _state)
    {
        return getValue(_state);
    }

private:
    void raisePropertyChanged(WidgetState _state)
    {
        Utils::Observable<WidgetPropertyObserver<T>>::callObservers(
            &WidgetPropertyObserver<T>::onPropertyChanged, std::ref(*this), _state);
    }

    T & getValue(WidgetState _state)
    {
        auto it = m_values.find(_state);
        return m_values.cend() == it ? m_values[WidgetState::Default] : it->second;
    }

private:
    std::unordered_map<WidgetState, T> m_values;
};

} // namespace Sol2D::Forms
