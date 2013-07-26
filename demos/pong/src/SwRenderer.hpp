#ifndef SW_RENDERER_HPP
#define SW_RENDERER_HPP

#include "Renderer.hpp"

struct SDL_Color;
struct SDL_Surface;
class Game;
class Field;
class Goal;
class Ball;
class Player;

class SwRenderer : public Renderer
{
public:
   SwRenderer(int res_x, int res_y);
   virtual ~SwRenderer();

   void PreRender() override;
   void DoRender(const Game& game) override;
   void PostRender() override;

   std::string GetName() const override;

private:
   void ClearScreen(const SDL_Color& color);
   void Draw(const Field& field);
   void Draw(const Goal& goal);
   void Draw(const Ball& ball);
   void Draw(const Player& player);

   const int mResX;
   const int mResY;
   SDL_Surface* mScreen = nullptr;
};

#endif // SW_RENDERER_HPP
