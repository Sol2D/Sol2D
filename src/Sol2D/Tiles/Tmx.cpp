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

#include <Sol2D/Tiles/Tmx.h>
#include <Sol2D/Utils/Zlib.h>
#include <Sol2D/Utils/Zstd.h>
#include <Sol2D/Utils/Base64.h>
#include <Sol2D/Utils/String.h>
#include <Sol2D/Def.h>
#include <SDL3_image/SDL_image.h>
#include <tinyxml2.h>
#include <list>

using namespace Sol2D;
using namespace Sol2D::SDL;
using namespace Sol2D::Tiles;
using namespace Sol2D::Utils;
using namespace tinyxml2;

namespace fs = std::filesystem;

namespace {

struct TileMapTileDescription
{
    int32_t x;
    int32_t y;
    uint32_t gid;
};

class TileMapLayerData final
{
public:
    TileMapLayerData(uint32_t _layer_id, const std::string & _layer_name);
    void startChunk(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height);
    void addTile(int32_t _x, int32_t _y, uint32_t _gid);
    TileMapTileLayer * createLayer(TileMapLayerContainer & _container, uint32_t _tile_width, uint32_t _tile_height);

private:
    uint32_t m_layer_id;
    const std::string m_layer_name;
    std::list<TileMapTileDescription> m_tiles;
    int32_t m_x;
    int32_t m_y;
    int32_t m_last_x;
    int32_t m_last_y;
};

struct TileMapLayerDefinition
{
    uint32_t id;
    const char * name;
};

class XmlLoader
{
    S2_DISABLE_COPY_AND_MOVE(XmlLoader)

protected:
    XmlLoader(SDL_Renderer & _renderer, TileHeap & _heap,
              ObjectHeap & _object_heap, TileMap & _map, const fs::path & _path);
    uint32_t readRequiredUintAttribute(const XMLElement & _xml_element, const char * _attr);
    uint32_t readRequiredPositiveUintAttribute(const XMLElement & _xml_element, const char * _attr);
    int32_t readRequiredIntAttribute(const XMLElement & _xml_element, const char * _attr);
    const char * readRequiredAttribute(const XMLElement & _xml_element, const char * _attr);
    std::string formatFileReadErrorMessage() const;
    std::string formatFileReadErrorMessage(const fs::path & _path) const;
    std::string formatXmlRootElemetErrorMessage(const char * _expected) const;
    bool tryParseColor(const char * _value, SDL_Color & _color) const;
    TexturePtr parseImage(const XMLElement & _xml);

protected:
    SDL_Renderer & mr_renderer;
    TileHeap & mr_tile_heap;
    ObjectHeap & mr_object_heap;
    TileMap & mr_map;
    const fs::path & mr_path;
};

class TileMapXmlLoader : private XmlLoader
{
public:
    TileMapXmlLoader(
        SDL_Renderer & _renderer,
        TileHeap & _tile_heap,
        ObjectHeap & _object_heap,
        TileMap & _map,
        const Workspace & _workspace,
        const fs::path & _path);
    void loadFromFile();

private:
    void loadTileSet(const XMLElement & _xml);
    void loadLayers(const XMLElement & _xml, TileMapLayerContainer & _container);
    void loadTileLayer(const XMLElement & _xml, TileMapLayerContainer & _container);
    TileMapLayerDefinition readLayerDefinition(const XMLElement & _xml);
    void readLayer(const XMLElement & _xml, TileMapLayer & _layer);
    void loadTileLayerChunk(const XMLElement & _xml, const char *_encoding, const char *_compression,
                            int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, TileMapLayerData &_data);
    void loadTileLayerDataUnencoded(const XMLElement & _xml, int32_t _x, int32_t _y,
                                    uint32_t _width, uint32_t _height, TileMapLayerData &_data);
    void loadTileLayerDataBase64(const XMLElement & _xml, const char *_compression, int32_t _x, int32_t _y,
                                 uint32_t _width, uint32_t _height, TileMapLayerData &_data);
    void loadTileLayerDataCsv(const XMLElement & _xml, int32_t _x, int32_t _y,
                              uint32_t _width, uint32_t _height, TileMapLayerData &_data);
    void loadObjectLayer(const XMLElement & _xml, TileMapLayerContainer & _container);
    void loadObject(const XMLElement & _xml, TileMapObjectLayer & _layer);
    void loadPoints(const XMLElement & _xml, TileMapPolyX & _poly);
    void loadText(const XMLElement & _xml, TileMapText & _text);
    void loadImageLayer(const XMLElement & _xml, TileMapLayerContainer & _container);
    void loadGroupLayer(const XMLElement & _xml, TileMapLayerContainer & _container);

private:
    void loadFromXml(const XMLDocument & _xml);

private:
    const Workspace & mr_workspace;
    bool m_is_map_infinite;
};


class TileSetXmlLoader : private XmlLoader
{
public:
    TileSetXmlLoader(SDL_Renderer & _renderer,
                     TileHeap & _tile_heap,
                     ObjectHeap & _object_heap,
                     TileMap & _map,
                     const fs::path & _path);
    void loadFromFile(uint32_t _first_gid);
    void loadFromXml(const XMLElement & _xml, uint32_t _first_gid);

private:
    void makeTiles(TexturePtr _texture, TileSet & _set, uint32_t _first_gid, uint32_t _tile_width,
                   uint32_t _tile_height, uint32_t _spacing, uint32_t _margin);

public:
    static const char * sc_root_tag_name;
};

} // namespace

