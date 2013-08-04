#ifndef MT_SW_RENDERER_HPP
#define MT_SW_RENDERER_HPP

#ifdef WITH_MTSWRENDERER

#include "SwRenderer.hpp"

#include <memory>

struct SDL_Color;
struct SDL_Surface;
class Level;
class Player;
class ResourceCache;

class MtSwRenderer : public SwRenderer
{
public:
   MtSwRenderer(int res_x, int res_y, int threads);
   virtual ~MtSwRenderer();

   std::string GetName() const override;

private:
   void DrawPlayerView(const Level& level, const Player& player) override;

   const int mThreadCnt;
};

#endif // WITH_MTSWRENDERER

#endif // MT_SW_RENDERER_HPP
