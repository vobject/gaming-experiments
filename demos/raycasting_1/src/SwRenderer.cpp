#include "SwRenderer.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <cmath>

SwRenderer::SwRenderer(const std::string& app_name, const int res_x, const int res_y)
    : Renderer(app_name)
    , mResX(res_x)
    , mResY(res_y)
{
    mScreen = SDL_CreateWindow(app_name.c_str(),
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               mResX, mResY, SDL_WINDOW_RESIZABLE);
    if (!mScreen) {
        throw "SDL_CreateWindow() failed.";
    }

    mRenderer = SDL_CreateRenderer(mScreen, -1, 0);
    if (!mRenderer) {
        throw "SDL_CreateRenderer() failed.";
    }

    mTexture = SDL_CreateTexture(mRenderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 mResX, mResY);
    if (!mTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mSurface = SDL_CreateRGBSurface(0, mResX, mResY, 32, 0, 0, 0, 0);
    if (!mSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }
}

SwRenderer::~SwRenderer()
{
    SDL_FreeSurface(mSurface);
    SDL_DestroyTexture(mTexture);
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mScreen);
}

void SwRenderer::PreRender()
{

}

void SwRenderer::DoRender(const Level& level, const Player& player)
{
    DrawCeiling({ 0x60, 0x60, 0x60, 0x00 });
    DrawFloor({ 0x80, 0x80, 0x80, 0x00 });
    DrawPlayerView(level, player);
    DrawMinimap(level, player);
}

void SwRenderer::PostRender()
{
    SDL_UpdateTexture(mTexture, nullptr, mSurface->pixels, mResX * sizeof(Uint32));
    SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
    SDL_RenderPresent(mRenderer);

    mFrameCount++;
    if (mRefreshCaption) {
        SDL_SetWindowTitle(mScreen, mCaption.c_str());
        mRefreshCaption = false;
    }
}

void SwRenderer::DrawCeiling(const SDL_Color color)
{
    const auto ceiling = SDL_MapRGB(mSurface->format, color.r, color.g, color.b);
    SDL_Rect ceiling_rect = { 0,
                              0,
                              static_cast<Uint16>(mResX),
                              static_cast<Uint16>(mResY / 2) };
    SDL_FillRect(mSurface, &ceiling_rect, ceiling);
}

void SwRenderer::DrawFloor(const SDL_Color color)
{
    const auto floor = SDL_MapRGB(mSurface->format, color.r, color.g, color.b);
    SDL_Rect floor_rect = { 0,
                            static_cast<Sint16>(mResY / 2),
                            static_cast<Uint16>(mResX),
                            static_cast<Uint16>(mResY / 2) };
    SDL_FillRect(mSurface, &floor_rect, floor);
}

void SwRenderer::DrawPlayerView(const Level& level, const Player& player)
{
    for (auto x = 0; x < mResX; x++)
    {
      // Current column position relative to the center of the screen.
      // Left edge is -1, right edge is 1, and center is 0.
      const double cam_x = 2. * x / mResX - 1;

      // Starting direction and  positionof the current ray to be cast.
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
         wall_hit = level.mGrid[map_x][map_y] > 0;
      }

      // Calculate the perpendicular distance projected on camera direction.
      // Oblique distance would give fisheye effect.
      const double perp_wall_dist = y_side_hit ?
                                 std::fabs((map_y - ray_pos_y + (1 - step_y) / 2) / ray_dir_y) :
                                 std::fabs((map_x - ray_pos_x + (1 - step_x) / 2) / ray_dir_x);

      // Calculate the height of the vertical line to draw on screen.
      const int line_height = std::abs(int(mResY / perp_wall_dist));

      // Set where the vertical line should be drawn.
      int line_start = (-line_height / 2) + (mResY / 2);
      int line_end = line_height / 2 + mResY / 2;

      if (line_start < 0) {
         line_start = 0;
      }

      if (line_end >= mResY) {
         line_end = mResY - 1;
      }

      // Choose wall color.
      SDL_Color color;

      switch(level.mGrid[map_x][map_y])
      {
         case 1:  color = { 0xff, 0x00, 0x00, 0x00 }; break; // red
         case 2:  color = { 0x00, 0xff, 0x00, 0x00 }; break; // green
         case 3:  color = { 0x00, 0x00, 0xff, 0x00 }; break; // blue
         case 4:  color = { 0xff, 0xff, 0xff, 0x00 }; break; // white
         case 5:  color = { 0xff, 0xff, 0x00, 0x00 }; break; // yellow
         default: color = { 0x30, 0x30, 0x30, 0x00 }; break; // dark gray
      }

      if (y_side_hit)
      {
         // Give X and Y-sides different brightness.
         color.r = color.r * .7f;
         color.g = color.g * .7f;
         color.b = color.b * .7f;
      }

      // Draw the pixels of the stripe as a vertical line.
      DrawVerticalLine(x, line_start, line_end, color);
    }
}

