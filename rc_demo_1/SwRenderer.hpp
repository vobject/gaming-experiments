#ifndef SW_RENDERER_HPP
#define SW_RENDERER_HPP

#include "Renderer.hpp"

struct SDL_Color;
struct SDL_Surface;
class Level;
class Player;

class SwRenderer : public Renderer
{
public:
   SwRenderer(int res_x, int res_y);
   virtual ~SwRenderer();

   void PreRender() override;
   void DoRender(const Level& level, const Player& player) override;
   void PostRender() override;

   std::string GetName() const override;

private:
   void DrawCeiling(SDL_Color color);
   void DrawFloor(SDL_Color color);

   void DrawPlayerView(const Level& level, const Player& player);
   void DrawMinimap(const Level& level, const Player& player);

   void DrawVerticalLine(int x, int y1, int y2, SDL_Color color);

   const int mResX;
   const int mResY;
   SDL_Surface* mScreen = nullptr;
};

#endif // SW_RENDERER_HPP
