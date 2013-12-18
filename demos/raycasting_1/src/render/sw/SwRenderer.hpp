#ifndef SW_RENDERER_HPP
#define SW_RENDERER_HPP

#include "../Renderer.hpp"

#include <SDL.h>

class Level;
class Player;

class SwRenderer : public Renderer
{
public:
    SwRenderer(int res_x, int res_y, const std::string& app_name);
    SwRenderer(int res_x, int res_y, const std::string& app_name, const std::string& renderer_name);
    virtual ~SwRenderer();

    void Startup() override;
    void Shutdown() override;

    void PreRender() override;
    void PostRender() override;
    void DoRender(const Level& level, const Player& player) override;

protected:
    void InitMinimap(const Level& level);

    virtual void DrawPlayerView(const Level& level, const Player& player);
    virtual void DrawMinimap(const Level& level, const Player& player);

    SDL_Renderer* mRenderer = nullptr;
    SDL_Texture* mTexture = nullptr;
    SDL_Surface* mSurface = nullptr;

    SDL_Texture* mMinimapTexture = nullptr;
    SDL_Surface* mMinimapSurface = nullptr;

    // things that should go into some sort of resource cache
    Uint32 mCeilingColor = 0;
    Uint32 mFloorColor = 0;
    Uint32 mWallColors[5 + 1]; // 5 colors + 1 fallback

    Uint32 mMinimapFloorColor = 0;
    Uint32 mMinimapWallColor = 0;
    Uint32 mMinimapPlayerColor = 0;
};

#endif // SW_RENDERER_HPP
