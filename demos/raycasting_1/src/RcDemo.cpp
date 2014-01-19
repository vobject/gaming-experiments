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

const static struct luaL_Reg rcdemo_funcs[] = {
    { "set_app_name", [](lua_State* state)
        {
            if (!lua_isstring(state, -1)) {
                throw "set_app_name() must return a string.";
                return -1;
            }

            const char* name = lua_tostring(state, -1);

            RcDemo::Instance().SetAppName(name);
            return 0;
        }
    },

    { "set_resolution", [](lua_State* state)
        {
            if (!lua_isnumber(state, -1) || !lua_isnumber(state, -2)) {
                throw "set_resolution() must return two numbers.";
                return -1;
            }

            const auto height = lua_tointeger(state, -1);
            const auto width = lua_tointeger(state, -2);

            RcDemo::Instance().SetResolution(width, height);
            return 0;
        }
    },

    { nullptr, nullptr }
};

RcDemo& RcDemo::Instance()
{
    static RcDemo instance;
    return instance;
}

RcDemo::RcDemo()
{

}

RcDemo::~RcDemo()
{

}

void RcDemo::Start()
{
    atexit(SDL_Quit);

    Initialize();
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

const Renderer& RcDemo::GetRenderer() const
{
    return *mRenderer;
}

const World& RcDemo::GetWorld() const
{
    return *mWorld;
}

void RcDemo::RegisterLua(LuaInterpreter& lua)
{
    lua.RegisterFunctions("rcdemo", rcdemo_funcs);
}

void RcDemo::Mainloop()
{
    // Mainloop based on an article from Glenn Fiedler:
    //  http://gafferongames.com/game-physics/fix-your-timestep/

    // A game update call will update the game status by this amount of time.
    const std::chrono::milliseconds delta_time(2);

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

void RcDemo::Initialize()
{
    mRenderer = Utils::make_unique<SwRenderer>(mResX, mResY, mAppName);

    // load the default level
    mWorld = Utils::make_unique<World>("");
    mWorld->InternalGetPlayer().SetHorizontalRayCount(mResX);
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
        const auto res_x = mRenderer->GetResX();
        const auto res_y = mRenderer->GetResY();
        const auto app_name = mRenderer->GetAppName();

        switch (event.key.keysym.scancode)
        {
            case SDL_SCANCODE_1:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<SwRenderer>(res_x, res_y, app_name);
                break;
            case SDL_SCANCODE_2:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<SwRendererMt>(res_x, res_y, app_name, 1);
                break;
            case SDL_SCANCODE_3:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<SwRendererMt>(res_x, res_y, app_name, 2);
                break;
            case SDL_SCANCODE_4:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<SwRendererMt>(res_x, res_y, app_name, 4);
                break;
#ifdef WITH_TEXTURE
            case SDL_SCANCODE_5:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<TexSwRenderer>(res_x, res_y, app_name);
                break;
#endif // WITH_TEXTURE
#ifdef WITH_OPENCL
            case SDL_SCANCODE_6:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<ClRenderer>(res_x, res_y, app_name);
                break;
#endif // WITH_OPENCL
#ifdef WITH_SVG
            case SDL_SCANCODE_7:
                mRenderer = nullptr;
                mRenderer = Utils::make_unique<SvgSwRenderer>(res_x, res_y, app_name);
                break;
#endif // WITH_SVG
            default:
                break;
        }
        return;
    }

    if ((event.type == SDL_KEYDOWN))
    {
        switch (event.key.keysym.scancode)
        {
            case SDL_SCANCODE_COMMA:
                mWorld->InternalGetPlayer().mPlaneY -= 0.05;
                break;
            case SDL_SCANCODE_PERIOD:
                mWorld->InternalGetPlayer().mPlaneY += 0.05;
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
