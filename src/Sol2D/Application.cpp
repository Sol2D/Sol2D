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
#include <Sol2D/Lua/LuaLibrary.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <box2d/base.h>
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
    void onWindowResized(const SDL_WindowEvent & _event);
    void onMouseButtonDown(const SDL_MouseButtonEvent & _event);
    void onMouseButtonUp(const SDL_MouseButtonEvent & _event);

private:
    SDL_Window * mp_sdl_window;
    SDL_Renderer * mp_sdl_renderer;
    const Workspace & mr_workspace;
    RenderState m_render_state;
    Window * mp_window;
    StoreManager * mp_store_manager;
    LuaLibrary * mp_lua;
};

SDL_AssertState SDLCALL sdlAssertionHandler(const SDL_AssertData * _data, void * /*_userdata*/)
{
    std::cerr << "SDL assertion failed: \"" << _data->condition <<
        "\" at " << _data->filename << ":" << _data->linenum << std::endl;
    return SDL_ASSERTION_BREAK;
}

int box2dAssertionHandler(const char * _condition, const char * _file_name, int _line_number)
{
    std::cerr << "Box2D assertion failed: \"" << _condition <<
        "\" at " << _file_name << ":" << _line_number << std::endl;
    return 1;
}

} // namespace


Application::Application(const Workspace & _workspace) :
    mp_sdl_window(nullptr),
    mp_sdl_renderer(nullptr),
    mr_workspace(_workspace),
    m_render_state{},
    mp_window(nullptr),
    mp_store_manager(nullptr),
    mp_lua(nullptr)
{
}

Application::~Application()
{
    if(mp_sdl_renderer)
        SDL_DestroyRenderer(mp_sdl_renderer);
    if(mp_sdl_window)
        SDL_DestroyWindow(mp_sdl_window);
    delete mp_lua;
    delete mp_window;
    delete mp_store_manager;
    IMG_Quit();
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
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
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        mr_workspace.getMainLogger().critical("SDL initialization failed. {}", SDL_GetError());
        return false;
    }
    if(IMG_Init(IMG_INIT_PNG) == 0)
    {
        mr_workspace.getMainLogger().critical("SDL_Image initialization failed. {}", SDL_GetError());
        return false;
    }
    if(!TTF_Init())
    {
        mr_workspace.getMainLogger().critical("SDL_TTF initialization failed. {}", SDL_GetError());
        return false;
    }
    if(!Mix_OpenAudio(0, nullptr)) // TODO: allow user to select a device
    {
        mr_workspace.getMainLogger().critical("SDL_Mixer initialization failed. {}", SDL_GetError());
        return false;
    }
    SDL_SetAssertionHandler(sdlAssertionHandler, nullptr);
    b2SetAssertFcn(box2dAssertionHandler);
    // TODO: SDL_WINDOW_VULKAN from manifest
    mp_sdl_window = SDL_CreateWindow(
        mr_workspace.getApplicationName().c_str(),
        800,
        600,
        SDL_WINDOW_RESIZABLE
        // SDL_WINDOW_FULLSCREEN
        // | SDL_WINDOW_HIGH_PIXEL_DENSITY
        | SDL_WINDOW_VULKAN
        // TODO: "vulkan" renderer cannot be created with SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if(!mp_sdl_window)
    {
        mr_workspace.getMainLogger().critical("Unable to create window. {}", SDL_GetError());
        return false;
    }
    mp_sdl_renderer = SDL_CreateRenderer(mp_sdl_window, nullptr); // TODO: select driver
    if(!mp_sdl_renderer)
    {
        mr_workspace.getMainLogger().critical("Unable to create renderer. {}", SDL_GetError());
        return false;
    }
    mp_window = new Window(*mp_sdl_renderer);
    mp_store_manager = new StoreManager();
    mp_lua = new LuaLibrary(mr_workspace, *mp_store_manager, *mp_window, *mp_sdl_renderer);
    mp_lua->executeMainScript();
    return true;
}

