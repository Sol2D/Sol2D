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

#include <Sol2D/Workspace.h>
#include <Sol2D/StepState.h>
#include <Sol2D/Window.h>
#include <Sol2D/MediaLayer/MediaLayer.h>
#include <Sol2D/Lua/LuaLibrary.h>
#include <iostream>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

class SDLAssertionHandler final
{
    S2_DISABLE_COPY_AND_MOVE(SDLAssertionHandler)

public:
    explicit SDLAssertionHandler(const Workspace & _workspace) :
        mr_workspace(_workspace)
    {
    }

    void handle(const SDL_AssertData * _data) const
    {
        mr_workspace.getMainLogger().critical(
            "SDL assertion failed: {} at {}:{}",
            _data->condition,
            _data->filename,
            _data->linenum);
    }

private:
    const Workspace & mr_workspace;
};

class Application final
{
    S2_DISABLE_COPY_AND_MOVE(Application)

public:
    explicit Application(const Workspace & _workspace);
    int exec();

private:
    void runMainLoop(SDL_Window * _window, SDL_GPUDevice * _device);
    bool handleEvent(const SDL_Event & _event);
    void onWindowResized(const SDL_WindowEvent & _event);
    void onMouseButtonDown(const SDL_MouseButtonEvent & _event);
    void onMouseButtonUp(const SDL_MouseButtonEvent & _event);
    void step();

private:
    const Workspace & mr_workspace;
    SDLAssertionHandler m_sdl_assertion_handler;
    StepState m_step_state;
    Window m_window;
};

SDL_AssertState SDLCALL sdlAssertionHandler(const SDL_AssertData * _data, void * _userdata)
{
    static_cast<const SDLAssertionHandler *>(_userdata)->handle(_data);
    return SDL_ASSERTION_BREAK;
}

} // namespace

Application::Application(const Workspace & _workspace) :
    mr_workspace(_workspace),
    m_sdl_assertion_handler(_workspace),
    m_step_state{}
{
}

int Application::exec()
{
    SDL_Window * sdl_window = nullptr;
    SDL_GPUDevice * device = nullptr;
    int result = 0;
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        mr_workspace.getMainLogger().critical("Unable to initialize SDL: {}", SDL_GetError());
        goto FAIL_APP;
    }
    if(!TTF_Init())
    {
        mr_workspace.getMainLogger().critical("SDL_TTF initialization failed. {}", SDL_GetError());
        goto FAIL_APP;
    }
    if(!Mix_OpenAudio(0, nullptr)) // TODO: allow user to select a device
    {
        mr_workspace.getMainLogger().critical("SDL_Mixer initialization failed. {}", SDL_GetError());
        goto FAIL_APP;
    }
    SDL_SetAssertionHandler(sdlAssertionHandler, &m_sdl_assertion_handler);
    sdl_window = SDL_CreateWindow(
        mr_workspace.getApplicationName().c_str(),
        1024,
        768,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    if(sdl_window == nullptr)
    {
        mr_workspace.getMainLogger().critical("Unable to create window: {}", SDL_GetError());
        goto FAIL_APP;
    }
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if(device == nullptr)
    {
        mr_workspace.getMainLogger().critical("Unable to create GPU device: {}");
        goto FAIL_APP;
    }
    if(!SDL_ClaimWindowForGPUDevice(device, sdl_window))
    {
        mr_workspace.getMainLogger().critical("Unable to claim window for GPU device: {}", SDL_GetError());
        goto FAIL_APP;
    }
    if(!SDL_ShowWindow(sdl_window))
    {
        mr_workspace.getMainLogger().critical("Unable to show window: {}", SDL_GetError());
        result = 1;
        goto FAIL_APP;
    }
    try
    {
        runMainLoop(sdl_window, device);
    }
    catch(const std::runtime_error & error)
    {
        mr_workspace.getMainLogger().critical(error.what());
        goto FAIL_APP;
    }
    catch(...)
    {
        mr_workspace.getMainLogger().critical("An unknown error has occurred");
        goto FAIL_APP;
    }
    goto EXIT_APP;
FAIL_APP:
    result = 1;
EXIT_APP:
    if(device) SDL_DestroyGPUDevice(device);
    if(sdl_window) SDL_DestroyWindow(sdl_window);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
    return result;
}

