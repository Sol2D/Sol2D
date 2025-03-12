cmake_minimum_required(VERSION 3.20)

project(ImGui)

add_library(
    ImGui
    STATIC
    ImGui/imgui.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui.h
    ImGui/imgui_internal.h
    ImGui/imgui_tables.cpp
    ImGui/imgui_widgets.cpp
    ImGui/imstb_rectpack.h
    ImGui/imstb_textedit.h
    ImGui/imstb_truetype.h
    ImGui/backends/imgui_impl_sdl3.h
    ImGui/backends/imgui_impl_sdl3.cpp
    ImGui/backends/imgui_impl_sdlgpu3.h
    ImGui/backends/imgui_impl_sdlgpu3.cpp
)

target_include_directories(ImGui PRIVATE ImGui ../SDL/include)
