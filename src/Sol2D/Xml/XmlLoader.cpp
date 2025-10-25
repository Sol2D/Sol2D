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

using namespace tinyxml2;
using namespace Sol2D::Xml;

void XmlLoader::loadDocument(XMLDocument & _xml_document)
{
    if(_xml_document.LoadFile(m_path.c_str()) != XML_SUCCESS)
        throw IOException(formatFileReadErrorMessage());
}

uint32_t XmlLoader::readRequiredUintAttribute(const XMLElement & _xml_element, const char * _attr)
{
    int int_val = readRequiredIntAttribute(_xml_element, _attr);
    if(int_val < 0)
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" of the tag \"" << _xml_element.Name() << "\" in the XML file "
           << m_path << " must be a non-negative integer";
        throw XmlException(ss.str());
    }
    return static_cast<uint32_t>(int_val);
}

uint32_t XmlLoader::readRequiredPositiveUintAttribute(const XMLElement & _xml_element, const char * _attr)
{
    int int_val = readRequiredIntAttribute(_xml_element, _attr);
    if(int_val <= 0)
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" of the tag \"" << _xml_element.Name() << "\" in the XML file "
           << m_path << " must be a positive integer";
        throw XmlException(ss.str());
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
        ss << "The attribute \"" << _attr << "\" of the tag \"" << _xml_element.Name() << "\" in the XML file "
           << m_path << " must be an integer";
        throw XmlException(ss.str());
    }
    return static_cast<int32_t>(int_val);
}

const char * XmlLoader::readRequiredAttribute(const XMLElement & _xml_element, const char * _attr)
{
    const char * value = _xml_element.Attribute(_attr);
    if(value == nullptr)
    {
        std::stringstream ss;
        ss << "The attribute \"" << _attr << "\" is required for the tag \"" << _xml_element.Name()
           << "\" in the XML file " << m_path;
        throw XmlException(ss.str());
    }
    return value;
}

std::string XmlLoader::formatFileReadErrorMessage() const
{
    return formatFileReadErrorMessage(m_path);
}

std::string XmlLoader::formatFileReadErrorMessage(const std::filesystem::path & _path) const
{
    return std::format("Unable to read file: {}", _path.string());
}

std::string XmlLoader::formatXmlRootElemetErrorMessage(const char * _expected) const
{
    return std::format("The root element of the XML file {}: must be \"{}\"", m_path.string(), _expected);
}