inline TileMapLayerData::TileMapLayerData(uint32_t _layer_id, const std::string & _layer_name) :
    m_layer_id(_layer_id),
    m_layer_name(_layer_name),
    m_x(0),
    m_y(0),
    m_last_x(0),
    m_last_y(0)
{
}

void TileMapLayerData::startChunk(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height)
{
    if(m_tiles.empty())
    {
        m_x = _x;
        m_y = _y;
        m_last_x = _x + _width;
        m_last_y = _y + _height;
    }
    else
    {
        if(_x < m_x) m_x = _x;
        if(_y < m_y) m_y = _y;
        int32_t last_x = _x + _width;
        if(last_x > m_last_x) m_last_x = last_x;
        int32_t last_y = _y + _height;
        if(last_y > m_last_y) m_last_y = last_y;
    }
}

inline void TileMapLayerData::addTile(int32_t _x, int32_t _y, uint32_t _gid)
{
    m_tiles.push_back({ .x = _x, .y = _y, .gid = _gid });
}

TileMapTileLayer * TileMapLayerData::createLayer(TileMapLayerContainer & _container,
                                                uint32_t _tile_width,
                                                uint32_t _tile_height)
{
    int32_t width = m_last_x - m_x;
    int32_t height = m_last_y - m_y;
    if(width <= 0 || height <= 0)
        return nullptr;
    TileMapTileLayer & layer = _container.createTileLayer(
        m_layer_id,
        m_layer_name,
        _tile_width,
        _tile_height,
        m_x,
        m_y,
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    );
    for(const TileMapTileDescription & tile : m_tiles)
        layer.setTile(tile.x, tile.y, tile.gid);
    return &layer;
}

inline XmlLoader::XmlLoader(SDL_Renderer & _renderer,
                            TileHeap & _tile_heap,
                            ObjectHeap & _object_heap,
                            TileMap &_map,
                            const fs::path & _path) :
    mr_renderer(_renderer),
    mr_tile_heap(_tile_heap),
    mr_object_heap(_object_heap),
    mr_map(_map),
    mr_path(_path)
{
}

uint32_t XmlLoader::readRequiredUintAttribute(const XMLElement & _xml_element, const char * _attr)
{
    int int_val = readRequiredIntAttribute(_xml_element, _attr);
    if(int_val < 0)
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" of the tag \"" <<
            _xml_element.Name() << "\" in the XML file " << mr_path << " must be a non-negative integer";
        throw TiledXmlException(ss.str());
    }
    return static_cast<uint32_t>(int_val);
}

uint32_t XmlLoader::readRequiredPositiveUintAttribute(const XMLElement & _xml_element, const char * _attr)
{
    int int_val = readRequiredIntAttribute(_xml_element, _attr);
    if(int_val <= 0)
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" of the tag \"" <<
            _xml_element.Name() << "\" in the XML file " << mr_path << " must be a positive integer";
        throw TiledXmlException(ss.str());
    }
    return static_cast<uint32_t>(int_val);
}

int32_t XmlLoader::readRequiredIntAttribute(const XMLElement & _xml_element, const char * _attr)
{
    const char * value = readRequiredAttribute(_xml_element, _attr);
    size_t len = strlen(value);
    char * end;
    long int_val = std::strtol(value, &end, 10);
    if(end != (value + len))
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" of the tag \"" <<
            _xml_element.Name() << "\" in the XML file " << mr_path << " must be an integer";
        throw TiledXmlException(ss.str());
    }
    return static_cast<int32_t>(int_val);
}

const char * XmlLoader::readRequiredAttribute(const XMLElement & _xml_element, const char * _attr)
{
    const char * value = _xml_element.Attribute(_attr);
    if(value == nullptr)
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" is required for the tag \"" <<
            _xml_element.Name() << "\" in the XML file " << mr_path;
        throw TiledXmlException(ss.str());
    }
    return value;
}

