#include "Pong.hpp"
#include "MainFrame.hpp"
#include "Input.hpp"
#include "Game.hpp"
#include "Field.hpp"
#include "Goal.hpp"
#include "Ball.hpp"
#include "Player.hpp"
#include "SwRenderer.hpp"

#include <SDL.h>

#include <chrono>

Pong::Pong()
{

}

Pong::~Pong()
{

}

void Pong::Start()
{
   Initialize();
   Mainloop();
}

void Pong::Mainloop()
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

void Pong::Initialize()
{
   const auto res_x = 800;
   const auto res_y = 600;

   mRenderers = {
      std::make_shared<SwRenderer>(res_x, res_y)
   };
   mActiveRenderer = 0;

   mMainFrame = std::make_shared<MainFrame>("Pong");
   mMainFrame->SetRendererName(mRenderers[mActiveRenderer]->GetName());

   mInputs = {
      std::make_shared<Input>(SDLK_w, SDLK_s),
      std::make_shared<Input>(SDLK_UP, SDLK_DOWN)
   };

   mGame.reset(new Game());
   mGame->SetField(std::make_shared<Field>(0.05f, 0.2f, 0.9f, 0.75f));
   mGame->SetPlayer1(std::make_shared<Player>("Player_1", *mInputs.at(0), .2f, .5f, .03f, .15f));
   mGame->SetPlayer2(std::make_shared<Player>("Player_2", *mInputs.at(1), .8f, .5f, .03f, .15f));
   mGame->SetPlayer1Goal(std::make_shared<Goal>(0.045f, 0.2f, 0.01f, 0.75));
   mGame->SetPlayer2Goal(std::make_shared<Goal>(0.945f, 0.2f, 0.01f, 0.75));
   mGame->AddBall(std::make_shared<Ball>(0.3f, 0.5f, 0.025f, 0.025f));
   mGame->AddBall(std::make_shared<Ball>(0.6f, 0.4f, 0.05f, 0.05f));

}

void Pong::ProcessInput()
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

   switch (event.type)
   {
      case SDL_KEYDOWN:
         for (auto& input : mInputs) {
            input->Press(event.key.keysym.sym);
         }
         break;
      case SDL_KEYUP:
         for (auto& input : mInputs) {
            input->Release(event.key.keysym.sym);
         }
         break;
      default:
         break;
   }
}

void Pong::UpdateScene(const int app_time, const int elapsed_time)
{
   (void) app_time;

   mGame->Update(elapsed_time);
   mMainFrame->UpdateDone();
}

void Pong::RenderScene()
{
   auto& renderer = *(mRenderers[mActiveRenderer]);

   renderer.PreRender();
   renderer.DoRender(*mGame);
   renderer.PostRender();

   mMainFrame->FrameDone();
}
