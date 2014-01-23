#include "MainLoop.hpp"
#include "LuaInstanceMap.hpp"
#include "LuaInterpreter.hpp"
#include "LuaHelper.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Utils.hpp"
#include "render/NullRenderer.hpp"
#include "render/sw/SwRenderer.hpp"
#include "render/sw/SwRendererMt.hpp"
#include "render/sw/SvgSwRenderer.hpp"
#include "render/sw/TexSwRenderer.hpp"
#include "render/cl/ClRenderer.hpp"

#include <lua.hpp>

#include <SDL.h>

#include <chrono>

namespace {

const std::string SW_RENDERER = "SwRenderer";
const std::string SW_RENDERER_MT_1 = "SwRendererMt1";
const std::string SW_RENDERER_MT_2 = "SwRendererMt2";
const std::string SW_RENDERER_MT_4 = "SwRendererMt4";
const std::string TEX_RENDERER = "TexRenderer";
const std::string CL_RENDERER = "ClRenderer";
const std::string SVG_RENDERER = "SvgRenderer";

LuaInstanceMap<MainLoop> instances;

} // unnamed namespace

MainLoop::MainLoop(LuaInterpreter& lua)
    : mLua(lua)
    , mRenderer(Utils::make_unique<NullRenderer>(mLua))
{
    atexit(SDL_Quit);

    instances.Add(mLua.GetState(), *this);
    mLua.RegisterAPI(GetModuleName(), GetAPI().data());
}

MainLoop::~MainLoop()
{
    instances.Remove(*this);
}

void MainLoop::Run()
{
    mQuitRequested = false;

    LuaOnStart();

    // Mainloop based on an article from Glenn Fiedler:
    //  http://gafferongames.com/game-physics/fix-your-timestep/

    // TODO: handle verly large deltas that can happen while debugging.

    // A game update call will update the game status by this amount of time.
    const std::chrono::milliseconds delta_time(mUpdateDeltaTime);

    auto old_time = std::chrono::milliseconds(SDL_GetTicks());
    auto game_time = std::chrono::milliseconds::zero();
    auto accumulator = std::chrono::milliseconds::zero();

    while (!mQuitRequested)
    {
        const auto new_time = std::chrono::milliseconds(SDL_GetTicks());
        auto frame_time = new_time - old_time;
        old_time = new_time;

        // Number of ms the game lacks behind and has to be updated for.
        accumulator += frame_time;

        while (accumulator >= delta_time)
        {
            ProcessInput();
            UpdateScene(game_time.count(), delta_time.count());
            accumulator -= delta_time;
            game_time += delta_time;
        }

        RenderScene();
    }
}

void MainLoop::RequestQuit()
{
    LuaOnQuit();

    mQuitRequested = true;
}

void MainLoop::SetUpdateTime(const int ms)
{
    mUpdateDeltaTime = ms;
}

void MainLoop::SetRenderer(const std::string& name)
{
    SelectRendererFromName(name);
}

void MainLoop::SetWorld(const std::string& name)
{
    (void) name;

    mWorld = Utils::make_unique<World>(mLua);
}

int MainLoop::GetUpdateTime() const
{
    return mUpdateDeltaTime;
}

Renderer& MainLoop::GetRenderer() const
{
    return *mRenderer;
}

World& MainLoop::GetWorld() const
{
    return *mWorld;
}

void MainLoop::ProcessInput()
{
    LuaOnInput();

    // update all inputs regardless of there being an observable event or not
    if (mWorld) {
        mWorld->ProcessInput();
    }

    SDL_Event event;
    if (!SDL_PollEvent(&event)) {
        return;
    }

    if((event.type == SDL_QUIT) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))) {
        RequestQuit(); // The user closed the window.
        return;
    }

    // Handle application-level requests, e.g. switching of renderer.
    if ((event.type == SDL_KEYDOWN) && (event.key.keysym.mod & KMOD_LCTRL))
    {
        SelectRendererFromScanCode(event.key.keysym.scancode);
        return;
    }

    if ((event.type == SDL_KEYDOWN))
    {
        switch (event.key.keysym.scancode)
        {
            case SDL_SCANCODE_COMMA:
                mWorld->GetPlayer().mPlaneY -= 0.05;
                break;
            case SDL_SCANCODE_PERIOD:
                mWorld->GetPlayer().mPlaneY += 0.05;
                break;
            default:
                break;
        }
        return;
    }
}