inline std::string XmlLoader::formatFileReadErrorMessage() const
{
    return formatFileReadErrorMessage(mr_path.c_str());
}

std::string XmlLoader::formatFileReadErrorMessage(const fs::path & _path) const
{
    std::stringstream ss;
    ss << "Unable to read file: " << _path;
    return ss.str();
}

std::string XmlLoader::formatXmlRootElemetErrorMessage(const char * _expected) const
{
    std::stringstream ss;
    ss << "The root element of the XML file " << mr_path << ": " << " must be \"" << _expected << "\"";
    return ss.str();
}

bool XmlLoader::tryParseColor(const char * _value, SDL_Color & _color) const
{
    if(_value == nullptr)
        return false;

    const char * value = _value;
    size_t len = strlen(value);
    if(len == 0)
        return false;
    if(value[0] == '#')
    {
        value += 1;
        len -= 1;
    }
    if(len != 6 && len != 8)
        return false;
    for(size_t i = 1; i < len; ++i)
    {
        if(!isdigit(value[i]))
            return false;
    }
    char num[3];
    num[2] = '\0';

    size_t i = 0;

    if(len == 8)
    {
        num[0] = value[i++];
        num[1] = value[i++];
        _color.a = atoi(num);
    }
    else
    {
        _color.a = 0;
    }

    num[0] = value[i++];
    num[1] = value[i++];
    _color.r = atoi(num);

    num[0] = value[i++];
    num[1] = value[i++];
    _color.g = atoi(num);

    num[0] = value[i++];
    num[1] = value[i];
    _color.b = atoi(num);

    return true;
}

TexturePtr XmlLoader::parseImage(const XMLElement & _xml)
{
    SDL_Surface * surface = nullptr;
    if(const char * source = _xml.Attribute("source"))
    {
        fs::path path(source);
        if(path.is_relative())
            path = mr_path.parent_path() / path;
        surface = IMG_Load(path.c_str());
        if(surface == nullptr)
            throw IOException(formatFileReadErrorMessage(path));
    }
    else
    {
        // TODO: load <data>
        throw NotSupportedException("Inline images are not supported yet");
    }
    if(const char * trans = _xml.Attribute("trans"))
    {
        SDL_Color color;
        if(tryParseColor(trans, color))
        {
            SDL_SetSurfaceColorKey(
                surface,
                SDL_TRUE,
                SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a)
            );
        }
    }
    SDL_Texture * texture = SDL_CreateTextureFromSurface(&mr_renderer, surface);
    SDL_DestroySurface(surface);
    return wrapSdlTexturePtr(texture);
}

inline TileMapXmlLoader::TileMapXmlLoader(SDL_Renderer & _renderer,
                                          TileHeap & _tile_heap,
                                          ObjectHeap & _object_heap,
                                          TileMap & _map,
                                          const Workspace & _workspace,
                                          const fs::path & _path) :
    XmlLoader(_renderer, _tile_heap, _object_heap, _map, _path),
    mr_workspace(_workspace),
    m_is_map_infinite(false)
{
}

void TileMapXmlLoader::loadFromFile()
{
    XMLDocument xml;
    if(xml.LoadFile(mr_path.c_str()) != XML_SUCCESS)
        throw IOException(formatFileReadErrorMessage());
    loadFromXml(xml);
}

