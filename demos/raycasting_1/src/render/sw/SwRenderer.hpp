#ifndef SW_RENDERER_HPP
#define SW_RENDERER_HPP

#include "../Renderer.hpp"

#include <SDL.h>

class World;
class Player;
struct lua_State;

class SwRenderer : public Renderer
{
public:
    SwRenderer(lua_State* L);
    SwRenderer(lua_State* L, const std::string& renderer_name);
    virtual ~SwRenderer();

    void Startup() override;
    void Shutdown() override;

    void PreRender() override;
    void PostRender() override;
    void DoRender(const World& world) override;

protected:
    void InitMinimap(const World& world);

    virtual void DrawPlayerView(const World& world, const Player& player);
    virtual void DrawMinimap(const World& world, const Player& player);

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
    Uint32 mMinimapRayColor = 0;
};

#endif // SW_RENDERER_HPP
