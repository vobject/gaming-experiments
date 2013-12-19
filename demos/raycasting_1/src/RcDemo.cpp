#include "RcDemo.hpp"
#include "Level.hpp"
#include "Input.hpp"
#include "Player.hpp"
#include "Utils.hpp"
#include "render/sw/SwRenderer.hpp"
#include "render/sw/SwRendererMt.hpp"
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

    mLevel = std::make_shared<Level>();
    mInput = std::make_shared<Input>(SDL_SCANCODE_W, SDL_SCANCODE_S,
                                     SDL_SCANCODE_A, SDL_SCANCODE_D,
                                     SDL_SCANCODE_E, SDL_SCANCODE_F);
    mPlayer = std::make_shared<Player>(*mLevel, *mInput);
}

void RcDemo::ProcessInput()
{
    mInput->Update();

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
                mRenderer = Utils::make_unique<SwRenderer>(res_x, res_y, app_name);
                break;
            case SDL_SCANCODE_2:
                mRenderer = Utils::make_unique<SwRendererMt>(res_x, res_y, app_name, 1);
                break;
            case SDL_SCANCODE_3:
                mRenderer = Utils::make_unique<SwRendererMt>(res_x, res_y, app_name, 2);
                break;
            case SDL_SCANCODE_4:
                mRenderer = Utils::make_unique<SwRendererMt>(res_x, res_y, app_name, 4);
                break;
#ifdef WITH_OPENCL
            case SDL_SCANCODE_5:
                mRenderer = Utils::make_unique<ClRenderer>(res_x, res_y, app_name);
                break;
#endif // WITH_OPENCL
            case SDL_SCANCODE_6:
                mRenderer = Utils::make_unique<TexSwRenderer>(res_x, res_y, app_name);
                break;
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
                mPlayer->mPlaneY -= 0.05;
                break;
            case SDL_SCANCODE_PERIOD:
                mPlayer->mPlaneY += 0.05;
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

    mPlayer->Update(elapsed_time);
}

void RcDemo::RenderScene()
{
    mRenderer->PreRender();
    mRenderer->DoRender(*mLevel, *mPlayer);
    mRenderer->PostRender();
}
