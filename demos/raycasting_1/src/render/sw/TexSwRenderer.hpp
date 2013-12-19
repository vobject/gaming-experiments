#ifndef TEX_SW_RENDERER_HPP
#define TEX_SW_RENDERER_HPP

#include "SwRenderer.hpp"

#include <memory>

class ResourceCache;

class TexSwRenderer : public SwRenderer
{
public:
    TexSwRenderer(int res_x, int res_y, const std::string& app_name);
    virtual ~TexSwRenderer();

protected:
    virtual void DrawPlayerView(const Level& level, const Player& player);

    std::unique_ptr<const ResourceCache> mResCache;
};

#endif // TEX_SW_RENDERER_HPP