void Application::runMainLoop()
{
    const Uint32 render_frame_delay = floor(1000 / mr_workspace.getFrameRate());
    Uint32 last_rendering_ticks = SDL_GetTicks();
    SDL_Event event;
    for(;;)
    {
        while(SDL_PollEvent(&event))
            if(handleEvent(event)) return;
        const Uint32 now_ticks = SDL_GetTicks();
        const Uint32 passed_ticks = now_ticks - last_rendering_ticks;
        if(passed_ticks >= render_frame_delay)
        {
            last_rendering_ticks = now_ticks;
            m_render_state.time_passed = std::chrono::milliseconds(passed_ticks);
            m_render_state.mouse_state.buttons = SDL_GetMouseState(
                &m_render_state.mouse_state.position.x,
                &m_render_state.mouse_state.position.y);
            render();
            if(m_render_state.mouse_state.lb_click.state == MouseClickState::Finished)
                m_render_state.mouse_state.lb_click.state = MouseClickState::None;
            if(m_render_state.mouse_state.rb_click.state == MouseClickState::Finished)
                m_render_state.mouse_state.rb_click.state = MouseClickState::None;
            if(m_render_state.mouse_state.mb_click.state == MouseClickState::Finished)
                m_render_state.mouse_state.mb_click.state = MouseClickState::None;
        }
        if(render_frame_delay - passed_ticks > 5)
        {
            SDL_Delay(5); // Reduce CPU usage
        }
    }
}

bool Application::handleEvent(const SDL_Event & _event)
{
    switch (_event.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        onMouseButtonDown(_event.button);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        onMouseButtonUp(_event.button);
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        onWindowResized(_event.window);
        break;
    case SDL_EVENT_QUIT:
        return true;
    default:
        break;
    }
    return false;
}

inline void Application::onWindowResized(const SDL_WindowEvent & /*_event*/)
{
    mp_window->resize();
}

inline void Application::onMouseButtonDown(const SDL_MouseButtonEvent & _event)
{
    switch(_event.button)
    {
    case SDL_BUTTON_LEFT:
        m_render_state.mouse_state.lb_click.state = MouseClickState::Started;
        m_render_state.mouse_state.lb_click.start = makePoint(_event.x, _event.y);
        break;
    case SDL_BUTTON_RIGHT:
        m_render_state.mouse_state.rb_click.state = MouseClickState::Started;
        m_render_state.mouse_state.rb_click.start = makePoint(_event.x, _event.y);
        break;
    case SDL_BUTTON_MIDDLE:
        m_render_state.mouse_state.mb_click.state = MouseClickState::Started;
        m_render_state.mouse_state.mb_click.start = makePoint(_event.x, _event.y);
        break;
    }
}

inline void Application::onMouseButtonUp(const SDL_MouseButtonEvent & _event)
{
    switch(_event.button)
    {
    case SDL_BUTTON_LEFT:
        m_render_state.mouse_state.lb_click.state = MouseClickState::Finished;
        m_render_state.mouse_state.lb_click.finish = makePoint(_event.x, _event.y);
        break;
    case SDL_BUTTON_RIGHT:
        m_render_state.mouse_state.rb_click.state = MouseClickState::Finished;
        m_render_state.mouse_state.rb_click.finish = makePoint(_event.x, _event.y);
        break;
    case SDL_BUTTON_MIDDLE:
        m_render_state.mouse_state.mb_click.state = MouseClickState::Finished;
        m_render_state.mouse_state.mb_click.finish = makePoint(_event.x, _event.y);
        break;
    }
}

void Application::render()
{
    int width, height;
    SDL_GetCurrentRenderOutputSize(mp_sdl_renderer, &width, &height);
    mp_window->render(m_render_state);
    mp_lua->step(m_render_state);
}

int main(int _argc, const char ** _argv)
{
    if(_argc == 0)
    {
        std::cerr << "Executable path not set" << std::endl;
        return 1;
    }
    auto workspace = Workspace::load(
        fs::absolute(fs::path(_argv[0]))
            .parent_path()
            .append(_argc > 1 ? _argv[1] : "game.xml"));
    if(!workspace)
    {
        std::cerr << "Unable to load manifest file" << std::endl;
        return 2;
    }
    return Application::run(*workspace) ? 0 : 255;
}
