#include <Sol2D/Tiles/TileMap.h>

using namespace Sol2D::Tiles;

TileMap::TileMap(const TileHeap & _tile_heap, const ObjectHeap & _object_heap) :
    TileMapLayerContainer(_tile_heap, _object_heap),
    m_orientation(Orientation::Orthogonal),
    m_render_order(RenderOrder::RightDown),
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0),
    m_tile_width(0),
    m_tile_height(0),
    m_hex_side_length(0),
    m_stagger_axis(Axis::Y),
    m_stagger_index(StaggerIndex::Odd),
    m_parallax_origin_x(0),
    m_parallax_origin_y(0),
    m_background_color{ 0, 0, 0, 0 }
{
}

void TileMap::expand(int _x, int _y, uint32_t _width, uint32_t _height)
{
    int32_t delta_x = m_x - _x;
    if(delta_x > 0)
    {
        m_x -= delta_x;
        m_width += delta_x;
    }
    int32_t delta_y = m_y - _y;
    if(delta_y > 0)
    {
        m_y -= delta_y;
        m_height += delta_y;
    }
    if(m_width < _width)
        m_width = _width;
    if(m_height < _height)
        m_height = _height;
}
