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

#define DISABLE_COPY(classname) \
    classname(const classname &) = delete; \
    classname & operator = (const classname &) = delete;

#define DISABLE_COPY_AND_MOVE(classname) \
    classname(const classname &) = delete; \
    classname(classname &&) = delete; \
    classname & operator = (const classname &) = delete; \
    classname & operator = (classname &&) = delete;

#define DEFAULT_COPY(classname) \
    classname(const classname &) = default; \
    classname & operator = (const classname &) = default;

#define DEFAULT_COPY_AND_MOVE(classname) \
classname(const classname &) = default; \
    classname(classname &&) = default; \
    classname & operator = (const classname &) = default; \
    classname & operator = (classname &&) = default;

#define NOT_USED(var) ((void)var)
