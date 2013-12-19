#include "SwRendererMt.hpp"
#include "Slice.hpp"
#include "../../Ray.hpp"
#include "../../Level.hpp"
#include "../../Player.hpp"
#include "../../Utils.hpp"

#include <thread>

SwRendererMt::SwRendererMt(const int res_x, const int res_y, const std::string& app_name, const int threads)
    : SwRenderer(res_x, res_y, app_name, std::string("Software(threads:") + std::to_string(threads) + ")")
    , mThreadCnt{threads}
{

}

SwRendererMt::~SwRendererMt()
{

}

void SwRendererMt::DrawPlayerView(const Level& level, const Player& player)
{
    const int thread_slice = mResX / mThreadCnt;
    std::vector<std::thread> threads(mThreadCnt);

    const double player_pos_x = player.mPosX;
    const double player_pos_y = player.mPosY;
    const double player_dir_x = player.mDirX;
    const double player_dir_y = player.mDirY;
    const double player_plane_x = player.mPlaneX;
    const double player_plane_y = player.mPlaneY;

    auto is_level_blocking = [&](const int map_x, const int map_y)
    {
        return level.IsBlocking(map_x, map_y);
    };

    auto render = [&](int resx_start, int resx_stop)
    {
        for (auto x = resx_start; x < resx_stop; x++)
        {
           // Current column position relative to the center of the screen.
           // Left edge is -1, right edge is 1, and center is 0.
           const double cam_x = 2.0 * x / mResX - 1;

           // Starting direction of the current ray to be cast.
           const double slice_dir_x = player_dir_x + (player_plane_x * cam_x);
           const double slice_dir_y = player_dir_y + (player_plane_y * cam_x);

           const RaycastResult r = cast_ray(player_pos_x, player_pos_y,
                                            slice_dir_x, slice_dir_y,
                                            is_level_blocking);
           auto wall_color = mWallColors[level.GetBlockType(r.map_pos_x, r.map_pos_y)];
           auto pixels = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * x);

           draw_slice(r, mResY, pixels, wall_color, mCeilingColor, mFloorColor);
        }
    };

    for (auto i = 0; i < mThreadCnt; i++)
    {
        const auto slice_start = i * thread_slice;
        const auto slice_stop = slice_start + thread_slice;

        threads[i] = std::thread{ render, slice_start, slice_stop };
    }

    for (auto& t : threads)
    {
        t .join();
    }
}
