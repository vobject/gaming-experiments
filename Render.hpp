#ifndef RENDER_HPP
#define RENDER_HPP

#include <cmath>

struct SDL_Color;
struct SDL_Surface;
class Level;
class Player;

class Render
{
public:
   Render(int res_x, int res_y);
   ~Render();

   void PreRender();
   void DoRender(const Level& level, const Player& player);
   void PostRender();

private:
   void DrawCeiling(SDL_Color color);
   void DrawFloor(SDL_Color color);

   void DrawPlayerView(const Level& level, const Player& player);
   void DrawPlayerView2(const Level& level, const Player& player);
   void DrawMinimap(const Level& level, const Player& player);

   void DrawVerticalLine(int x, int y1, int y2, SDL_Color color);
   void CastRay(double angle, unsigned int strip, const Level& level, const Player& player);

   const int mResX;
   const int mResY;
   SDL_Surface* mScreen = nullptr;

   // New stuff.
   const double mFieldOfVision = 90. * M_PI / 180;
   const double mViewDistance = (mResX / 2) / std::tan((mFieldOfVision / 2));
   const unsigned int mStrip = 1;
   const unsigned int mRayCount = std::ceil(mResX / mStrip);
};

#endif // RENDER_HPP
