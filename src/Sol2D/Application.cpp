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

#include <Sol2D/Workspace.h>
#include <Sol2D/Lua/LuaSceneController.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <iostream>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace fs = std::filesystem;

namespace {

class Application final
{
private:
    explicit Application(const Workspace & _workspace);

public:
    ~Application();
    static bool run(const Workspace & _workspace);
    void render();

private:
    bool initialize();
    void initializeLibTmx();
    void runMainLoop();
    bool handleEvent(const SDL_Event & _event);

private:
    SDL_Window * mp_window;
    SDL_Renderer * mp_renderer;
    const Workspace & mr_workspace;
    Scene * mp_scene;
    LuaSceneController * mp_controller;
};

} // namespace


Application::Application(const Workspace & _workspace) :
    mp_window(nullptr),
    mp_renderer(nullptr),
    mr_workspace(_workspace),
    mp_scene(nullptr),
    mp_controller(nullptr)
{
}

Application::~Application()
{
    if(mp_renderer)
        SDL_DestroyRenderer(mp_renderer);
    if(mp_window)
        SDL_DestroyWindow(mp_window);
    IMG_Quit();
    SDL_Quit();
    delete mp_controller;
    delete mp_scene;
}

bool Application::run(const Workspace & _workspace)
{
    std::unique_ptr<Application> app(new Application(_workspace));
    if(!app->initialize())
        return false;
    app->runMainLoop();
    return true;
}

bool Application::initialize()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) != 0)
    {
        mr_workspace.getMainLogger().critical("SDL initialization failed. {}", SDL_GetError());
        return false;
    }
    if(IMG_Init(IMG_INIT_PNG) == 0)
    {
        mr_workspace.getMainLogger().critical("SDL_Image initialization failed. {}", SDL_GetError());
        return false;
    }
    // TODO: SDL_WINDOW_VULKAN from manifest
    mp_window = SDL_CreateWindow(
        mr_workspace.getApplicationName().c_str(),
        800,
        600,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN /*| SDL_WINDOW_FULLSCREEN*/);
    if(!mp_window)
    {
        mr_workspace.getMainLogger().critical("Unable to create window. {}", SDL_GetError());
        return false;
    }
    mp_renderer = SDL_CreateRenderer(mp_window, nullptr, SDL_RENDERER_ACCELERATED);
    if(!mp_renderer)
    {
        mr_workspace.getMainLogger().critical("Unable to create renderer. {}", SDL_GetError());
        return false;
    }
    mp_scene = new Scene(*mp_renderer, mr_workspace);
    mp_controller = new LuaSceneController(mr_workspace, *mp_scene);
    mp_controller->prepare();
    return true;
}

void Application::runMainLoop()
{
    Uint32 render_frame_delay = floor(1000 / mr_workspace.getFrameRate());
    Uint32 next_render_ticks = SDL_GetTicks() + render_frame_delay;
    Uint32 now_ticks;
    SDL_Event event;
    for(;;)
    {
        while(SDL_PollEvent(&event))
           if(handleEvent(event)) return;
        now_ticks = SDL_GetTicks();
        int delta_next = next_render_ticks - now_ticks;
        if(delta_next <= 0)
        {
           render();
           next_render_ticks += render_frame_delay;
        }
        else if(delta_next > 5)
        {
           SDL_Delay(5); // Reduce CPU usage
        }
    }
}

bool Application::handleEvent(const SDL_Event & _event)
{
    switch (_event.type)
    {
    case SDL_EVENT_QUIT:
        return true;
    default:
        break;
    }
    return false;
}

void Application::render()
{
    int width, height;
    SDL_GetCurrentRenderOutputSize(mp_renderer, &width, &height);
    mp_controller->render(SDL_FRect { 20.0f, 20.0f, static_cast<float>(width) - 40, static_cast<float>(height) - 40 }); // TODO: it is the test viewport
}

int main(int _argc, const char ** _argv)
{
    if(_argc == 0)
    {
        std::cerr << "Executable path not set" << std::endl;
        return 1;
    }
    auto workspace = Workspace::load(fs::path(_argv[0]).parent_path().append(Workspace::s_default_dirname));
    if(!workspace)
    {
        std::cerr << "Unable to load manifest file" << std::endl;
        return 2;
    }
    return Application::run(*workspace) ? 0 : 255;
}
