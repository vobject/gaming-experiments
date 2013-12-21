#include "TexSwRenderer.hpp"
#include "ResourceCache.hpp"
#include "Slice.hpp"
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

void TexSwRenderer::DrawPlayerView(const World& level, const Player& player)
{
    auto is_level_blocking = [&](const int map_x, const int map_y)
    {
        return level.IsBlocking(map_x, map_y);
    };

    for (auto x = 0; x < mResX; x++)
    {
        // Current column position relative to the center of the screen.
        // Left edge is -1, right edge is 1, and center is 0.
        const double cam_x = 2.0 * x / mResX - 1;

        // Starting direction of the current ray to be cast.
        const double ray_dir_x = player.mDirX + (player.mPlaneX * cam_x);
        const double ray_dir_y = player.mDirY + (player.mPlaneY * cam_x);

        const RaycastResult r = cast_ray(player.mPosX, player.mPosY,
                                         ray_dir_x, ray_dir_y,
                                         is_level_blocking);
        //auto wall_color = mWallColors[level.GetBlockType(r.map_pos_x, r.map_pos_y)];
        //auto slice = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * x);
        //draw_slice(r, mResY, slice, wall_color, mCeilingColor, mFloorColor);

        draw_slice(r, x, mResY, player.mPosX, player.mPosY, ray_dir_x, ray_dir_y,
            mSurface, level, *mResCache);
    }


    //const Vector player_pos = player.GetPosition();
    //const Vector player_dir = player.GetDirection();
    //const Vector player_plane = player.GetPlane();

    //for (auto x = 0; x < mResX; x++)
    //{
    //   // Current column position relative to the center of the screen.
    //   // Left edge is -1, right edge is 1, and center is 0.
    //   const double cam_x = 2. * x / mScreen->w - 1;

    //   // Starting direction of the current ray to be cast.
    //   const auto ray_dir = player_dir + (player_plane * cam_x);

    //   const Ray ray(player_pos, ray_dir, level);

    //   Slice slice (mScreen, x);
    //   slice.Draw(ray, player_pos, ray_dir, level, *mResCache);
    //}
}
