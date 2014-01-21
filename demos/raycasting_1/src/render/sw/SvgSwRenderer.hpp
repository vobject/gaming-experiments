#ifndef SVG_SW_RENDERER_HPP
#define SVG_SW_RENDERER_HPP

#ifdef WITH_SVG

#include "../Renderer.hpp"

#include <SDL.h>

class Player;

class SvgSwRenderer : public Renderer
{
public:
    SvgSwRenderer();
    SvgSwRenderer(const std::string& renderer_name);
    virtual ~SvgSwRenderer();

    void Startup() override;
    void Shutdown() override;

    void PreRender() override;
    void PostRender() override;
    void DoRender(const World& level, const Player& player) override;

protected:
    void InitMinimap(const World& level);

    virtual void DrawPlayerView(const World& level, const Player& player);
    virtual void DrawMinimap(const World& level, const Player& player);

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

#endif // WITH_SVG

#endif // SVG_SW_RENDERER_HPP
