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

#include <stdexcept>

namespace Sol2D {

class Exception : public std::runtime_error
{
public:
    explicit Exception(const std::string & _message) : std::runtime_error(_message) { }
};

} // namespace Sol2D

#define DEFINE_EXCEPTION(classname) \
    class classname : public Sol2D::Exception \
    { \
    public: \
        explicit classname(const std::string & _message) : Sol2D::Exception(_message) { } \
    };

namespace Sol2D {

DEFINE_EXCEPTION(IOException)

DEFINE_EXCEPTION(NotSupportedException)

} // namespace Sol2D