void MainLoop::UpdateScene(const long app_time, const long elapsed_time)
{
    (void) app_time;

    LuaOnUpdate();

    if (mWorld) {
        mWorld->Update(elapsed_time);
    }
}

void MainLoop::RenderScene()
{
    LuaOnRender();

    mRenderer->PreRender();

    if (mWorld) {
        mRenderer->DoRender(*mWorld);
    }

    mRenderer->PostRender();
}

void MainLoop::SetRenderer(const SDL_Scancode code)
{
    SelectRendererFromScanCode(code);
}

void MainLoop::SelectRendererFromScanCode(const SDL_Scancode code)
{
    switch (code)
    {
    case SDL_SCANCODE_1:
        SelectRendererFromName(SW_RENDERER);
        break;
    case SDL_SCANCODE_2:
        SelectRendererFromName(SW_RENDERER_MT_1);
        break;
    case SDL_SCANCODE_3:
        SelectRendererFromName(SW_RENDERER_MT_2);
        break;
    case SDL_SCANCODE_4:
        SelectRendererFromName(SW_RENDERER_MT_4);
        break;
#ifdef WITH_TEXTURE
    case SDL_SCANCODE_5:
        SelectRendererFromName(TEX_RENDERER);
        break;
#endif // WITH_TEXTURE
#ifdef WITH_OPENCL
    case SDL_SCANCODE_6:
        SelectRendererFromName(CL_RENDERER);
        break;
#endif // WITH_OPENCL
#ifdef WITH_SVG
    case SDL_SCANCODE_7:
        SelectRendererFromName(SVG_RENDERER);
        break;
#endif // WITH_SVG
    default:
        break;
    }
}

void MainLoop::SelectRendererFromName(const std::string& name)
{
    // the active renderer is always deleted before a new one is created.
    // this makes sure that the new renderer can capture the mouse sensibly.

    if (name == SW_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRenderer>(mLua);
    }
    else if (name == SW_RENDERER_MT_1)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRendererMt>(mLua, 1);
    }
    else if (name == SW_RENDERER_MT_2)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRendererMt>(mLua, 2);
    }
    else if (name == SW_RENDERER_MT_4)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRendererMt>(mLua, 4);
    }

#ifdef WITH_TEXTURE
    else if (name == TEX_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<TexSwRenderer>(mLua);
    }
#endif // WITH_TEXTURE

#ifdef WITH_OPENCL
    else if (name == CL_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<ClRenderer>(mLua);
    }
#endif // WITH_OPENCL

#ifdef WITH_SVG
    else if (name == SVG_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SvgSwRenderer>(mLua);
    }
#endif // WITH_SVG
}

void MainLoop::LuaOnStart()
{
    lua_State* const L = mLua.GetState();

    lua_getglobal(L, "mainloop_on_start");
    lua_pcall(L, 0, 0, 0);
}

void MainLoop::LuaOnQuit()
{
    lua_State* const L = mLua.GetState();

    lua_getglobal(L, "mainloop_on_quit");
    lua_pcall(L, 0, 0, 0);
}

void MainLoop::LuaOnInput()
{
    lua_State* const L = mLua.GetState();

    lua_getglobal(L, "mainloop_on_input");
    lua_pcall(L, 0, 0, 0);
}

void MainLoop::LuaOnUpdate()
{
    lua_State* const L = mLua.GetState();

    lua_getglobal(L, "mainloop_on_update");
    lua_pcall(L, 0, 0, 0);
}

void MainLoop::LuaOnRender()
{
    lua_State* const L = mLua.GetState();

    lua_getglobal(L, "mainloop_on_render");
    lua_pcall(L, 0, 0, 0);
}

std::string MainLoop::GetModuleName()
{
    return "mainloop";
}

std::vector<luaL_Reg> MainLoop::GetAPI()
{
    return {
        { "run", [](lua_State* L) {
            instances.Get(L).Run();
            return 0;
        } },

        { "request_quit", [](lua_State* L) {
            instances.Get(L).RequestQuit();
            return 0;
        } },

        { "set_update_time", [](lua_State* L) {
            const int update_time = static_cast<int>(lua_tointeger(L, -1));
            instances.Get(L).SetUpdateTime(update_time);
            return 0;
        } },

        { "set_renderer", [](lua_State* L) {
            const char* const name = lua_tostring(L, -1);
            instances.Get(L).SetRenderer(name);
            return 0;
        } },

        { "set_world", [](lua_State* L) {
            const char* const name = lua_tostring(L, -1);
            instances.Get(L).SetWorld(name);
            return 0;
        } },

        { nullptr, nullptr }
    };
}
