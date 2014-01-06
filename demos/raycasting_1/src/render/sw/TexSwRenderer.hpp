#ifndef TEX_SW_RENDERER_HPP
#define TEX_SW_RENDERER_HPP

#ifdef WITH_TEXTURE

#include "SwRenderer.hpp"

#include <memory>

class ResourceCache;

class TexSwRenderer : public SwRenderer
{
public:
    TexSwRenderer(int res_x, int res_y, const std::string& app_name);
    virtual ~TexSwRenderer();

    void Startup() override;
    void Shutdown() override;

protected:
    void DrawPlayerView(const World& world, const Player& player) override;

//     void InitSky(const SDL_Surface* const sky_tex);
//     virtual void DrawSky(const Player& player);
// 
//     SDL_Texture* mSkyTexture = nullptr;

    std::unique_ptr<const ResourceCache> mResCache;
};

#endif // WITH_TEXTURE

#endif // TEX_SW_RENDERER_HPP
