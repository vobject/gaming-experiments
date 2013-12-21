#include "SwRenderer.hpp"
#include "Slice.hpp"
#include "../../Ray.hpp"
#include "../../World.hpp"
#include "../../Player.hpp"

#include <algorithm>

SwRenderer::SwRenderer(const int res_x, const int res_y, const std::string& app_name)
    : SwRenderer(res_x, res_y, app_name, "Software")
{

}

SwRenderer::SwRenderer(const int res_x, const int res_y, const std::string& app_name, const std::string& renderer_name)
    : Renderer(res_x, res_y, app_name, renderer_name)
{
    Startup();
}

SwRenderer::~SwRenderer()
{
    Shutdown();
}

void SwRenderer::Startup()
{
    Renderer::Startup();

    mRenderer = SDL_CreateRenderer(mScreen, -1, 0);
    if (!mRenderer) {
        throw "SDL_CreateRenderer() failed.";
    }

    mTexture = SDL_CreateTexture(mRenderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 mResY, mResX);
    if (!mTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mSurface = SDL_CreateRGBSurface(0, mResY, mResX, 32, 0, 0, 0, 0);
    if (!mSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }

    // initialize colors for ceiling and floor
    mCeilingColor = SDL_MapRGBA(mSurface->format, 128, 128, 128, 0); // ceiling
    mFloorColor = SDL_MapRGBA(mSurface->format, 96, 96, 96, 0); // floor

    // initialize wall colors
    mWallColors[0] = SDL_MapRGBA(mSurface->format, 0, 0, 0, 0); // black (fallback)
    mWallColors[1] = SDL_MapRGBA(mSurface->format, 255, 0, 0, 0); // red
    mWallColors[2] = SDL_MapRGBA(mSurface->format, 0, 255, 0, 0); // green
    mWallColors[3] = SDL_MapRGBA(mSurface->format, 0, 0, 255, 0); // blue
    mWallColors[4] = SDL_MapRGBA(mSurface->format, 255, 255, 255, 0); // white
    mWallColors[5] = SDL_MapRGBA(mSurface->format, 255, 255, 0, 0); // yellow
}

void SwRenderer::Shutdown()
{
    if (mMinimapSurface) {
        SDL_FreeSurface(mMinimapSurface);
        mMinimapSurface = nullptr;
    }

    if (mMinimapTexture) {
        SDL_DestroyTexture(mMinimapTexture);
        mMinimapTexture = nullptr;
    }

    if (mSurface) {
        SDL_FreeSurface(mSurface);
        mSurface = nullptr;
    }

    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }

    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }

    Renderer::Shutdown();
}

void SwRenderer::PreRender()
{
    Renderer::PreRender();
}

void SwRenderer::PostRender()
{
    auto res_diff = (std::max(mResX, mResY) - std::min(mResX, mResY)) / 2;
    SDL_Rect dest = { res_diff, -res_diff, mResY, mResX };

    SDL_UpdateTexture(mTexture, nullptr, mSurface->pixels, mSurface->pitch);
    SDL_RenderCopyEx(mRenderer, mTexture, nullptr, &dest, -90.0, nullptr, SDL_FLIP_NONE);

    if (mMinimapTexture)
    {
        SDL_UpdateTexture(mMinimapTexture, nullptr, mMinimapSurface->pixels, mMinimapSurface->w * sizeof(Uint32));
        SDL_Rect mmDst = { 0,
                           0,
                           mMinimapSurface->w * 4,
                           mMinimapSurface->h * 4 };
        SDL_RenderCopy(mRenderer, mMinimapTexture, nullptr, &mmDst);
    }

    SDL_RenderPresent(mRenderer);

    Renderer::PostRender();
}

void SwRenderer::DoRender(const World& level, const Player& player)
{
    DrawPlayerView(level, player);
    DrawMinimap(level, player);
}

void SwRenderer::InitMinimap(const World& level)
{
    const auto width = level.GetWidth();
    const auto height = level.GetHeight();

    mMinimapTexture = SDL_CreateTexture(mRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        width, height);
    if (!mMinimapTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mMinimapSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!mMinimapSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }

    // initialize minimap colors
    mMinimapFloorColor = SDL_MapRGBA(mMinimapSurface->format, 255, 255, 255, 0);
    mMinimapWallColor = SDL_MapRGBA(mMinimapSurface->format, 0, 0, 0, 0);
    mMinimapPlayerColor = SDL_MapRGBA(mMinimapSurface->format, 255, 128, 128, 0);
}

void SwRenderer::DrawPlayerView(const World& level, const Player& player)
{
    auto is_level_blocking = [&](const int map_x, const int map_y) {
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
        auto wall_color = mWallColors[level.GetCellType(r.map_pos_x, r.map_pos_y)];
        auto slice = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * x);

        draw_slice(r, mResY, slice, wall_color, mCeilingColor, mFloorColor);
    }
}

void SwRenderer::DrawMinimap(const World& level, const Player& player)
{
    if (!mMinimapTexture) {
        InitMinimap(level);
    }

    const auto cells_x = level.GetWidth();
    const auto cells_y = level.GetHeight();
    const int player_cell_x = static_cast<int>(player.mPosY);
    const int player_cell_y = static_cast<int>(player.mPosX);

    auto const pixels = static_cast<Uint32*>(mMinimapSurface->pixels);

    for (int cell_y = 0; cell_y < cells_y; cell_y++)
    {
        const auto offset_y = mMinimapSurface->w * cell_y;
        const auto pixels_y = pixels + offset_y;

        for (int cell_x = 0; cell_x < cells_x; cell_x++)
        {
            auto bufp = pixels_y + cell_x;

            if (level.GetCellType(cell_y, cell_x) != 0)
            {
                // This cell is a wall. Mark it on the minimap.
                *bufp = mMinimapWallColor;
            }
            else if ((cell_y == player_cell_y) && (cell_x == player_cell_x))
            {
                // Draw player to minimap.
                *bufp = mMinimapPlayerColor;
            }
            else
            {
                *bufp = mMinimapFloorColor;
            }
        }
    }
}
