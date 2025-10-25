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

#include <Sol2D/Xml/XmlException.h>
#include <Sol2D/Def.h>
#include <tinyxml2.h>
#include <filesystem>

namespace Sol2D::Xml {

class XmlLoader
{
    S2_DISABLE_COPY_AND_MOVE(XmlLoader)

public:
    virtual ~XmlLoader() { }

protected:
    explicit XmlLoader(const std::filesystem::path & _path) :
        m_path(_path)
    {
    }

    void loadDocument(tinyxml2::XMLDocument & _xml_document);
    uint32_t readRequiredUintAttribute(const tinyxml2::XMLElement & _xml_element, const char * _attr);
    uint32_t readRequiredPositiveUintAttribute(const tinyxml2::XMLElement & _xml_element, const char * _attr);
    int32_t readRequiredIntAttribute(const tinyxml2::XMLElement & _xml_element, const char * _attr);
    const char * readRequiredAttribute(const tinyxml2::XMLElement & _xml_element, const char * _attr);
    std::string formatFileReadErrorMessage(const std::filesystem::path & _path) const;
    std::string formatXmlRootElemetErrorMessage(const char * _expected) const;

private:
    std::string formatFileReadErrorMessage() const;

protected:
    const std::filesystem::path m_path;
};

} // namespace Sol2D::Xml
