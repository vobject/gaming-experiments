#include "SwRenderer.hpp"
#include "Game.hpp"
#include "Field.hpp"
#include "Goal.hpp"
#include "Ball.hpp"
#include "Player.hpp"

#include <SDL.h>

#include <cmath>

SwRenderer::SwRenderer(const int res_x, const int res_y)
   : mResX(res_x)
   , mResY(res_y)
{
   if (0 > SDL_Init(SDL_INIT_VIDEO)) {
      throw "Cannot init SDL video subsystem.";
   }
   atexit(SDL_Quit);

   mScreen = SDL_SetVideoMode(mResX, mResY, 32, SDL_SWSURFACE |
                                                SDL_DOUBLEBUF);
   if (!mScreen) {
      throw "SDL_SetVideoMode() failed.";
   }

   // The return value of SDL_SetVideoMode() (-> screen) should not be freed
   //  by the caller. The man page tells us to rely on SDL_Quit() to do this.
}

SwRenderer::~SwRenderer()
{

}

void SwRenderer::PreRender()
{
   // Screen size might have changed.
   mScreen = SDL_GetVideoSurface();

   ClearScreen({ 0x00, 0x00, 0x00});
}

void SwRenderer::DoRender(const Game& game)
{
    if (game.GetField()) {
        Draw(*game.GetField());
    }

    for (auto&& obj : game.GetPlayers()) {
        Draw(*obj);
    }

//    Draw(*game.GetPlayer1Goal());
//    Draw(*game.GetPlayer2Goal());

    for (auto&& obj : game.GetBalls()) {
        Draw(*obj);
    }
}

void SwRenderer::PostRender()
{
    for (const auto& cb : mCallbacks) {
       cb(mScreen);
    }

    SDL_Flip(mScreen);
}

void SwRenderer::RegisterPostRenderHook(std::function<void(void*)> callback)
{
   mCallbacks.push_back(callback);
}

std::string SwRenderer::GetName() const
{
   return "Software";
}

void* SwRenderer::GetUnderlying() const
{
   return mScreen;
}

void SwRenderer::ClearScreen(const SDL_Color& color)
{
   const auto clear_color = SDL_MapRGB(mScreen->format, color.r, color.g, color.b);
   SDL_Rect screen_rect = {
      0, 0, static_cast<Uint16>(mResX), static_cast<Uint16>(mResY)
   };
   SDL_FillRect(mScreen, &screen_rect, clear_color);
}

void SwRenderer::Draw(const Field& field)
{
   const auto color = SDL_MapRGB(mScreen->format, 0x00, 0x00, 0xff);
   const auto pos_x = mResX * field.mPosX;
   const auto pos_y = mResY * field.mPosY;
   const auto size_x = mResX * field.mSizeX;
   const auto size_y = mResY * field.mSizeY;

   SDL_Rect rect = {
      static_cast<Sint16>(pos_x),
      static_cast<Sint16>(pos_y),
      static_cast<Uint16>(size_x),
      static_cast<Uint16>(size_y)
   };
   SDL_FillRect(mScreen, &rect, color);
}

void SwRenderer::Draw(const Goal& goal)
{
   const auto color = SDL_MapRGB(mScreen->format, 0x00, 0x7f, 0x00);
   const auto pos_x = mResX * goal.mPosX;
   const auto pos_y = mResY * goal.mPosY;
   const auto size_x = mResX * goal.mSizeX;
   const auto size_y = mResY * goal.mSizeY;

   SDL_Rect rect = {
      static_cast<Sint16>(pos_x),
      static_cast<Sint16>(pos_y),
      static_cast<Uint16>(size_x),
      static_cast<Uint16>(size_y)
   };
   SDL_FillRect(mScreen, &rect, color);
}

void SwRenderer::Draw(const Ball& ball)
{
   const auto color = SDL_MapRGB(mScreen->format, 0xff, 0xff, 0x00);
   const auto pos_x = mResX * (ball.mPosX - (ball.mSizeX / 2.f));
   const auto pos_y = mResY * (ball.mPosY - (ball.mSizeY / 2.f));
   const auto size_x = mResX * ball.mSizeX;
   const auto size_y = mResY * ball.mSizeY;

   SDL_Rect rect = {
      static_cast<Sint16>(pos_x),
      static_cast<Sint16>(pos_y),
      static_cast<Uint16>(size_x),
      static_cast<Uint16>(size_y)
   };
   SDL_FillRect(mScreen, &rect, color);
}

void SwRenderer::Draw(const Player& player)
{
   const auto color = SDL_MapRGB(mScreen->format, 0xff, 0xff, 0xff);
   const auto pos_x = mResX * (player.mPosX - (player.mSizeX / 2.f));
   const auto pos_y = mResY * (player.mPosY - (player.mSizeY / 2.f));
   const auto size_x = mResX * player.mSizeX;
   const auto size_y = mResY * player.mSizeY;

   SDL_Rect rect = {
      static_cast<Sint16>(pos_x),
      static_cast<Sint16>(pos_y),
      static_cast<Uint16>(size_x),
      static_cast<Uint16>(size_y)
   };
   SDL_FillRect(mScreen, &rect, color);
}
