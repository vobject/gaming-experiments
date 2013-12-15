#include "SwRenderer.hpp"
#include "../../Level.hpp"
#include "../../Player.hpp"

#include <cmath>

static Uint32 CEILING_COLOR;
static Uint32 FLOOR_COLOR;
static Uint32 WALL_COLORS[5 + 1]; // 5 colors + 1 fallback

SwRenderer::SwRenderer(const int res_x, const int res_y, const std::string& app_name)
    : Renderer(res_x, res_y, app_name)
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
    CEILING_COLOR = SDL_MapRGBA(mSurface->format, 128, 128, 128, 0); // ceiling
    FLOOR_COLOR = SDL_MapRGBA(mSurface->format, 96, 96, 96, 0); // floor

    // initialize wall colors
    WALL_COLORS[0] = SDL_MapRGBA(mSurface->format, 0, 0, 0, 0); // black (fallback)
    WALL_COLORS[1] = SDL_MapRGBA(mSurface->format, 255, 0, 0, 0); // red
    WALL_COLORS[2] = SDL_MapRGBA(mSurface->format, 0, 255, 0, 0); // green
    WALL_COLORS[3] = SDL_MapRGBA(mSurface->format, 0, 0, 255, 0); // blue
    WALL_COLORS[4] = SDL_MapRGBA(mSurface->format, 255, 255, 255, 0); // white
    WALL_COLORS[5] = SDL_MapRGBA(mSurface->format, 255, 255, 0, 0); // yellow
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

void SwRenderer::DoRender(const Level& level, const Player& player)
{
    DrawPlayerView(level, player);
    DrawMinimap(level, player);
}

const std::string& SwRenderer::GetName() const
{
    static std::string name("Software");
    return name;
}

