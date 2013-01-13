#ifndef RENDER_HPP
#define RENDER_HPP

#include <SDL.h>

#include <memory>

class Level;
class Player;
class ResourceCache;

class Render
{
public:
   Render(int res_x, int res_y, int threads);
   ~Render();

   void PreRender();
   void DoRender(const Level& level, const Player& player);
   void PostRender();

private:
//   void DrawCeiling(SDL_Color color);
//   void DrawFloor(SDL_Color color);

   void DrawPlayerView(const Level& level, const Player& player);
   void DrawMinimap(const Level& level, const Player& player);

//   void DrawVerticalLine(int x, int y1, int y2, SDL_Color color);

//   void SetPixel(int x, int y, SDL_Color color);

   const int mResX;
   const int mResY;
   const int mThreadCnt;
   SDL_Surface* mScreen = nullptr;

   // Must be initialized after the video system has been set up.
   std::unique_ptr<const ResourceCache> mResCache;
};

#endif // RENDER_HPP
