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

#include <Sol2D/Tiles/TileMapObject.h>
#include <Sol2D/Def.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <functional>
#include <memory>
#include <boost/container/slist.hpp>

namespace Sol2D::Tiles {

template<typename T>
concept TileMapObjectConcept =
    std::same_as<T, TileMapObject> || std::same_as<T, TileMapCircle> || std::same_as<T, TileMapPoint> ||
    std::same_as<T, TileMapPolygon> || std::same_as<T, TileMapPolyline> || std::same_as<T, TileMapText>;

class ObjectHeap final
{
    S2_DISABLE_COPY_AND_MOVE(ObjectHeap)

private:
    struct ObjectIdTag
    {
    };
    struct ObjectLayerTag
    {
    };
    struct ObjectClassTag
    {
    };
    struct ObjectNameTag
    {
    };

    using ObjectMap = boost::multi_index_container<
        std::shared_ptr<TileMapObject>,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_unique<
                boost::multi_index::tag<ObjectIdTag>,
                boost::multi_index::const_mem_fun<TileMapObject, uint32_t, &TileMapObject::getId>>,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<ObjectLayerTag>,
                boost::multi_index::const_mem_fun<TileMapObject, uint32_t, &TileMapObject::getLayerId>>,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<ObjectClassTag>,
                boost::multi_index::const_mem_fun<TileMapObject, const std::string &, &TileMapObject::getClass>>,
            boost::multi_index::hashed_non_unique<
                boost::multi_index::tag<ObjectNameTag>,
                boost::multi_index::const_mem_fun<TileMapObject, const std::string &, &TileMapObject::getName>>>>;

    using ObjectMapById = ObjectMap::index<ObjectIdTag>::type;
    using ObjectMapByLayer = ObjectMap::index<ObjectLayerTag>::type;
    using ObjectMapByClass = ObjectMap::index<ObjectClassTag>::type;
    using ObjectMapByName = ObjectMap::index<ObjectNameTag>::type;

    template<TileMapObjectConcept TBase>
    class ObjectImpl : public TBase
    {
    public:
        ObjectImpl(ObjectMapById & _map, const TileMapObjectDef & _def) :
            TBase(_def),
            m_map(_map)
        {
        }

        void setClass(const std::string & _class) override
        {
            m_map.modify(m_map.find(TBase::getId()), [&_class](std::shared_ptr<TileMapObject> & __object) {
                static_cast<ObjectImpl<TBase> *>(__object.get())->m_class = _class;
            });

            // TODO: listeners, like create/destroy box2d object
        }

        void setLayerId(uint32_t _layer_id) override
        {
            m_map.modify(m_map.find(TBase::getId()), [_layer_id](std::shared_ptr<TileMapObject> & __object) {
                static_cast<ObjectImpl<TBase> *>(__object.get())->m_layer_id = _layer_id;
            });

            // TODO: listeners?
        }

        void setName(const std::string & _name) override
        {
            m_map.modify(m_map.find(TBase::getId()), [&_name](std::shared_ptr<TileMapObject> & __object) {
                static_cast<ObjectImpl<TBase> *>(__object.get())->m_name = _name;
            });

            // TODO: listeners?
        }

    private:
        ObjectMapById & m_map;
    };

public:
    ObjectHeap();

    template<TileMapObjectConcept T>
    T & createObject(uint32_t _layer_id, const char * _class, const char * _name);

    template<TileMapObjectConcept T>
    T & createObject(uint32_t _layer_id, uint32_t _gid, const char * _class, const char * _name);

    template<TileMapObjectConcept T>
    const T * findObject(const std::string & _name) const;

    template<TileMapObjectConcept T>
    const T * findObject(uint32_t _gid) const;

    const TileMapObject * findBasicObject(const std::string & _name) const;
    const TileMapObject * findBasicObject(uint32_t _gid) const;
    boost::container::slist<const TileMapObject *> findBasicObjects(const std::string & _class) const;
    void forEachObject(uint32_t _layer_id, std::function<void(const TileMapObject &)> _cb) const;
    void forEachObject(uint32_t _layer_id, std::function<void(TileMapObject &)> _cb);
    void forEachObject(const std::string & _class, std::function<void(const TileMapObject &)> _cb) const;
    void forEachObject(const std::string & _class, std::function<void(TileMapObject &)> _cb);
    void forEachObject(std::function<void(const TileMapObject &)> _cb) const;
    void forEachObject(std::function<void(TileMapObject &)> _cb);

private:
    ObjectMap m_objects;
    ObjectMapById & m_objects_by_id;
    ObjectMapByLayer & m_objects_by_layer;
    ObjectMapByClass & m_objects_by_class;
    ObjectMapByName & m_objects_by_name;
    uint32_t m_next_gid;
};

inline ObjectHeap::ObjectHeap() :
    m_objects_by_id(m_objects.get<ObjectIdTag>()),
    m_objects_by_layer(m_objects.get<ObjectLayerTag>()),
    m_objects_by_class(m_objects.get<ObjectClassTag>()),
    m_objects_by_name(m_objects.get<ObjectNameTag>()),
    m_next_gid(1)
{
}

template<TileMapObjectConcept T>
inline T & ObjectHeap::createObject(uint32_t _layer_id, const char * _class, const char * _name)
{
    return createObject<T>(_layer_id, m_next_gid, _class, _name);
}

template<TileMapObjectConcept T>
T & ObjectHeap::createObject(uint32_t _layer_id, uint32_t _gid, const char * _class, const char * _name)
{
    if(m_objects.contains(_gid))
    {
        // TODO: if ID exists (see TileHeap)
    }
    TileMapObjectDef def {.id = _gid, .layer_id = _layer_id, .klass = _class, .name = _name};
    ObjectImpl<T> * object = new ObjectImpl<T>(m_objects_by_id, def);
    m_objects.insert(std::shared_ptr<TileMapObject>(object));
    if(m_next_gid <= _gid)
        m_next_gid = _gid + 1;
    return *object;
}

template<TileMapObjectConcept T>
inline const T * ObjectHeap::findObject(const std::string & _name) const
{
    const TileMapObject * object = findBasicObject(_name);
    return object ? dynamic_cast<const T *>(object) : nullptr;
}

template<TileMapObjectConcept T>
inline const T * ObjectHeap::findObject(uint32_t _gid) const
{
    const TileMapObject * object = findBasicObject(_gid);
    return object ? dynamic_cast<const T *>(object) : nullptr;
}

} // namespace Sol2D::Tiles
