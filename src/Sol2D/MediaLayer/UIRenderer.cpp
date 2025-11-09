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

#include <Sol2D/MediaLayer/UIRenderer.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

using namespace Sol2D;

void UIRenderer::render(const RenderingContext & _ctx, const UI & _ui) const
{
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    for(auto & form : _ui.getForms())
    {
        form->renderWidgets();
    }
    ImGui::Render();

    SDL_GPUCommandBuffer * prepare_cmd_buffer = SDL_AcquireGPUCommandBuffer(_ctx.device);
    for(const auto & form : _ui.getForms())
    {
        ImDrawData * draw_data = form->getDrawData();
        ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, prepare_cmd_buffer);
    }
    SDL_SubmitGPUCommandBuffer(prepare_cmd_buffer);

    for(const auto & form : _ui.getForms())
    {
        ImDrawData * draw_data = form->getDrawData();
        ImGui_ImplSDLGPU3_RenderDrawData(draw_data, _ctx.command_buffer, _ctx.render_pass);
    }
}
