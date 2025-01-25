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

#include <Sol2D/Tiles/ObjectHeap.h>

using namespace Sol2D::Tiles;

const TileMapObject * ObjectHeap::findBasicObject(const std::string & _name) const
{
    auto it = mr_objects_by_name.find(_name);
    return it == mr_objects_by_name.cend() ? nullptr : it->get();
}

const TileMapObject * ObjectHeap::findBasicObject(uint32_t _gid) const
{
    auto it = mr_objects_by_id.find(_gid);
    return it == mr_objects_by_id.cend() ? nullptr : it->get();
}

void ObjectHeap::forEachObject(uint32_t _layer_id, std::function<void(const TileMapObject &)> _cb) const
{
    auto range = mr_objects_by_layer.equal_range(_layer_id);
    for(auto it = range.first; it != range.second; ++it)
        _cb(*it->get());
}

void ObjectHeap::forEachObject(uint32_t _layer_id, std::function<void(TileMapObject &)> _cb)
{
    auto range = mr_objects_by_layer.equal_range(_layer_id);
    for(auto it = range.first; it != range.second; ++it)
        _cb(*it->get());
}

void ObjectHeap::forEachObject(const std::string & _class, std::function<void(const TileMapObject &)> _cb) const
{
    auto range = mr_objects_by_class.equal_range(_class);
    for(auto it = range.first; it != range.second; ++it)
        _cb(*it->get());
}

void ObjectHeap::forEachObject(const std::string & _class, std::function<void(TileMapObject &)> _cb)
{
    auto range = mr_objects_by_class.equal_range(_class);
    for(auto it = range.first; it != range.second; ++it)
        _cb(*it->get());
}

void ObjectHeap::forEachObject(std::function<void(const TileMapObject &)> _cb) const
{
    for(const auto & obj : m_objects)
        _cb(*obj);
}

void ObjectHeap::forEachObject(std::function<void(TileMapObject &)> _cb)
{
    for(const auto & obj : m_objects)
        _cb(*obj);
}

boost::container::slist<const TileMapObject *> ObjectHeap::findBasicObjects(const std::string & _class) const
{
    auto range = mr_objects_by_class.equal_range(_class);
    boost::container::slist<const TileMapObject *> result;
    for(auto it = range.first; it != range.second; ++it)
        result.push_front(it->get());
    return result;
}
