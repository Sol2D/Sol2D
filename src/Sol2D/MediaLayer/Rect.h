// // Sol2D Game Engine
// // Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
// //
// // This program is free software: you can redistribute it and/or modify it under
// // the terms of the GNU Lesser General Public License as published by the Free
// // Software Foundation, either version 3 of the License, or (at your option) any
// // later version.
// //
// // This program is distributed in the hope that it will be useful, but WITHOUT
// // ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// // FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// // details.
// //
// // You should have received a copy of the GNU Lesser General Public License
// // along with this program. If not, see <http://www.gnu.org/licenses/>.

// #pragma once

// #include <Sol2D/MediaLayer/Size.h>

// namespace Sol2D {

// struct Rect
// {
//     Rect() :
//         x(.0f),
//         y(.0f),
//         w(.0f),
//         h(.0f)
//     {
//     }

//     Rect(const Rect & _rect) :
//         x(_rect.x),
//         y(_rect.y),
//         w(_rect.w),
//         h(_rect.h)
//     {
//     }

//     Rect(float _x, float _y, float _w, float _h) :
//         x(_x),
//         y(_y),
//         w(_w),
//         h(_h)
//     {
//     }

//     Rect(const glm::vec2 & _point, float _w, float _h) :
//         x(_point.x),
//         y(_point.y),
//         w(_w),
//         h(_h)
//     {
//     }

//     Rect(const glm::vec2 & _point, const FSize & _size) :
//         x(_point.x),
//         y(_point.y),
//         w(_size.w),
//         h(_size.h)
//     {
//     }

//     Rect(float _x, float _y, const FSize & _size) :
//         x(_x),
//         y(_y),
//         w(_size.w),
//         h(_size.h)
//     {
//     }

//     Rect(const FSize& _size) :
//         x(.0f),
//         y(.0f),
//         w(_size.w),
//         h(_size.h)
//     {
//     }

//     Rect(float _w, float _h) :
//         x(.0f),
//         y(.0f),
//         w(_w),
//         h(_h)
//     {
//     }

//     Rect & operator = (const Rect & _rect)
//     {
//         if(this != &_rect)
//         {
//             x = _rect.x;
//             y = _rect.y;
//             w = _rect.w;
//             h = _rect.h;
//         }
//         return *this;
//     }

//     Rect & operator = (const FSize & _size)
//     {
//         x = .0f;
//         y = .0f;
//         w = _size.w;
//         h = _size.h;
//         return *this;
//     }

//     Rect operator + (const glm::vec2 & _point) const
//     {
//         return Rect(x + _point.x, y + _point.y, w, h);
//     }

//     Rect & operator += (const glm::vec2 & _point)
//     {
//         x += _point.x;
//         y += _point.y;
//         return *this;
//     }

//     Rect operator - (const glm::vec2 & _point) const
//     {
//         return Rect(x - _point.x, y - _point.y, w, h);
//     }

//     Rect & operator -= (const glm::vec2 & _point)
//     {
//         x -= _point.x;
//         y -= _point.y;
//         return *this;
//     }

//     glm::vec2 getPoint() const
//     {
//         return glm::vec2(x,  y);
//     }

//     operator glm::vec2() const
//     {
//         return glm::vec2(x, y);
//     }

//     FSize getSize() const
//     {
//         return Size(w, h);
//     }

//     operator FSize() const
//     {
//         return FSize(w, h);
//     }

//     float x;
//     float y;
//     float w;
//     float h;
// };

// } // namespace Sol2D
