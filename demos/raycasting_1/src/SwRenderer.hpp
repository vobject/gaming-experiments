#ifndef SW_RENDERER_HPP
#define SW_RENDERER_HPP

#include "Renderer.hpp"

#include <SDL.h>

#include <vector>

class Level;
class Player;

class SwRenderer : public Renderer
{
public:
   SwRenderer(const std::string& app_name, int res_x, int res_y);
   virtual ~SwRenderer();

   void PreRender() override;
   void DoRender(const Level& level, const Player& player) override;
   void PostRender() override;

private:
   void DrawCeiling(SDL_Color color);
   void DrawFloor(SDL_Color color);

   void DrawPlayerView(const Level& level, const Player& player);
   void DrawMinimap(const Level& level, const Player& player);

   void DrawVerticalLine(int x, int y1, int y2, SDL_Color color);

   const int mResX;
   const int mResY;
   SDL_Window* mScreen = nullptr;
   SDL_Renderer* mRenderer = nullptr;
   SDL_Texture* mTexture = nullptr;
   SDL_Surface* mSurface = nullptr;
};

#endif // SW_RENDERER_HPP
