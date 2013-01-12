#include "Render.hpp"
#include "ResourceCache.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Ray.hpp"
#include "Slice.hpp"
#include "Utils.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>
#include <cmath>

void render_thread(const Level& level, const Player& player, const ResourceCache& res,
                   int resx_start, int resx_stop, int resx, int resy, SDL_Surface* screen)
{
   const Vector player_pos = player.GetPosition();
   const Vector player_dir = player.GetDirection();
   const Vector player_plane = player.GetPlane();

   for (auto x = resx_start; x < resx_stop; x++)
   {
      // Current column position relative to the center of the screen.
      // Left edge is -1, right edge is 1, and center is 0.
      const double cam_x = 2. * x / resx - 1;

      // Starting direction of the current ray to be cast.
      const auto ray_dir = player_dir + (player_plane * cam_x);

      const Ray ray(player_pos, ray_dir, level);
      const Slice slice (resy, ray, player_pos, ray_dir, level, res);

      for (auto y = 0; y < resy; y++)
      {
         const auto color = slice.GetPixel(y);
         const auto offset = (screen->pitch * y) + (x * 4);
         const auto ptr = static_cast<Uint8*>(screen->pixels) + offset;
         memcpy(ptr, &color, sizeof(color));
      }
   }
}

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
   const int thread_cnt = 6;
   const int thread_slice = mResX / thread_cnt;

   std::vector<std::thread> threads;
   for (int i = 0; i < thread_cnt; i++)
   {
      const auto slice_start = i * thread_slice;
      const auto slice_stop = slice_start + thread_slice;

      threads.push_back(std::thread{render_thread,
                                    std::ref(level),
                                    std::ref(player),
                                    std::ref(*mResCache),
                                    slice_start, slice_stop,
                                    mResX, mResY,
                                    mScreen});
   }

   for (auto& t : threads)
   {
      t .join();
   }

//   const Vector player_pos = player.GetPosition();
//   const Vector player_dir = player.GetDirection();
//   const Vector player_plane = player.GetPlane();

//   for (auto x = 0; x < mResX; x++)
//   {
//      // Current column position relative to the center of the screen.
//      // Left edge is -1, right edge is 1, and center is 0.
//      const double cam_x = 2. * x / mResX - 1;

//      // Starting direction of the current ray to be cast.
//      const auto ray_dir = player_dir + (player_plane * cam_x);

//      const Ray ray(player_pos, ray_dir, level);
//      const Slice slice (mResY, ray, player_pos, ray_dir, level, *mResCache);

//      for (auto y = 0; y < mResY; y++)
//      {
//         SetPixel(x, y, slice.GetPixel(y));
//      }
//   }
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

void Render::SetPixel(const int x, const int y, const SDL_Color color)
{
   // We will always use 32bit surfaces.
   const auto offset = (mScreen->pitch * y) + (x * 4);
   const auto ptr = static_cast<Uint8*>(mScreen->pixels) + offset;
   memcpy(ptr, &color, sizeof(color));
}
