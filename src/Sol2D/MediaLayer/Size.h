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

#include <Sol2D/MediaLayer/Number.h>

namespace Sol2D {

template<Number NumT>
struct Size
{
    Size() :
        w(0),
        h(0)
    {
    }

    Size(NumT _w, NumT _h) :
        w(_w),
        h(_h)
    {
    }

    Size(const Size & _size) :
        w(_size.w),
        h(_size.h)
    {
    }

    Size & operator = (const Size & _size)
    {
        if(this != &_size)
        {
            w = _size.w;
            h = _size.h;
        }
        return *this;
    }

    Size operator + (const Size & _size) const
    {
        return SizeF(w + _size.w, h + _size.h);
    }

    Size & operator += (const Size & _size)
    {
        w += _size.w;
        h += _size.h;
        return *this;
    }

    Size operator - (const Size & _size) const
    {
        return SizeF(w - _size.w, h - _size.h);
    }

    Size & operator -= (const Size & _size)
    {
        w -= _size.w;
        h -= _size.h;
        return *this;
    }

    Size operator * (float _n) const
    {
        return SizeF(w * _n, h * _n);
    }

    Size & operator *= (float _n)
    {
        w *= _n;
        h *= _n;
        return *this;
    }

    Size operator / (float _n) const
    {
        return SizeF(w / _n, h / _n);
    }

    Size & operator /= (float _n)
    {
        w /= _n;
        h /= _n;
        return *this;
    }

    NumT w;
    NumT h;
};

using FSize = Size<float>;
using USize = Size<uint32_t>;

} // namespace Sol2D
