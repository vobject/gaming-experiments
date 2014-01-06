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
    mResCache = Utils::make_unique<ResourceCache>("res", mResX, mResY,
                                                  mSurface->format->format);
}

TexSwRenderer::~TexSwRenderer()
{

}

void TexSwRenderer::Startup()
{
    SwRenderer::Startup();
}

void TexSwRenderer::Shutdown()
{
//     if (mSkyTexture) {
//         SDL_DestroyTexture(mSkyTexture);
//         mSkyTexture = nullptr;
//     }

    SwRenderer::Shutdown();
}

void TexSwRenderer::DrawPlayerView(const World& world, const Player& player)
{
//     DrawSky(player);

    const auto rays = player.GetRaycastResults();

    for (auto x = 0; x < mResX; x++)
    {
        const auto& ray = rays[x];

        const double cam_x = 2.0 * x / mResX - 1;
        const double ray_dir_x = player.mDirX + (player.mPlaneX * cam_x);
        const double ray_dir_y = player.mDirY + (player.mPlaneY * cam_x);

        // FIXME: passing non-zero mVerticalLook crashes draw_slice()

        draw_slice(ray, x, mResY, player.mPosX, player.mPosY, ray_dir_x, ray_dir_y,
            0, mSurface, world, *mResCache);
    }
}

// void TexSwRenderer::InitSky(const SDL_Surface* const sky_tex)
// {
//     mSkyTexture = SDL_CreateTexture(mRenderer,
//                                     SDL_PIXELFORMAT_ARGB8888,
//                                     SDL_TEXTUREACCESS_STREAMING,
//                                     sky_tex->w, sky_tex->h);
//     if (!mSkyTexture) {
//         throw "SDL_CreateTexture() failed.";
//     }
// 
//     SDL_UpdateTexture(mSkyTexture, nullptr, sky_tex->pixels, sky_tex->pitch);
// }
// 
// void TexSwRenderer::DrawSky(const Player& player)
// {
//     const auto sky_tex = mResCache->GetSky(0);
// 
//     if (!mSkyTexture) {
//         InitSky(sky_tex);
//     }
// 
//     const int sky_x = sky_tex->w - (sky_tex->w / 360.) * player.GetRotation();
//     int sky_width = mResX;
//     int left_over = 0;
// 
//     if ((sky_x + sky_width) > sky_tex->w)
//     {
//         left_over = (sky_x + mResX) - sky_tex->w;
//         sky_width -= left_over;
//     }
// 
// 
//     if (sky_width > 0)
//     {
//         SDL_Rect sky_src = { sky_x, 0, sky_width, mResY / 2 };
//         SDL_Rect sky_dst = { 0, 0, sky_width, mResY / 2 };
//         SDL_RenderCopy(mRenderer, mSkyTexture, &sky_src, &sky_dst);
//     }
// 
//     if (left_over > 0)
//     {
//         SDL_Rect sky_src = { 0, 0, left_over, mResY / 2 };
//         SDL_Rect sky_dst = { sky_width, 0, left_over, mResY / 2 };
//         SDL_RenderCopy(mRenderer, mSkyTexture, &sky_src, &sky_dst);
//     }
// }

#endif // WITH_TEXTURE
