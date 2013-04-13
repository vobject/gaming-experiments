#include "RcDemo.hpp"
#include "MainFrame.hpp"
#include "Level.hpp"
#include "Input.hpp"
#include "Player.hpp"
#include "SwRenderer.hpp"
#include "ClRenderer.hpp"

#if !defined(_WIN32)
#include <X11/Xlib.h>
#endif

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
#if !defined(_WIN32)
   // HACK: How can I get rid of this? Could not find a solution yet. *sadface*
   //  https://github.com/DrMcCoy/xoreos/commit/9a6c84d5458256ac5a0ff7525055ef2d8761e683
   //  http://stackoverflow.com/questions/13128272/sdl-locksurface-for-multithreading
   if (!XInitThreads()) {
      throw "Failed to initialize Xlib muti-threading support";
   }
#endif

   const auto res_x = 640;
   const auto res_y = 480;

   mRenderers = {
      std::make_shared<SwRenderer>(res_x, res_y),
      std::make_shared<ClRenderer>(res_x, res_y)
   };
   mActiveRenderer = 1;

   mMainFrame = std::make_shared<MainFrame>("RcDemo_1");
   mMainFrame->SetRendererName(mRenderers[mActiveRenderer]->GetName());

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
      if (SDLK_r == event.key.keysym.sym)
      {
         NextRenderer();
      }
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
   mRenderers[mActiveRenderer]->PreRender();
   mRenderers[mActiveRenderer]->DoRender(*mLevel, *mPlayer);
   mRenderers[mActiveRenderer]->PostRender();

   mMainFrame->FrameDone();
}

void RcDemo::NextRenderer()
{
   if (++mActiveRenderer >= mRenderers.size())
   {
      mActiveRenderer = 0;
   }
   mMainFrame->SetRendererName(mRenderers[mActiveRenderer]->GetName());
}