void TileMapXmlLoader::loadFromXml(const XMLDocument & _xml)
{
    static const char * tag_name_map = "map";
    const XMLElement * xmap = _xml.RootElement();
    if(strcmp(tag_name_map, xmap->Name()) != 0)
        throw TiledXmlException(formatXmlRootElemetErrorMessage(tag_name_map));
    m_is_map_infinite = xmap->BoolAttribute("infinite");
    mr_map.setClass(xmap->Attribute("class"));
    mr_map.setWidth(xmap->UnsignedAttribute("width"));
    mr_map.setHeight(xmap->UnsignedAttribute("height"));
    mr_map.setTileWidth(xmap->UnsignedAttribute("tilewidth"));
    mr_map.setTileHeight(xmap->UnsignedAttribute("tileheight"));
    mr_map.setHexSideLength(xmap->UnsignedAttribute("hexsidelength"));
    mr_map.setParallaxOriginX(xmap->IntAttribute("parallaxoriginx"));
    mr_map.setParallaxOriginY(xmap->IntAttribute("parallaxoriginy"));
    if(const char * render_order = xmap->Attribute("renderorder"))
    {
        if(strcmp("right-down", render_order) == 0)
            mr_map.setRenderOrder(TileMap::RenderOrder::RightDown);
        else if(strcmp("right-up", render_order) == 0)
            mr_map.setRenderOrder(TileMap::RenderOrder::RightUp);
        else if(strcmp("left-down", render_order) == 0)
            mr_map.setRenderOrder(TileMap::RenderOrder::LeftDown);
        else if(strcmp("left-up", render_order) == 0)
            mr_map.setRenderOrder(TileMap::RenderOrder::LeftUp);
    }
    if(const char * stagger_axis = xmap->Attribute("staggeraxis"))
    {
        if(strcmp("x", stagger_axis))
            mr_map.setStaggerAxis(TileMap::Axis::X);
        else if(strcmp("y", stagger_axis))
            mr_map.setStaggerAxis(TileMap::Axis::Y);
    }
    if(const char * stagger_index = xmap->Attribute("staggerindex"))
    {
        if(strcmp("even", stagger_index) == 0)
            mr_map.setStaggerIndex(TileMap::StaggerIndex::Even);
        else if(strcmp("odd", stagger_index) == 0)
            mr_map.setStaggerIndex(TileMap::StaggerIndex::Odd);
    }
    {
        SDL_Color color;
        if(tryParseColor(xmap->Attribute("backgroundcolor"), color))
            mr_map.setBackgroundColor(color);
    }
    for(const XMLElement * xts = xmap->FirstChildElement("tileset"); xts; xts = xts->NextSiblingElement(xts->Name()))
    {
        loadTileSet(*xts);
    }
    loadLayers(*xmap, mr_map);
}

void TileMapXmlLoader::loadTileSet(const XMLElement & _xml)
{
    uint32_t first_gid = readRequiredUintAttribute(_xml, "firstgid");
    const char * source = _xml.Attribute("source");
    if(source)
    {
        fs::path source_path(source);
        if(!source_path.is_absolute())
            source_path = mr_path.parent_path() / source_path;
        TileSetXmlLoader loader(mr_renderer, mr_tile_heap, mr_object_heap, mr_map, source_path);
        loader.loadFromFile(first_gid);
    }
    else
    {
        TileSetXmlLoader loader(mr_renderer, mr_tile_heap, mr_object_heap, mr_map, mr_path);
        loader.loadFromXml(_xml, first_gid);
    }
}

void TileMapXmlLoader::loadLayers(const XMLElement & _xml, TileMapLayerContainer & _container)
{
    for(const XMLElement * xchild = _xml.FirstChildElement(); xchild; xchild = xchild->NextSiblingElement())
    {
        const char * name = xchild->Name();
        if(strcmp("layer", name) == 0)
            loadTileLayer(*xchild, _container);
        else if(strcmp("objectgroup", name) == 0)
            loadObjectLayer(*xchild, _container);
        else if(strcmp("imagelayer", name) == 0)
            loadImageLayer(*xchild, _container);
        else if(strcmp("group", name) == 0)
            loadGroupLayer(*xchild, _container);
    }
}

void TileMapXmlLoader::loadTileLayer(const XMLElement & _xml, TileMapLayerContainer & _container)
{
    TileMapLayerDefinition def = readLayerDefinition(_xml);
    std::unique_ptr<TileMapLayerData> data(new TileMapLayerData(def.id, def.name));
    if(const XMLElement * xdata = _xml.FirstChildElement("data"))
    {
        const char * encoding = xdata->Attribute("encoding");
        const char * compression = xdata->Attribute("compression");
        static const char * width_attr = "width";
        static const char * height_attr = "height";
        if(m_is_map_infinite)
        {
            for(const XMLElement * xchunk = xdata->FirstChildElement("chunk");
                 xchunk;
                 xchunk = xchunk->NextSiblingElement(xchunk->Name()))
            {
                int32_t x = readRequiredIntAttribute(*xchunk, "x");
                int32_t y = readRequiredIntAttribute(*xchunk, "y");
                uint32_t width = readRequiredPositiveUintAttribute(*xchunk, width_attr);
                uint32_t height = readRequiredPositiveUintAttribute(*xchunk, height_attr);
                data->startChunk(x, y, width, height);
                loadTileLayerChunk(*xchunk, encoding, compression, x, y, width, height, *data);
            }
        }
        else
        {
            int32_t x = 0;
            int32_t y = 0;
            uint32_t width = readRequiredPositiveUintAttribute(_xml, width_attr);
            uint32_t height = readRequiredPositiveUintAttribute(_xml, height_attr);
            data->startChunk(x, y, width, height);
            loadTileLayerChunk(*xdata, encoding, compression, x, y, width, height, *data);
        }
    }
    TileMapTileLayer * layer = data->createLayer(_container, mr_map.getTileWidth(), mr_map.getTileHeight());
    if(layer)
    {
        readLayer(_xml, *layer);
        mr_map.expand(layer->getX(), layer->getY(), layer->getWidth(), layer->getHeight());
    }
}

