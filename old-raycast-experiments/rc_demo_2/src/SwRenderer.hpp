#ifndef SW_RENDERER_HPP
#define SW_RENDERER_HPP

#include "Renderer.hpp"

#include <memory>

struct SDL_Color;
struct SDL_Surface;
class Level;
class Player;
class ResourceCache;

class SwRenderer : public Renderer
{
public:
   SwRenderer(int res_x, int res_y, int threads);
   ~SwRenderer();

   void PreRender() override;
   void DoRender(const Level& level, const Player& player) override;
   void PostRender() override;

   std::string GetName() const override;

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

#endif // SW_RENDERER_HPP
