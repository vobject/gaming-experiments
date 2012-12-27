#ifndef RENDER_HPP
#define RENDER_HPP

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
   void DrawVerticalLine(int x, int y1, int y2, SDL_Color color);

   const int mResX;
   const int mResY;
   SDL_Surface* mScreen = nullptr;
};

#endif // RENDER_HPP