inline TileMapLayerDefinition TileMapXmlLoader::readLayerDefinition(const XMLElement & _xml)
{
    return {
        .id = readRequiredUintAttribute(_xml, "id"),
        .name = readRequiredAttribute(_xml, "name")
    };
}

void TileMapXmlLoader::readLayer(const XMLElement & _xml, TileMapLayer & _layer)
{
    _layer.setVisibility(_xml.BoolAttribute("visible", true));
    _layer.setClass(_xml.Attribute("class"));
    if(float opacity = _xml.FloatAttribute("opacity", 1.0f) != 1.0f)
        _layer.setOpacity(opacity);
    if(float parallax_x = _xml.FloatAttribute("parallaxx", 1.0f) != 1.0f)
        _layer.setParallaxX(parallax_x);
    if(float parallax_y = _xml.FloatAttribute("parallaxy", 1.0f) != 1.0f)
        _layer.setParallaxY(parallax_y);
    if(int32_t offset_x = _xml.IntAttribute("offsetx", 0))
        _layer.setOffsetX(offset_x);
    if(int32_t offset_y = _xml.IntAttribute("offsety", 0))
        _layer.setOffsetY(offset_y);
    SDL_Color tint_color;
    if(tryParseColor(_xml.Attribute("tintcolor"), tint_color))
        _layer.setTintColor(tint_color);
}

void TileMapXmlLoader::loadTileLayerChunk(const XMLElement & _xml,
                                          const char * _encoding,
                                          const char * _compression,
                                          int32_t _x,
                                          int32_t _y,
                                          uint32_t _width,
                                          uint32_t _height,
                                          TileMapLayerData & _data)
{
    if(_encoding == nullptr)
    {
        loadTileLayerDataUnencoded(_xml, _x, _y, _width, _height, _data);
    }
    else if(strcmp("base64", _encoding) == 0)
    {
        loadTileLayerDataBase64(_xml, _compression, _x, _y, _width, _height, _data);
    }
    else if(strcmp("csv", _encoding) == 0)
    {
        loadTileLayerDataCsv(_xml, _x, _y, _width, _height, _data);
    }
    else
    {
        std::stringstream ss;
        ss << "Unknown encoding type: \"" << _encoding << "\"";
        throw TiledXmlException(ss.str());
    }
}

void TileMapXmlLoader::loadTileLayerDataUnencoded(const XMLElement & _xml, int32_t _x, int32_t _y,
                                                  uint32_t _width, uint32_t _height, TileMapLayerData & _data)
{
    const int32_t last_x = _x + _width;
    const int32_t last_y = _y + _height;
    const XMLElement * xtile = _xml.FirstChildElement("tile");
    int32_t loaded_rows = 0;
    for(int32_t y = _y; loaded_rows < last_y; ++y)
    {
        for(int32_t x = _x; x < last_x && xtile; ++x)
        {
            uint32_t tile_gid = xtile->UnsignedAttribute("gid", UINT32_MAX);
            if(tile_gid != UINT32_MAX)
            {
                // tile_gid & 0x1FFFFFFF; // TODO: flip bit
                _data.addTile(x, y, tile_gid);
            }
            xtile = xtile->NextSiblingElement(xtile->Name());
            ++loaded_rows;
        }
    }
}

void TileMapXmlLoader::loadTileLayerDataBase64(const XMLElement & _xml,
                                               const char * _compression,
                                               int32_t _x,
                                               int32_t _y,
                                               uint32_t _width,
                                               uint32_t _height,
                                               TileMapLayerData & _data)
{

    std::shared_ptr<std::vector<uint8_t>> data = decodeBase64(trimString(_xml.GetText()));
    if(data == nullptr)
    {
        throw TiledXmlException("Unable to decode base64-encoded data");
    }
    if(_compression != nullptr)
    {
        if(strcmp("zlib", _compression) == 0)
        {
            data = zlibDecompress(ZlibAlgorithm::Zlib, *data);
        }
        else if(strcmp("gzip", _compression) == 0)
        {
            data = zlibDecompress(ZlibAlgorithm::GZip, *data);
        }
        else if(strcmp("zstd", _compression) == 0)
        {
            data = zstdDecompress(*data);
        }
        else
        {
            std::stringstream ss;
            ss << "Unknown compression type: \"" << _compression << "\"";
            throw TiledXmlException(ss.str());
        }
    }
    if(data) // TODO: error handling?
    {
        const int32_t last_x = _x + _width;
        const int32_t last_y = _y + _height;
        const uint32_t * gids = reinterpret_cast<const uint32_t *>(data->data());
        size_t gid_count = data->size() / sizeof(uint32_t);
        uint32_t loaded_gids = 0;
        for(int32_t y = _y; y < last_y; ++y)
        {
            for(int32_t x = _x; x < last_x; ++x)
            {
                if(loaded_gids >= gid_count) return;
                uint32_t tile_gid = gids[loaded_gids];// & 0x1FFFFFFF; // TODO: flip bit
                if(tile_gid) _data.addTile(x, y, tile_gid);
                ++loaded_gids;
            }
        }
    }
}

