#ifdef WITH_TEXTURE

#include "TexSwRenderer.hpp"
#include "Slice.hpp"
#include "../ResourceCache.hpp"
#include "../../Ray.hpp"
#include "../../World.hpp"
#include "../../Player.hpp"
#include "../../Utils.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <sstream>
#include <cmath>

TexSwRenderer::TexSwRenderer(const int res_x, const int res_y, const std::string& app_name)
    : SwRenderer(res_x, res_y, app_name, "TexSoftware")
{
    mResCache = Utils::make_unique<ResourceCache>("res", mSurface->format->format);
}

TexSwRenderer::~TexSwRenderer()
{

}

void TexSwRenderer::DrawPlayerView(const World& world, const Player& player)
{
    const auto rays = player.GetRaycastResults();

    for (auto x = 0; x < mResX; x++)
    {
        const auto& ray = rays[x];

        const double cam_x = 2.0 * x / mResX - 1;
        const double ray_dir_x = player.mDirX + (player.mPlaneX * cam_x);
        const double ray_dir_y = player.mDirY + (player.mPlaneY * cam_x);

        draw_slice(ray, x, mResY, player.mPosX, player.mPosY, ray_dir_x, ray_dir_y,
            mSurface, world, *mResCache);
    }
}

#endif // WITH_TEXTURE
