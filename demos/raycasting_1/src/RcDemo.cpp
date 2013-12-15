#include "RcDemo.hpp"
#include "Level.hpp"
#include "Input.hpp"
#include "Player.hpp"
#include "render/sw/SwRenderer.hpp"
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
    const auto app_name = "RayCasting_1";

    mRenderer = std::make_shared<SwRenderer>(res_x, res_y, app_name);

    mLevel = std::make_shared<Level>();
    mInput = std::make_shared<Input>(SDL_SCANCODE_W, SDL_SCANCODE_S,
                                     SDL_SCANCODE_A, SDL_SCANCODE_D,
                                     SDL_SCANCODE_E, SDL_SCANCODE_F);

    mPlayer = std::make_shared<Player>(*mLevel, *mInput);
}

void RcDemo::ProcessInput()
{
    mInput->MouseMotion(0, 0);

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
                mRenderer = std::make_shared<SwRenderer>(res_x, res_y, app_name);
                break;
#ifdef WITH_OPENCL
            case SDL_SCANCODE_2:
                mRenderer = std::make_shared<ClRenderer>(res_x, res_y, app_name);
                break;
#endif // WITH_OPENCL
            default:
                break;
        }
        return;
    }

    switch (event.type)
    {
        case SDL_KEYDOWN:
            mInput->Press(event.key.keysym.scancode);
            break;
        case SDL_KEYUP:
            mInput->Release(event.key.keysym.scancode);
            break;
        case SDL_MOUSEMOTION:
            mInput->MouseMotion(event.motion.xrel, event.motion.yrel);
            break;
        default:
            break;
    }
}

void RcDemo::UpdateScene(const int app_time, const int elapsed_time)
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