void TileMapXmlLoader::loadTileLayerDataCsv(const XMLElement & _xml,
                                            int32_t _x,
                                            int32_t _y,
                                            uint32_t _width,
                                            uint32_t _height,
                                            TileMapLayerData & _data)
{
    std::string csv = trimString(_xml.GetText());
    char * position = csv.data();
    const char * csv_end = position + csv.size() - 1;
    const int32_t last_x = _x + _width;
    const int32_t last_y = _y + _height;
    for(int32_t y = _y, i = 0; y < last_y; ++y)
    {
        for(int32_t x = _x; x < last_x; ++x, ++i)
        {
            if(position >= csv_end) return;
            uint32_t tile_gid = std::strtoul(position, &position, 10); // & 0x1FFFFFFF; // TODO: flip bit
            if(tile_gid) _data.addTile(x, y, tile_gid);
            ++position;
        }
    }
}

void TileMapXmlLoader::loadObjectLayer(const XMLElement & _xml, TileMapLayerContainer & _container)
{
    TileMapLayerDefinition def = readLayerDefinition(_xml);
    TileMapObjectLayer & layer = _container.createObjectLayer(def.id, def.name);
    readLayer(_xml, layer);
    for(const XMLElement * xobj = _xml.FirstChildElement("object"); xobj; xobj = xobj->NextSiblingElement(xobj->Name()))
        loadObject(*xobj, layer);
}

void TileMapXmlLoader::loadObject(const XMLElement & _xml, TileMapObjectLayer & _layer)
{
    if(_xml.Attribute("template"))
    {
        // TODO: load from the template
        throw NotSupportedException("Object loading from a template is not supported yet");
    }
    const uint32_t id = readRequiredUintAttribute(_xml, "id");
    const float width = _xml.FloatAttribute("width");
    const float height = _xml.FloatAttribute("height");
    const float rotation = _xml.FloatAttribute("rotation");
    float x = _xml.FloatAttribute("x");
    float y = _xml.FloatAttribute("y");
    const char * cls = _xml.Attribute("type");
    if(!cls) cls = _xml.Attribute("class");
    const char * name = _xml.Attribute("name");
    TileMapObject * object = nullptr;
    TileMapObjectWithWidthAndHeight * object_with_width_and_height = nullptr;

    if(_xml.FirstChildElement("ellipse"))
    {
        if(width == height)
        {
            TileMapCircle & circle = mr_object_heap.createObject<TileMapCircle>(_layer.getId(), id, cls, name);
            float radius = width / 2;
            circle.setRadius(radius);
            object = &circle;
            x += radius;
            y += radius;
        }
        else
        {
            mr_workspace.getMainLogger().warn("Ellipses are not supported. Loading TMX object {} skipped.", id);
            return;
        }
    }
    else if(_xml.FirstChildElement("point"))
    {
        object = &mr_object_heap.createObject<TileMapPoint>(_layer.getId(), id, cls, name);
    }
    else if(const XMLElement * spec = _xml.FirstChildElement("polygon"))
    {
        TileMapPolygon & polygon = mr_object_heap.createObject<TileMapPolygon>(_layer.getId(), id, cls, name);
        object = &polygon;
        object_with_width_and_height = &polygon;
        loadPoints(*spec, polygon);
        if(rotation) polygon.rotate(degreesToRadians(rotation));
    }
    else if(const XMLElement * spec = _xml.FirstChildElement("polyline"))
    {
        TileMapPolyline & polyline = mr_object_heap.createObject<TileMapPolyline>(_layer.getId(), id, cls, name);
        object = &polyline;
        object_with_width_and_height = &polyline;
        loadPoints(*spec, polyline);
        if(rotation) polyline.rotate(degreesToRadians(rotation));
    }
    else if(const XMLElement * spec = _xml.FirstChildElement("text"))
    {
        TileMapText & text = mr_object_heap.createObject<TileMapText>(_layer.getId(), id, cls, name);
        object = &text;
        object_with_width_and_height = &text;
        loadText(*spec, text);
    }
    else
    {
        TileMapPolygon & polygon = mr_object_heap.createObject<TileMapPolygon>(_layer.getId(), id, cls, name);
        polygon.addPoint({ .0f, .0f });
        polygon.addPoint({ width, 0.0f });
        polygon.addPoint({ width, height });
        polygon.addPoint({ .0f, height });
        object = &polygon;
        object_with_width_and_height = &polygon;
        if(rotation) polygon.rotate(degreesToRadians(rotation));
    }
    object->setX(x);
    object->setY(y);
    object->setVisibility(_xml.BoolAttribute("visible", true));
    if(object_with_width_and_height)
    {
        object_with_width_and_height->setWidth(width);
        object_with_width_and_height->setHeight(height);
    }
    uint32_t gid = _xml.UnsignedAttribute("gid", UINT32_MAX);
    if(gid != UINT32_MAX)object->setTileGid(gid);
}

