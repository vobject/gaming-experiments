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
    virtual ~SwRenderer();

    void Startup() override;
    void Shutdown() override;

    void PreRender() override;
    void PostRender() override;
    void DoRender(const Level& level, const Player& player) override;

private:
    void InitMinimap(const Level& level);

    void DrawPlayerView(const Level& level, const Player& player);
    void DrawMinimap(const Level& level, const Player& player);

    void DrawVerticalLine(int x, int y1, int y2, Uint32 wall_color);

    SDL_Renderer* mRenderer = nullptr;
    SDL_Texture* mTexture = nullptr;
    SDL_Surface* mSurface = nullptr;

    SDL_Texture* mMinimapTexture = nullptr;
    SDL_Surface* mMinimapSurface = nullptr;
};

#endif // SW_RENDERER_HPP
