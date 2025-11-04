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

#include <Sol2D/Xml/XmlLoader.h>
#include <Sol2D/Utils/Color.h>
#include <Sol2D/SpriteSheet.h>
#include <boost/container/slist.hpp>

using namespace Sol2D;
using namespace Sol2D::Utils;
using namespace tinyxml2;

namespace {

class AtlasXmlLoader : public Xml::XmlLoader
{
public:
    explicit AtlasXmlLoader(const std::filesystem::path & _path);
    bool load();
    const std::string & getTextureName() const;
    const boost::container::slist<SpriteSheetFrame> & getFrames() const;
    std::optional<SDL_Color> getColorToAlpha() const;

private:
    std::string m_texture_name;
    std::optional<SDL_Color> m_color_to_alpha;
    boost::container::slist<SpriteSheetFrame> m_frames;
};

} // namespace

AtlasXmlLoader::AtlasXmlLoader(const std::filesystem::path & _path) :
    Xml::XmlLoader(_path)
{
}

bool AtlasXmlLoader::load()
{
    const char * const tag_atlas = "atlas";
    const char * const tag_frame = "frame";
    const char * const attr_alpha = "alpha";

    XMLDocument xml;
    loadDocument(xml);
    const XMLElement * xml_root = xml.RootElement();
    if(strcmp(tag_atlas, xml_root->Name()) != 0)
        throw Xml::XmlException(formatXmlRootElemetErrorMessage(tag_atlas));
    {
        uint32_t version = readRequiredPositiveUintAttribute(*xml_root, "version");
        if(version != 1)
        {
            throw InvalidOperationException(
                std::format("Unsupported atlas version: {}. Latest supported version is {}", version, 1));
        }
    }

    m_texture_name = readRequiredAttribute(*xml_root, "texture");
    m_frames.clear();

    if(const char * alpha = xml_root->Attribute(attr_alpha))
    {
        m_color_to_alpha = tryParseHexColorString(alpha);
        if(!m_color_to_alpha.has_value())
            throw Xml::XmlException(std::format(
                "The \"{}\" attribute of the \"{}\" tag contains an invalid color value",
                attr_alpha,
                tag_atlas));
    }

    for(
        const XMLElement * xml_frame = xml_root->FirstChildElement(tag_frame);
        xml_frame;
        xml_frame = xml_frame->NextSiblingElement(tag_frame))
    {
        SpriteSheetFrame frame
        {
            .texture_rect =
            {
                .x = static_cast<float>(readRequiredIntAttribute(*xml_frame, "tx")),
                .y = static_cast<float>(readRequiredIntAttribute(*xml_frame, "ty")),
                .w = static_cast<float>(readRequiredIntAttribute(*xml_frame, "tw")),
                .h = static_cast<float>(readRequiredIntAttribute(*xml_frame, "th"))
            },
            .sprite_size = FSize(
                static_cast<float>(xml_frame->IntAttribute("sw")),
                static_cast<float>(xml_frame->IntAttribute("sh"))),
            .sprite_point =
            {
                .x = static_cast<float>(xml_frame->IntAttribute("sx")),
                .y = static_cast<float>(xml_frame->IntAttribute("sy")),
            },
            .is_rotated = xml_frame->BoolAttribute("rotated")
        };
        if(frame.sprite_size.w == .0f)
            frame.sprite_size.w = frame.texture_rect.w;
        if(frame.sprite_size.h == .0f)
            frame.sprite_size.h = frame.texture_rect.h;
        m_frames.push_front(frame);
    }

    if(m_frames.empty())
        return false;

    return true;
}

inline const std::string & AtlasXmlLoader::getTextureName() const
{
    return m_texture_name;
}

inline const boost::container::slist<SpriteSheetFrame> & AtlasXmlLoader::getFrames() const
{
    return m_frames;
}

inline std::optional<SDL_Color> AtlasXmlLoader::getColorToAlpha() const
{
    return m_color_to_alpha;
}

SpriteSheet::SpriteSheet(Renderer & _renderer) :
    m_renderer(&_renderer)
{
}

bool SpriteSheet::loadFromFile(const std::filesystem::path & _path, const SpriteSheetOptions & _options)
{
    if(!_options.row_count || !_options.col_count || !_options.sprite_width || !_options.sprite_height)
        return false;
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return false;
    if(_options.color_to_alpha.has_value())
    {
        SDL_Color color = toR8G8B8A8_UINT(_options.color_to_alpha.value());
        const SDL_PixelFormatDetails * pixel_format = SDL_GetPixelFormatDetails(surface->format);
        SDL_SetSurfaceColorKey(surface, true, SDL_MapRGBA(pixel_format, nullptr, color.r, color.g, color.b, color.a));
    }
    m_texture = m_renderer->createTexture(
        *surface,
        std::format("Sprite Sheet {}", _path.filename().string()).c_str());
    SDL_DestroySurface(surface);
    SDL_FRect rect
    {
        .x = .0f,
        .y = .0f,
        .w = static_cast<float>(_options.sprite_width),
        .h = static_cast<float>(_options.sprite_height)
    };
    for(uint16_t row = 0; row < _options.row_count; ++row)
    {
        rect.y = _options.margin_top + row * _options.sprite_height + row * _options.vertical_spacing;
        for(uint16_t col = 0; col < _options.col_count; ++col)
        {
            rect.x = _options.margin_left + col * _options.sprite_width + col * _options.horizontal_spacing;
            m_frames.push_back(
                SpriteSheetFrame
                {
                    .texture_rect = rect,
                    .sprite_size = FSize(rect.w, rect.h),
                    .sprite_point = { .x = .0f, .y = .0f },
                    .is_rotated = false
                });
        }
    }
    return true;
}

bool SpriteSheet::loadFromAtlas(const std::filesystem::path & _path)
{
    AtlasXmlLoader loader(_path);
    if(loader.load())
    {
        std::filesystem::path texture_path(loader.getTextureName());
        if(texture_path.is_relative())
            texture_path = _path / texture_path;
        SDL_Surface * surface = IMG_Load(texture_path.c_str());
        if(!surface)
            return false;
        m_texture = m_renderer->createTexture(
            *surface,
            std::format("Atlas {}", texture_path.filename().string()).c_str());
        std::optional<SDL_Color> color_to_alpha = loader.getColorToAlpha();
        if(color_to_alpha.has_value())
        {
            const SDL_PixelFormatDetails * pixel_format = SDL_GetPixelFormatDetails(surface->format);
            SDL_SetSurfaceColorKey(surface, true, SDL_MapRGBA(
                pixel_format,
                nullptr,
                color_to_alpha->r,
                color_to_alpha->g,
                color_to_alpha->b,
                color_to_alpha->a));
        }
        SDL_DestroySurface(surface);
        const auto & frames = loader.getFrames();
        m_frames.clear();
        m_frames.assign(frames.begin(), frames.end());
        return true;
    }
    return false;
}
