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

protected:
    virtual void DrawPlayerView(const World& world, const Player& player);

    std::unique_ptr<const ResourceCache> mResCache;
};

#endif // WITH_TEXTURE

#endif // TEX_SW_RENDERER_HPP
