#include "RcDemo.hpp"
#include "LuaInterpreter.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Utils.hpp"
#include "render/sw/SwRenderer.hpp"
#include "render/sw/SwRendererMt.hpp"
#include "render/sw/SvgSwRenderer.hpp"
#include "render/sw/TexSwRenderer.hpp"
#include "render/cl/ClRenderer.hpp"

#include <lua.hpp>

#include <SDL.h>

#include <chrono>

namespace {

const struct luaL_Reg lua_funcs[] = {
    { "set_app_name", [](lua_State* state) {
        if (!lua_isstring(state, -1)) {
            throw "set_app_name() must return a string.";
        }

        const char* name = lua_tostring(state, -1);
        RcDemo::Instance().SetAppName(name);
        return 0;
    }},

    { "set_resolution", [](lua_State* state) {
        if (!lua_isnumber(state, -1) || !lua_isnumber(state, -2)) {
            throw "set_resolution() must return two integers.";
        }

        const int height = static_cast<int>(lua_tointeger(state, -1));
        const int width = static_cast<int>(lua_tointeger(state, -2));
        RcDemo::Instance().SetResolution(width, height);
        return 0;
    }},

    { "set_update_time", [](lua_State* state) {
        if (!lua_isnumber(state, -1)) {
            throw "set_update_time() must return an integer.";
        }

        const int update_time = static_cast<int>(lua_tointeger(state, -1));
        RcDemo::Instance().SetUpdateTime(update_time);
        return 0;
    }},

    { "set_renderer", [](lua_State* state) {
        if (!lua_isstring(state, -1)) {
            throw "set_renderer() must return a string.";
        }

        const char* name = lua_tostring(state, -1);
        RcDemo::Instance().SetRenderer(name);
        return 0;
    } },

    { nullptr, nullptr }
};

const std::string SW_RENDERER = "SwRenderer";
const std::string SW_RENDERER_MT_1 = "SwRendererMt1";
const std::string SW_RENDERER_MT_2 = "SwRendererMt2";
const std::string SW_RENDERER_MT_4 = "SwRendererMt4";
const std::string TEX_RENDERER = "TexRenderer";
const std::string CL_RENDERER = "ClRenderer";
const std::string SVG_RENDERER = "SvgRenderer";

} // unnamed namespace

RcDemo& RcDemo::Instance()
{
    static RcDemo instance;
    return instance;
}

RcDemo::RcDemo()
{
    atexit(SDL_Quit);

    SelectRendererFromName(SW_RENDERER);
    mWorld = Utils::make_unique<World>();

    RegisterLua(mLua);
    mLua.RunScript("raycasting.lua");
}

RcDemo::~RcDemo()
{

}

void RcDemo::Start()
{
    Mainloop();
}

void RcDemo::SetAppName(const std::string& name)
{
    mAppName = name;
}

void RcDemo::SetResolution(int width, int height)
{
    mResX = width;
    mResY = height;
}

void RcDemo::SetUpdateTime(int ms)
{
    mUpdateDeltaTime = ms;
}

void RcDemo::SetRenderer(const std::string& name)
{
    SelectRendererFromName(name);
}

void RcDemo::SetRenderer(const SDL_Scancode code)
{
    SelectRendererFromScanCode(code);
}

std::string RcDemo::GetAppName() const
{
    return mAppName;
}

int RcDemo::GetWindowWidth() const
{
    return mResX;
}

int RcDemo::GetWindowHeight() const
{
    return mResY;
}

int RcDemo::GetUpdateTime() const
{
    return mUpdateDeltaTime;
}

Renderer& RcDemo::GetRenderer() const
{
    return *mRenderer;
}

World& RcDemo::GetWorld() const
{
    return *mWorld;
}

void RcDemo::RegisterLua(LuaInterpreter& lua)
{
    lua.RegisterFunctions("rcdemo", lua_funcs);
    mWorld->RegisterLua(lua);
    mRenderer->RegisterLua(lua);
}

void RcDemo::Mainloop()
{
    // Mainloop based on an article from Glenn Fiedler:
    //  http://gafferongames.com/game-physics/fix-your-timestep/

    // A game update call will update the game status by this amount of time.
    const std::chrono::milliseconds delta_time(mUpdateDeltaTime);

    auto old_time = std::chrono::milliseconds(SDL_GetTicks());
    auto game_time = std::chrono::milliseconds::zero();
    auto accumulator = std::chrono::milliseconds::zero();

    while(!mQuitRequested)
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

void RcDemo::ProcessInput()
{
    // update all inputs regardless of there being an observable event or not
    mWorld->ProcessInput();

    SDL_Event event;
    if (!SDL_PollEvent(&event)) {
        return;
    }

    if((event.type == SDL_QUIT) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))) {
        // The user closed the window.
        mQuitRequested = true;
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

void RcDemo::UpdateScene(const long app_time, const long elapsed_time)
{
    (void) app_time;

    mWorld->Update(elapsed_time);
}

void RcDemo::RenderScene()
{
    mRenderer->PreRender();
    mRenderer->DoRender(*mWorld);
    mRenderer->PostRender();
}

void RcDemo::SelectRendererFromName(const std::string& name)
{
    // the active renderer is always deleted before a new one is created.
    // this makes sure that the new renderer can capture the mouse sensibly.

    if (name == SW_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRenderer>(mResX, mResY, mAppName);
    }
    else if (name == SW_RENDERER_MT_1)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRendererMt>(mResX, mResY, mAppName, 1);
    }
    else if (name == SW_RENDERER_MT_2)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRendererMt>(mResX, mResY, mAppName, 2);
    }
    else if (name == SW_RENDERER_MT_4)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SwRendererMt>(mResX, mResY, mAppName, 4);
    }

#ifdef WITH_TEXTURE
    else if (name == TEX_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<TexSwRenderer>(mResX, mResY, mAppName);
    }
#endif // WITH_TEXTURE

#ifdef WITH_OPENCL
    else if (name == CL_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<ClRenderer>(mResX, mResY, mAppName);
    }
#endif // WITH_OPENCL

#ifdef WITH_SVG
    else if (name == SVG_RENDERER)
    {
        mRenderer = nullptr;
        mRenderer = Utils::make_unique<SvgSwRenderer>(mResX, mResY, mAppName);
    }
#endif // WITH_SVG
}

void RcDemo::SelectRendererFromScanCode(const SDL_Scancode code)
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
