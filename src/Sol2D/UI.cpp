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

#include <Sol2D/UI.h>

using namespace Sol2D;

class UIForm::Widget
{
public:
    virtual ~Widget() {}
    virtual bool push() = 0;
};

class UIForm::Label : public UIForm::Widget
{
public:
    explicit Label(const std::string _label) :
          m_label(_label)
    {
    }

    bool push() override
    {
        ImGui::TextUnformatted(m_label.c_str());
        return false;
    }

private:
    std::string m_label;
};

class UIForm::Button : public UIForm::Widget
{
public:
    explicit Button(const std::string _label) :
        m_label(_label)
    {
    }

    bool push() override
    {
        return ImGui::Button(m_label.c_str());
    }

private:
    std::string m_label;
};

class UIForm::Checkbox : public UIForm::Widget
{
public:
    Checkbox(const std::string _label, bool _checked) :
        m_label(_label),
        m_checked(_checked)
    {
    }

    bool push() override
    {
        return ImGui::Checkbox(m_label.c_str(), &m_checked);
    }

private:
    std::string m_label;
    bool m_checked;
};

UIForm::UIForm(uint32_t _id, const std::string & _title) :
    m_id(_id),
    m_title(_title)
{
}

UIForm::~UIForm()
{
    for(Widget * widget : m_ordered_widgets)
        delete widget;
}

void UIForm::addLabel(const std::string & _label)
{
    addWidget(_label, new Label(_label));
}

void UIForm::addWidget(const std::string & _key, Widget * _widget)
{
    auto it = m_widgets.find(_key);
    if(it != m_widgets.end())
        delete it->second;
    m_widgets[_key] = _widget;
    m_ordered_widgets.push_back(_widget);
}

void UIForm::addButton(const std::string & _label)
{
    addWidget(_label, new Button(_label));
}

void UIForm::addCheckbox(const std::string & _label, bool _checked /*= false*/)
{
    addWidget(_label, new Checkbox(_label, _checked));
}

void UIForm::renderWidgets()
{
    ImGui::Begin(m_title.c_str());
    for(Widget * widget : m_ordered_widgets)
        widget->push();
    ImGui::End();
}

UI::~UI()
{
    for(auto & pair : m_forms)
        delete pair.second;
}

UIForm & UI::addForm(const std::string & _title)
{
    uint32_t id = m_next_id++;
    UIForm * form = new UIForm(id, _title);
    m_forms[id] = form;
    return *form;
}

UIForm * UI::getForm(uint32_t _id)
{
    auto it = m_forms.find(_id);
    return it == m_forms.end() ? nullptr : it->second;
}
