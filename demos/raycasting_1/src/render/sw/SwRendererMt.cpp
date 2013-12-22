#include "SwRendererMt.hpp"
#include "Slice.hpp"
#include "../../Ray.hpp"
#include "../../World.hpp"
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

void SwRendererMt::DrawPlayerView(const World& world, const Player& player)
{
    const int thread_slice = mResX / mThreadCnt;
    std::vector<std::thread> threads(mThreadCnt);

    const auto rays = player.GetRaycastResults();

    auto render = [&](int resx_start, int resx_stop)
    {
        for (auto x = resx_start; x < resx_stop; x++)
        {
            const auto& ray = rays[x];

            const auto wall_type = world.GetCellType(ray.map_pos_x, ray.map_pos_y);
            const auto wall_color = mWallColors[wall_type];
            auto slice = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * x);

            draw_slice(ray, mResY, slice, wall_color, mCeilingColor, mFloorColor);
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