void SwRenderer::InitMinimap(const Level& level)
{
    mMinimapTexture = SDL_CreateTexture(mRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        level.GetWidth(), level.GetHeight());
    if (!mMinimapTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mMinimapSurface = SDL_CreateRGBSurface(0, level.GetWidth(), level.GetHeight(),
                                           32, 0, 0, 0, 0);
    if (!mMinimapSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }
}

void SwRenderer::DrawPlayerView(const Level& level, const Player& player)
{
    for (auto x = 0; x < mResX; x++)
    {
        // Current column position relative to the center of the screen.
        // Left edge is -1, right edge is 1, and center is 0.
        const double cam_x = 2. * x / mResX - 1;

        // Starting direction and  position of the current ray to be cast.
        const double ray_dir_x = player.mDirX + player.mPlaneX * cam_x;
        const double ray_dir_y = player.mDirY + player.mPlaneY * cam_x;
        const double ray_pos_x = player.mPosX;
        const double ray_pos_y = player.mPosY;

        // The direction to step in X or Y-direction (either +1 or -1).
        const int step_x = (ray_dir_x >= 0) ? 1 : -1;
        const int step_y = (ray_dir_y >= 0) ? 1 : -1;

        // Length of ray from one X and Y-side to next X and Y-side.
        const double delta_dist_x = std::sqrt(1 + std::pow(ray_dir_y, 2) / std::pow(ray_dir_x, 2));
        const double delta_dist_y = std::sqrt(1 + std::pow(ray_dir_x, 2) / std::pow(ray_dir_y, 2));

        // The player's current grid position inside the level.
        int map_x = ray_pos_x;
        int map_y = ray_pos_y;

        // Length of ray from its current position to next X- and Y-side.
        double side_dist_x = (step_x == 1) ?
                                ((map_x + 1.0 - ray_pos_x) * delta_dist_x) :
                                ((ray_pos_x - map_x) * delta_dist_x);
        double side_dist_y = (step_y == 1) ?
                                ((map_y + 1.0 - ray_pos_y) * delta_dist_y) :
                                ((ray_pos_y - map_y) * delta_dist_y);

        // Y walls (EW) will be drawn darker.
        bool y_side_hit;

        for (bool wall_hit = false; !wall_hit;) // Run the DDA algorithm.
        {
            if (side_dist_x < side_dist_y)
            {
                // Jump one square in X-direction.
                map_x += step_x;
                side_dist_x += delta_dist_x;
                y_side_hit = false;
            }
            else
            {
                // Jump one square in Y-direction.
                map_y += step_y;
                side_dist_y += delta_dist_y;
                y_side_hit = true;
            }

            // Check if the ray has hit a wall.
            wall_hit = level.GetBlockType(map_x, map_y) > 0;
        }

        // Calculate the perpendicular distance projected on camera direction.
        // Oblique distance would give fisheye effect.
        const double perp_wall_dist = y_side_hit ?
               std::abs((map_y - ray_pos_y + (1 - step_y) / 2) / ray_dir_y) :
               std::abs((map_x - ray_pos_x + (1 - step_x) / 2) / ray_dir_x);

        // Calculate the height of the vertical line to draw on screen.
        const double line_height_d = std::abs(mResY / perp_wall_dist);
        const int line_height = line_height_d;

        // FIXME: line_height might be negative when the player is near a wall
        //  because perp_wall_dist was so small. This will crash later.

        // Set where the vertical line should be drawn.
        int line_start = (-line_height / 2) + (mResY / 2);
        int line_end = line_height / 2 + mResY / 2;

        if (line_start < 0) {
            line_start = 0;
        }

        if (line_end >= mResY) {
            line_end = mResY - 1;
        }

        // Choose wall color (same as wall tye atm).
        Uint32 wall_color = WALL_COLORS[level.GetBlockType(map_x, map_y)];

        if (y_side_hit)
        {
            // Give X and Y-sides different brightness.
            wall_color = ((Uint8)((Uint8)(wall_color >> 24) * .7f) << 24) +
                         ((Uint8)((Uint8)(wall_color >> 16) * .7f) << 16) +
                         ((Uint8)((Uint8)(wall_color >>  8) * .7f) <<  8) +
                         ((Uint8)((Uint8)(wall_color >>  0) * .7f) <<  0);
        }

        // Draw the pixels of the stripe as a vertical line.
        DrawVerticalLine(x, line_start, line_end, wall_color);
    }
}

void SwRenderer::DrawMinimap(const Level& level, const Player& player)
{
    if (!mMinimapTexture) {
        InitMinimap(level);
    }

    const auto cells_x = level.GetWidth();
    const auto cells_y = level.GetHeight();
    const unsigned int player_cell_x = player.mPosY;
    const unsigned int player_cell_y = player.mPosX;

    const auto color_floor = SDL_MapRGBA(mMinimapSurface->format, 255, 255, 255, 0);
    const auto color_wall = SDL_MapRGBA(mMinimapSurface->format, 0, 0, 0, 0);
    const auto color_player = SDL_MapRGBA(mMinimapSurface->format, 255, 128, 128, 0);

    auto const pixels = static_cast<Uint32*>(mMinimapSurface->pixels);

    for (unsigned int cell_y = 0; cell_y < cells_y; cell_y++)
    {
        const auto offset_y = mMinimapSurface->w * cell_y;

        for (unsigned int cell_x = 0; cell_x < cells_x; cell_x++)
        {
            auto bufp = pixels + offset_y + cell_x;

            if (level.GetBlockType(cell_y, cell_x) != 0)
            {
                // This cell is a wall. Mark it on the minimap.
                *bufp = color_wall;
            }
            else if ((cell_y == player_cell_y) && (cell_x == player_cell_x))
            {
                // Draw player to minimap.
                *bufp = color_player;
            }
            else
            {
                *bufp = color_floor;
            }
        }
    }
}

void SwRenderer::DrawVerticalLine(
    const int x,
    const int y_start,
    const int y_end,
    const Uint32 wall_color
)
{
//    if (y_end < y_start)
//    {
//        // Make sure y_end is always greater than y_start.
//         std::swap(y_start, y_end);
//    }

//    if (y_end < 0 || y_start >= mResY || x < 0 || x >= mResX) {
//        return; // Not a single point of the line is on screen.
//    }

    auto bufp = static_cast<Uint32*>(mSurface->pixels) + (mSurface->w * x);

    for (int y = 0; y < mResX; y++)
    {
        if (y < y_start)
        {
            *bufp = CEILING_COLOR;
        }
        else if (y > y_end)
        {
            *bufp = FLOOR_COLOR;
        }
        else
        {
            *bufp = wall_color;
        }
        bufp++;
    }
}
