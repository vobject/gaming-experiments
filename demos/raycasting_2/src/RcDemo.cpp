#include "RcDemo.hpp"
#include "MainFrame.hpp"
#include "Level.hpp"
#include "Input.hpp"
#include "Player.hpp"
#include "SwRenderer.hpp"
#include "MtSwRenderer.hpp"
#include "ClRenderer.hpp"

#include <SDL.h>

#include <chrono>
#include <unistd.h>

RcDemo::RcDemo()
{

}

RcDemo::~RcDemo()
{

}

void RcDemo::Start()
{
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
   mRenderer = std::make_shared<SwRenderer>(mResX, mResY);
   mMainFrame = std::make_shared<MainFrame>("RayCasting_2");
   mMainFrame->SetRendererName(mRenderer->GetName());

   mLevel = std::make_shared<Level>();
   mInput = std::make_shared<Input>(SDLK_UP, SDLK_DOWN,
                                    SDLK_LEFT, SDLK_RIGHT,
                                    SDLK_LCTRL, SDLK_SPACE);

   mPlayer = std::make_shared<Player>(*mLevel, *mInput);
}

void RcDemo::ProcessInput()
{
   SDL_Event event;

   if (!SDL_PollEvent(&event)) {
      return;
   }

   if(SDL_QUIT == event.type) {
      // The user closed the window.
      mQuitRequested = true;
      return;
   }

   // Handle application-level requests, e.g. switching of renderer.
   if ((SDL_KEYDOWN == event.type) && (event.key.keysym.mod & KMOD_LCTRL))
   {
      if (SDLK_1 == event.key.keysym.sym) {
         mRenderer = std::make_shared<SwRenderer>(mResX, mResY);
         mMainFrame->SetRendererName(mRenderer->GetName());
      }

#ifdef WITH_MTSWRENDERER
      else if (SDLK_2 == event.key.keysym.sym) {
         mRenderer = std::make_shared<MtSwRenderer>(mResX, mResY, sysconf(_SC_NPROCESSORS_ONLN));
         mMainFrame->SetRendererName(mRenderer->GetName());
      }
#endif // WITH_MTSWRENDERER

#ifdef WITH_OPENCL
      else if (SDLK_3 == event.key.keysym.sym) {
         mRenderer = std::make_shared<ClRenderer>(mResX, mResY);
         mMainFrame->SetRendererName(mRenderer->GetName());
      }
#endif // WITH_OPENCL

      return;
   }

   switch (event.type)
   {
      case SDL_KEYDOWN:
         mInput->Press(event.key.keysym.sym);
         break;
      case SDL_KEYUP:
         mInput->Release(event.key.keysym.sym);
         break;
      default:
         break;
   }
}

void RcDemo::UpdateScene(const int app_time, const int elapsed_time)
{
   (void) app_time;

   mPlayer->Update(elapsed_time);
   mMainFrame->UpdateDone();
}

void RcDemo::RenderScene()
{
   mRenderer->PreRender();
   mRenderer->DoRender(*mLevel, *mPlayer);
   mRenderer->PostRender();

   mMainFrame->FrameDone();
}
