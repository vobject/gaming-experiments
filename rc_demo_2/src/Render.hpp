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
   void DrawSky(const Player& player);
   void DrawPlayerView(const Level& level, const Player& player);
   void DrawMinimap(const Level& level, const Player& player);

   const int mResX;
   const int mResY;
   const int mThreadCnt;
   SDL_Surface* mScreen = nullptr;

   // Must be initialized after the video system has been set up.
   std::unique_ptr<const ResourceCache> mResCache;
};

#endif // RENDER_HPP
