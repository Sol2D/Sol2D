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

#include <Sol2D/Utils/Unmap.h>
#include <Sol2D/Def.h>
#include <imgui.h>
#include <string>
#include <list>
#include <unordered_map>
#include <cstdint>

namespace Sol2D {

class UIForm final
{
    S2_DISABLE_COPY_AND_MOVE(UIForm)

private:
    class Widget;
    class Label;
    class Button;
    class Checkbox;

public:
    UIForm(uint32_t _id, const std::string & _title);
    ~UIForm();
    uint32_t getId() const { return m_id; }
    void addLabel(const std::string & _label);
    void addButton(const std::string & _label);
    void addCheckbox(const std::string & _label, bool _checked = false);
    void renderWidgets();
    ImDrawData * getDrawData() const { return ImGui::GetDrawData(); }

private:
    void addWidget(const std::string & _key, Widget * _widget);

private:
    const uint32_t m_id;
    std::string m_title;
    std::unordered_map<std::string, Widget *> m_widgets;
    std::list<Widget *> m_ordered_widgets;
};

class UI final
{
    S2_DISABLE_COPY_AND_MOVE(UI)

public:
    using FormMap = std::unordered_map<uint32_t, UIForm *>;
    using Forms = Utils::Unmap<FormMap>;
    using ConstForms = Utils::ConstUnmap<FormMap>;

    UI() :
        m_next_id(1)
    {
    }

    ~UI();
    UIForm & addForm(const std::string & _title);
    UIForm * getForm(uint32_t _id);
    Forms getForms() { return m_forms; }
    ConstForms getForms() const { return m_forms; }

private:
    FormMap m_forms;
    uint32_t m_next_id;
};

} // namespace Sol2D