void TileMapXmlLoader::loadPoints(const XMLElement & _xml, TileMapPolyX & _poly)
{
    const char * points = readRequiredAttribute(_xml, "points");
    if(!points) return;
    std::stringstream ss(points);
    std::string point_str;
    size_t del_pos;
    while(ss >> point_str)
    {
        del_pos = point_str.find(',', 0); // TODO: remove "find", use the 2nd arg of "strtol" (see loadTileLayerDataCsv)
        if(del_pos != std::string::npos)
        {            
            SDL_FPoint point;
            point.x = std::strtof(point_str.c_str(), nullptr);
            point.y = std::strtof(&point_str.c_str()[del_pos + 1], nullptr);
            _poly.addPoint(point);
        }
    }
}

void TileMapXmlLoader::loadText(const XMLElement & _xml, TileMapText & _text)
{
    if(const char * font = _xml.Attribute("fontfamily"))
        _text.setFontFamily(font);
    if(uint32_t font_size = _xml.UnsignedAttribute("pixelsize", UINT32_MAX) != UINT32_MAX)
        _text.setFontSize(static_cast<uint16_t>(font_size));
    if(const char * color_str = _xml.Attribute("color"))
    {
        SDL_Color color;
        if(tryParseColor(color_str, color))
            _text.setColor(color);
    }
    _text.setBold(_xml.BoolAttribute("bold", false));
    _text.setItalic(_xml.BoolAttribute("italic", false));
    _text.setUnderlined(_xml.BoolAttribute("underline", false));
    _text.setStruckOut(_xml.BoolAttribute("strikeout", false));
    _text.enableKerning(_xml.BoolAttribute("kerning", true));
    if(const char * halign = _xml.Attribute("halign"))
    {
        if(strcmp("left", halign) == 0)
            _text.setHorizontalAlignment(TileMapText::HAlignment::Center);
        else if(strcmp("center", halign) == 0)
            _text.setHorizontalAlignment(TileMapText::HAlignment::Center);
        else if(strcmp("right", halign) == 0)
            _text.setHorizontalAlignment(TileMapText::HAlignment::Right);
        else if(strcmp("justify", halign) == 0)
            _text.setHorizontalAlignment(TileMapText::HAlignment::Justify);
    }
    if(const char * valign = _xml.Attribute("valign"))
    {
        if(strcmp("top", valign) == 0)
            _text.setVerticalAlignment(TileMapText::VAlignment::Top);
        else if(strcmp("center", valign) == 0)
            _text.setVerticalAlignment(TileMapText::VAlignment::Center);
        else if(strcmp("bottom", valign) == 0)
            _text.setVerticalAlignment(TileMapText::VAlignment::Bottom);
    }
}

void TileMapXmlLoader::loadImageLayer(const XMLElement & _xml, TileMapLayerContainer & _container)
{
    TileMapLayerDefinition def = readLayerDefinition(_xml);
    TileMapImageLayer & layer = _container.createImageLayer(def.id, def.name);
    readLayer(_xml, layer);
    const XMLElement * ximage = _xml.FirstChildElement("image");
    if(ximage)
        layer.setImage(parseImage(*ximage));
}

void TileMapXmlLoader::loadGroupLayer(const XMLElement & _xml, TileMapLayerContainer & _container)
{
    TileMapLayerDefinition def = readLayerDefinition(_xml);
    TileMapGroupLayer & layer = _container.createGroupLayer(def.id, def.name);
    readLayer(_xml, layer);
    loadLayers(_xml, layer);
}

const char * TileSetXmlLoader::sc_root_tag_name = "tileset";

inline TileSetXmlLoader::TileSetXmlLoader(SDL_Renderer & _renderer,
                                          TileHeap & _tile_heap,
                                          ObjectHeap & _object_heap,
                                          TileMap & _map,
                                          const fs::path & _path) :
    XmlLoader(_renderer, _tile_heap, _object_heap, _map, _path)
{
}

