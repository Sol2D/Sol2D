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

#define S2_DISABLE_COPY(classname) \
    classname(const classname &) = delete; \
    classname & operator = (const classname &) = delete;

#define S2_DISABLE_MOVE(classname) \
    classname(classname &&) = delete; \
    classname & operator = (classname &&) = delete;

#define S2_DISABLE_COPY_AND_MOVE(classname) S2_DISABLE_COPY(classname) \
    S2_DISABLE_MOVE(classname)

#define S2_DEFAULT_COPY(classname) \
    classname(const classname &) = default; \
    classname & operator = (const classname &) = default;

#define S2_DEFAULT_MOVE(classname) \
    classname(classname &&) = default; \
    classname & operator = (classname &&) = default;

#define S2_DEFAULT_COPY_AND_MOVE(classname) \
    S2_DEFAULT_COPY(classname) \
    S2_DEFAULT_MOVE(classname)

#define S2_UNUSED(var) ((void)var);
