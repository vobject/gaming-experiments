#include "RcDemo.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Utils.hpp"
#include "render/sw/SwRenderer.hpp"
#include "render/sw/SwRendererMt.hpp"
#include "render/sw/SvgSwRenderer.hpp"
#include "render/sw/TexSwRenderer.hpp"
#include "render/cl/ClRenderer.hpp"

#include <SDL.h>

#include <chrono>

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

const Renderer& RcDemo::GetRenderer() const
{
    return *mRenderer;
}

const World& RcDemo::GetWorld() const
{
    return *mWorld;
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
    const auto res_x = 640;
    const auto res_y = 480;
    const auto app_name = "RayCasting";

    mRenderer = Utils::make_unique<TexSwRenderer>(res_x, res_y, app_name);

    // load the default level
    mWorld = Utils::make_unique<World>("");
    mWorld->InternalGetPlayer().SetHorizontalRayCount(res_x);
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