void Application::runMainLoop(SDL_Window * _window, SDL_GPUDevice * _device)
{
    ResourceManager resource_manager; // TODO: create in place
    Renderer renderer(resource_manager, _window, _device);
    StoreManager store_manager;
    std::unique_ptr<LuaLibrary> lua = std::make_unique<LuaLibrary>(mr_workspace, store_manager, m_window, renderer);
    lua->executeMainScript();
    const uint32_t render_frame_delay = floor(1000 / mr_workspace.getFrameRate());
    uint32_t last_rendering_ticks = SDL_GetTicks();
    SDL_Event event;
    for(;;)
    {
        while(SDL_PollEvent(&event))
            if(handleEvent(event)) return;
        const uint32_t now_ticks = SDL_GetTicks();
        const uint32_t passed_ticks = now_ticks - last_rendering_ticks;
        if(passed_ticks >= render_frame_delay)
        {
            last_rendering_ticks = now_ticks;
            m_step_state.time_passed = std::chrono::milliseconds(passed_ticks);
            m_step_state.mouse_state.buttons = SDL_GetMouseState(
                &m_step_state.mouse_state.position.x,
                &m_step_state.mouse_state.position.y);
            renderer.beginStep();
            step();
            renderer.submitStep();
            if(m_step_state.mouse_state.lb_click.state == MouseClickState::Finished)
                m_step_state.mouse_state.lb_click.state = MouseClickState::None;
            if(m_step_state.mouse_state.rb_click.state == MouseClickState::Finished)
                m_step_state.mouse_state.rb_click.state = MouseClickState::None;
            if(m_step_state.mouse_state.mb_click.state == MouseClickState::Finished)
                m_step_state.mouse_state.mb_click.state = MouseClickState::None;
        }
        int32_t delay = render_frame_delay - passed_ticks;
        if(delay > 5) SDL_Delay(delay);
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
    m_window.resize();
}

inline void Application::onMouseButtonDown(const SDL_MouseButtonEvent & _event)
{
    switch(_event.button)
    {
    case SDL_BUTTON_LEFT:
        m_step_state.mouse_state.lb_click.state = MouseClickState::Started;
        m_step_state.mouse_state.lb_click.start.x = _event.x;
        m_step_state.mouse_state.lb_click.start.y = _event.y;
        break;
    case SDL_BUTTON_RIGHT:
        m_step_state.mouse_state.rb_click.state = MouseClickState::Started;
        m_step_state.mouse_state.rb_click.start.x = _event.x;
        m_step_state.mouse_state.rb_click.start.y = _event.y;
        break;
    case SDL_BUTTON_MIDDLE:
        m_step_state.mouse_state.mb_click.state = MouseClickState::Started;
        m_step_state.mouse_state.mb_click.start.x = _event.x;
        m_step_state.mouse_state.mb_click.start.y = _event.y;
        break;
    }
}

inline void Application::onMouseButtonUp(const SDL_MouseButtonEvent & _event)
{
    switch(_event.button)
    {
    case SDL_BUTTON_LEFT:
        m_step_state.mouse_state.lb_click.state = MouseClickState::Finished;
        m_step_state.mouse_state.lb_click.finish.x = _event.x;
        m_step_state.mouse_state.lb_click.finish.y = _event.y;
        break;
    case SDL_BUTTON_RIGHT:
        m_step_state.mouse_state.rb_click.state = MouseClickState::Finished;
        m_step_state.mouse_state.rb_click.finish.x = _event.x;
        m_step_state.mouse_state.rb_click.finish.y = _event.y;
        break;
    case SDL_BUTTON_MIDDLE:
        m_step_state.mouse_state.mb_click.state = MouseClickState::Finished;
        m_step_state.mouse_state.mb_click.finish.x = _event.x;
        m_step_state.mouse_state.mb_click.finish.y = _event.y;
        break;
    }
}

void Application::step()
{
    m_window.step(m_step_state);
}

int main(int _argc, const char ** _argv)
{
    std::unique_ptr<Workspace> workspace;
    std::unique_ptr<Application> app;
    {
        std::filesystem::path config(_argc > 1 ? _argv[1] : "game.xml");
        if(config.is_relative())
            config = std::filesystem::path(SDL_GetBasePath()) / config;
        workspace = Workspace::load(config);
        if(!workspace)
        {
            std::cerr << "Unable to load manifest file" << std::endl;
            return 2;
        }
        app = std::make_unique<Application>(std::ref(*workspace));
    }
    return app->exec();
}