void SwRenderer::DrawMinimap(const Level& level, const Player& player)
{
    const auto cells_x = level.mGrid.at(0).size();
    const auto cells_y = level.mGrid.size();

    const Uint16 cell_size_x = (mResX / 4) / cells_x;
    const Uint16 cell_size_y = (mResY / 4) / cells_y;
    const Uint16 map_size_x = cell_size_x * cells_x;
    const Uint16 map_size_y = cell_size_y * cells_y;
    const Sint16 pos_x = mResX - map_size_x;
    const Sint16 pos_y = mResY - map_size_y;

    const auto color_floor = SDL_MapRGB(mSurface->format, 0xff, 0xff, 0xff);
    const auto color_wall = SDL_MapRGB(mSurface->format, 0x5f, 0x5f, 0x5f);
    const auto color_player = SDL_MapRGB(mSurface->format, 0xff, 0x1f, 0x1f);

    // Draw the minimap area.
    SDL_Rect map_rect = { pos_x, pos_y, map_size_x, map_size_y };
    SDL_FillRect(mSurface, &map_rect, color_floor);

    // Draw the individual walls to the minimap.
    SDL_Rect wall_rect = { map_rect.x, map_rect.y, cell_size_x, cell_size_y };
    const unsigned int player_cell_x = player.mPosY;
    const unsigned int player_cell_y = player.mPosX;

    for (unsigned int cell_y = 0; cell_y < level.mGrid.size(); cell_y++)
    {
        for (unsigned int cell_x = 0; cell_x < level.mGrid[cell_y].size(); cell_x++)
        {
            if (level.mGrid[cell_y][cell_x] != 0)
            {
                //Draw only cells of wall type.
                SDL_FillRect(mSurface, &wall_rect, color_wall);
            }

            if ((cell_y == player_cell_y) && (cell_x == player_cell_x))
            {
                //Draw player to minimap.
                SDL_FillRect(mSurface, &wall_rect, color_player);
            }

            wall_rect.x += cell_size_x;
        }
        wall_rect.x = pos_x;
        wall_rect.y += cell_size_y;
    }
}

void SwRenderer::DrawVerticalLine(
    const int x,
    int y_start,
    int y_end,
    const SDL_Color color
)
{
    if (y_end < y_start)
    {
        // Make sure y_end is always greater than y_start.
         std::swap(y_start, y_end);
   }

    if (y_end < 0 || y_start >= mResY || x < 0 || x >= mResX) {
        return; // Not a single point of the line is on screen.
    }

    if (y_start < 0)
    {
        y_start = 0; //clip
    }

    if (y_end >= mResX)
    {
        y_end = mResY - 1; //clip
    }

    const auto screen_color = SDL_MapRGB(mSurface->format, color.r,
                                                           color.g,
                                                           color.b);
    const auto bpp = mSurface->format->BytesPerPixel;
    const auto offset = (mSurface->pitch * y_start) + (x * bpp);
    auto bufp = static_cast<char*>(mSurface->pixels) + offset;

    for (int y = y_start; y <= y_end; y++)
    {
        *(Uint32*)bufp = screen_color;
        bufp += mSurface->pitch;
    }
}
