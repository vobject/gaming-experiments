#include "Render.hpp"
#include "ResourceCache.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Utils.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <cmath>

Render::Render(const int res_x, const int res_y)
   : mResX(res_x)
   , mResY(res_y)
{
   if (0 > SDL_Init(SDL_INIT_VIDEO)) {
      throw "Cannot init SDL video subsystem.";
   }
   atexit(SDL_Quit);

   const auto screen = SDL_SetVideoMode(mResX, mResY, 32, SDL_ANYFORMAT |
                                                          SDL_SWSURFACE |
                                                          SDL_DOUBLEBUF);
   if (!screen) {
      throw "SDL_SetVideoMode() failed.";
   }

   mResCache = make_unique<ResourceCache>(".");
}

Render::~Render()
{

}

void Render::PreRender()
{
   // Screen size might have changed.
   mScreen = SDL_GetVideoSurface();
}

void Render::DoRender(const Level& level, const Player& player)
{
   DrawCeiling({ 0x60, 0x60, 0x60 });
   DrawFloor({ 0x80, 0x80, 0x80 });
   DrawPlayerView(level, player);
   DrawMinimap(level, player);
}

void Render::PostRender()
{
   SDL_Flip(mScreen);
}

void Render::DrawCeiling(const SDL_Color color)
{
   const auto ceiling = SDL_MapRGB(mScreen->format, color.r, color.g, color.b);
   SDL_Rect ceiling_rect = { 0,
                             0,
                             static_cast<Uint16>(mResX),
                             static_cast<Uint16>(mResY / 2) };
   SDL_FillRect(mScreen, &ceiling_rect, ceiling);
}

void Render::DrawFloor(const SDL_Color color)
{
   const auto floor = SDL_MapRGB(mScreen->format, color.r, color.g, color.b);
   SDL_Rect floor_rect = { 0,
                           static_cast<Sint16>(mResY / 2),
                           static_cast<Uint16>(mResX),
                           static_cast<Uint16>(mResY / 2) };
   SDL_FillRect(mScreen, &floor_rect, floor);
}

void Render::DrawPlayerView(const Level& level, const Player& player)
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

      // Get the texture that matches the cell type.
      const auto texture_id = level.mGrid[map_x][map_y] - 1;
      const auto texture = mResCache->GetWall(texture_id);

      /////////////////////////////////////////////////
      // Where exactly the wall was hit.
      double wall_x;

      if (y_side_hit) {
         wall_x = ray_pos_x + ((map_y - ray_pos_y + (1 - step_y) / 2) / ray_dir_y) * ray_dir_x;
      }
      else {
         wall_x = ray_pos_y + ((map_x - ray_pos_x + (1 - step_x) / 2) / ray_dir_x) * ray_dir_y;
      }
      wall_x -= std::floor((wall_x));

      // X-coordinate on the texture.
      int tex_x = wall_x * texture->w;

      if(( y_side_hit && ray_dir_y < 0) ||
         (!y_side_hit && ray_dir_x > 0))
      {
         tex_x = texture->w - tex_x - 1;
      }

//      SDL_Rect src_rect = { static_cast<Sint16>(tex_x),
//                            static_cast<Sint16>(0),
//                            static_cast<Uint16>(1),
//                            static_cast<Uint16>(64) };
//      SDL_Rect dst_rect = { static_cast<Sint16>(x),
//                            static_cast<Sint16>(line_start) };
//      SDL_BlitSurface(texture, &src_rect, mScreen, &dst_rect);

      for(int y = line_start; y < line_end; y++)
      {
         const int tex_y = (y * 2 - mResY + line_height) *
                          (texture->h / 2) / line_height;

//         const auto bpp = mScreen->format->BytesPerPixel;
//         const auto offset = (texture->h * tex_y) + (tex_x * bpp);
//         const auto p = static_cast<char*>(mScreen->pixels) + offset;
//         Uint32 color = *(Uint32*)p;

         auto color = ((Uint32*)texture->pixels)[texture->h * tex_y + tex_x];

         //make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
         if(y_side_hit) color = (color >> 1) & 8355711;
         SetPixel(x, y, color);
      }
   }
}

void Render::DrawMinimap(const Level& level, const Player& player)
{
   const auto cells_x = level.mGrid.at(0).size();
   const auto cells_y = level.mGrid.size();

   const Uint16 cell_size_x = (mResX / 4) / cells_x;
   const Uint16 cell_size_y = (mResY / 4) / cells_y;
   const Uint16 map_size_x = cell_size_x * cells_x;
   const Uint16 map_size_y = cell_size_y * cells_y;
   const Sint16 pos_x = mResX - map_size_x;
   const Sint16 pos_y = mResY - map_size_y;

   const auto color_floor = SDL_MapRGB(mScreen->format, 0xff, 0xff, 0xff);
   const auto color_wall = SDL_MapRGB(mScreen->format, 0x5f, 0x5f, 0x5f);
   const auto color_player = SDL_MapRGB(mScreen->format, 0xff, 0x1f, 0x1f);

   // Draw the minimap area.
   SDL_Rect map_rect = { pos_x, pos_y, map_size_x, map_size_y };
   SDL_FillRect(mScreen, &map_rect, color_floor);

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
            SDL_FillRect(mScreen, &wall_rect, color_wall);
         }

         if ((cell_y == player_cell_y) && (cell_x == player_cell_x))
         {
            //Draw player to minimap.
            SDL_FillRect(mScreen, &wall_rect, color_player);
         }

         wall_rect.x += cell_size_x;
      }
      wall_rect.x = pos_x;
      wall_rect.y += cell_size_y;
   }
}

void Render::DrawVerticalLine(
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

   const auto screen_color = SDL_MapRGB(mScreen->format, color.r,
                                                         color.g,
                                                         color.b);
   const auto bpp = mScreen->format->BytesPerPixel;
   const auto offset = (mScreen->pitch * y_start) + (x * bpp);
   auto bufp = static_cast<char*>(mScreen->pixels) + offset;

   for(int y = y_start; y <= y_end; y++)
   {
      *(Uint32*)bufp = screen_color;
      bufp += mScreen->pitch;
   }
}

void Render::SetPixel(const int x, const int y, const Uint32 color)
{
//   const auto screen_color = SDL_MapRGB(mScreen->format, color.r,
//                                                         color.g,
//                                                         color.b);
   const auto bpp = mScreen->format->BytesPerPixel;
   const auto offset = (mScreen->pitch * y) + (x * bpp);
   auto bufp = static_cast<char*>(mScreen->pixels) + offset;
   *(Uint32*)bufp = color;
}