void TileSetXmlLoader::loadFromFile(uint32_t _first_gid)
{
    XMLDocument xml;
    if(xml.LoadFile(mr_path.c_str()) != XML_SUCCESS)
        throw IOException(formatFileReadErrorMessage());
    const XMLElement * xml_root = xml.RootElement();
    if(strcmp(sc_root_tag_name, xml_root->Name()) != 0)
        throw TiledXmlException(formatXmlRootElemetErrorMessage(sc_root_tag_name));
    loadFromXml(*xml_root, _first_gid);
}

void TileSetXmlLoader::loadFromXml(const XMLElement & _xml, uint32_t _first_gid)
{
    uint32_t tile_width = readRequiredPositiveUintAttribute(_xml, "tilewidth");
    uint32_t tile_height = readRequiredPositiveUintAttribute(_xml, "tileheight");
    uint32_t spacing = _xml.UnsignedAttribute("spacing");
    uint32_t margin = _xml.UnsignedAttribute("margin");

    TileSet & set = mr_tile_heap.createTileSet();
    set.setName(_xml.Attribute("name"));
    set.setClass(_xml.Attribute("class"));
    set.setTileWidth(tile_width);
    set.setTileHeight(tile_height);
    if(const char * object_alignment = _xml.Attribute("objectalignment"))
    {
        if(strcmp("unspecified", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::Unspecified);
        else if(strcmp("topleft", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::TopLeft);
        else if(strcmp("top", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::Top);
        else if(strcmp("topright", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::TopRight);
        else if(strcmp("left", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::Left);
        else if(strcmp("center", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::Center);
        else if(strcmp("right", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::Right);
        else if(strcmp("bottomleft", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::BottomLeft);
        else if(strcmp("bottom", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::Bottom);
        else if(strcmp("bottomright", object_alignment) == 0)
            set.setObjectAlignment(TileSet::ObjectAlignment::BottomRight);
    }
    if(const char * tile_render_size = _xml.Attribute("tilerendersize"))
    {
        if(strcmp("grid", tile_render_size) == 0)
            set.setTileRenderSize(TileSet::TileRenderSize::Grid);
        else if(strcmp("tile", tile_render_size) == 0)
            set.setTileRenderSize(TileSet::TileRenderSize::Tile);
    }
    if(const char * fill_mode = _xml.Attribute("fillmode"))
    {
        if(strcmp("preserve-aspect-fit", fill_mode) == 0)
            set.setFillMode(TileSet::FillMode::PreserveAspectFit);
        else if(strcmp("stretch", fill_mode) == 0)
            set.setFillMode(TileSet::FillMode::Stretch);
    }

    if(const XMLElement * xml_image = _xml.FirstChildElement("image"))
    {
        TexturePtr texture = parseImage(*xml_image);
        makeTiles(texture, set, _first_gid, tile_width, tile_height, spacing, margin);
    }
    else
    {
        // TODO: load tiles
    }

    // TODO: <tileoffset>
    // TODO: <grid>
    // TODO: <properties>
    // TODO: <terraintypes>
    // TODO: <wangsets>
    // TODO: <transformations>
}

void TileSetXmlLoader::makeTiles(TexturePtr _texture,
                                 TileSet & _set,
                                 uint32_t _first_gid,
                                 uint32_t _tile_width,
                                 uint32_t _tile_height,
                                 uint32_t _spacing,
                                 uint32_t _margin)
{
    int width, height;
    SDL_QueryTexture(_texture.get(), nullptr, nullptr, &width, &height);
    int max_x = width - _margin - _tile_width;
    int max_y = height - _margin - _tile_height;
    uint32_t gid = _first_gid;
    for(int y = _margin; y <= max_y; y += _spacing + _tile_height)
    {
        for(int x = _margin; x <= max_x; x += _spacing + _tile_width)
        {
            mr_tile_heap.createTile(gid++, _set, _texture, x, y, _tile_width, _tile_height);
        }
    }
}

Tmx Sol2D::Tiles::loadTmx(SDL_Renderer & _renderer, const Workspace & _workspace, const std::filesystem::path & _path)
{
    std::unique_ptr<TileHeap> tile_heap(new TileHeap);
    std::unique_ptr<ObjectHeap> object_heap(new ObjectHeap);
    std::unique_ptr<TileMap> map(new TileMap(*tile_heap, *object_heap));
    Tmx tmx(std::move(tile_heap), std::move(object_heap), std::move(map));
    TileMapXmlLoader loader(_renderer, *tmx.tile_heap, *tmx.object_heap, *tmx.tile_map, _workspace, _path);
    loader.loadFromFile();
    return tmx;
}
