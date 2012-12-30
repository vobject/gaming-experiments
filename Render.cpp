#include "Render.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <SDL.h>

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

   // The return value of SDL_SetVideoMode() (-> screen) should not be freed
   //  by the caller. The man page tells us to rely on SDL_Quit() to do this.
}

Render::~Render()
{

}

void Render::PreRender()
{
   // Screen size might have changed.
   mScreen = SDL_GetVideoSurface();

   DrawCeiling({ 0x60, 0x60, 0x60 });
   DrawFloor({ 0x80, 0x80, 0x80 });
}

void Render::DoRender(const Level& level, const Player& player)
{
   for (int x = 0; x < mResX; x++)
   {
      //calculate ray position and direction
      const double cameraX = 2 * x / double(mResX) - 1; //x-coordinate in camera space
      const double rayPosX = player.mPosX;
      const double rayPosY = player.mPosY;
      const double raydir_x = player.mDirX + player.mPlaneX * cameraX;
      const double raydir_y = player.mDirY + player.mPlaneY * cameraX;

      //which box of the map we're in
      int mapX = rayPosX;
      int mapY = rayPosY;

      //length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;

      //length of ray from one x or y-side to next x or y-side
      double deltaDistX = std::sqrt(1 + (raydir_y * raydir_y) / (raydir_x * raydir_x));
      double deltaDistY = std::sqrt(1 + (raydir_x * raydir_x) / (raydir_y * raydir_y));
      double perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      bool hit = false; //was there a wall hit?
      int side; //was a NS or a EW wall hit?

      //calculate step and initial sideDist
      if (raydir_x < 0)
      {
         stepX = -1;
         sideDistX = (rayPosX - mapX) * deltaDistX;
      }
      else
      {
         stepX = 1;
         sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
      }
      if (raydir_y < 0)
      {
         stepY = -1;
         sideDistY = (rayPosY - mapY) * deltaDistY;
      }
      else
      {
         stepY = 1;
         sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
      }

      //perform DDA
      while (!hit)
      {
         //jump to next map square, OR in x-direction, OR in y-direction
         if (sideDistX < sideDistY)
         {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
         }
         else
         {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
         }

         //Check if ray has hit a wall
         if ((mapX >= level.mGrid.size()) || (mapY >= level.mGrid[0].size()))
         {
            hit = true;
            break;
         }

         if (level.mGrid[mapX][mapY] > 0)
         {
            hit = true;
            break;
         }
      }

      //Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
      if (side == 0) {
         perpWallDist = std::fabs((mapX - rayPosX + (1 - stepX) / 2) / raydir_x);
      }
      else {
         perpWallDist = std::fabs((mapY - rayPosY + (1 - stepY) / 2) / raydir_y);
      }

      // Calculate height of line to draw on screen
      const int lineHeight = std::abs(int(mResY / perpWallDist));

      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + mResY / 2;
      if(drawStart < 0) drawStart = 0;
      int drawEnd = lineHeight / 2 + mResY / 2;
      if(drawEnd >= mResY) drawEnd = mResY - 1;

      //choose wall color
      SDL_Color color;
      if ((mapX >= level.mGrid.size()) || (mapY >= level.mGrid[0].size()))
      {
         color = { 0xff, 0x00, 0x00 }; // red borders
      }
      else
      {
         switch(level.mGrid[mapX][mapY])
         {
            case 1:  color = { 0xff, 0x00, 0x00 }; break; //red
            case 2:  color = { 0x00, 0xff, 0x00 }; break; //green
            case 3:  color = { 0x00, 0x00, 0xff }; break; //blue
            case 4:  color = { 0xff, 0xff, 0xff }; break; //white
            case 5:  color = { 0xff, 0xff, 0x00 }; break; //yellow
            default: color = { 0x30, 0x30, 0x30 }; break; //dark gray
         }
      }

      if (side == 1)
      {
         // Give x and y sides different brightness.
         color.r = color.r * .7f;
         color.g = color.g * .7f;
         color.b = color.b * .7f;
      }

      // Draw the pixels of the stripe as a vertical line.
      DrawVerticalLine(x, drawStart, drawEnd, color);
   }

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

void Render::DrawPixel(const int pos_x, const int pos_y, const unsigned int color)
{
   const auto bpp = mScreen->format->BytesPerPixel;
   const auto offset = (mScreen->pitch * pos_y) + (pos_x * bpp);

   SDL_LockSurface(mScreen);
   memcpy(static_cast<char*>(mScreen->pixels) + offset, &color, bpp);
   SDL_UnlockSurface(mScreen);
}

void Render::DrawLine(
   int src_x,
   int src_y,
   int dest_x,
   int dest_y,
   const unsigned int color
)
{
   // based on http://alawibaba.com/projects/whiteboard/drawing-SDL.c

   int lg_delta;
   int sh_delta;
   int cycle;
   int lg_step;
   int sh_step;

   lg_delta = dest_x - src_x;
   sh_delta = dest_y - src_y;
   lg_step = lg_delta > 0 ? 1 : (!lg_delta ? 0 : -1);
   lg_delta = std::abs(lg_delta);
   sh_step = sh_delta > 0 ? 1 : (!sh_delta ? 0 : -1);
   sh_delta = std::abs(sh_delta);

   if (sh_delta < lg_delta)
   {
      cycle = lg_delta >> 1;
      while (src_x != dest_x)
      {
         DrawPixel(src_x, src_y, color);

         cycle += sh_delta;
         if (cycle > lg_delta)
         {
            cycle -= lg_delta;
            src_y += sh_step;
         }
         src_x += lg_step;
      }
      DrawPixel(src_x, src_y, color);
   }

   cycle = sh_delta >> 1;
   while (src_y != dest_y)
   {
      DrawPixel(src_x, src_y, color);

      cycle += lg_delta;
      if (cycle > sh_delta)
      {
         cycle -= sh_delta;
         src_x += lg_step;
      }
      src_y += sh_step;
   }
   DrawPixel(src_x, src_y, color);
}

void Render::DrawVerticalLine(
   const int x,
   int y1,
   int y2,
   const SDL_Color color
)
{
   if(y2 < y1)
   {
      // TODO: In which case can this happen?

      // Make sure y2 is always greater than y1.
      std::swap(y1, y2);
   }

   if(y2 < 0 || y1 >= mResY  || x < 0 || x >= mResX) {
      return; // Not a single point of the line is on screen.
   }

   if(y1 < 0)
   {
      y1 = 0; //clip
   }

   if(y2 >= mResX)
   {
      y2 = mResY - 1; //clip
   }

   auto screen_color = SDL_MapRGB(mScreen->format, color.r, color.g, color.b);
   auto bufp = static_cast<char*>(mScreen->pixels)
               + (y1 * mScreen->pitch)
               + (x * mScreen->format->BytesPerPixel);

   for(int y = y1; y <= y2; y++)
   {
      *(Uint32*)bufp = screen_color;
      bufp += mScreen->pitch;
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
            DrawLine(wall_rect.x + (cell_size_x / 2),
                     wall_rect.y + (cell_size_y / 2),
                     wall_rect.x + std::sin(player.mDirX) * 8,
                     wall_rect.y + std::sin(player.mDirX) * 8,
                     color_player);
         }

         wall_rect.x += cell_size_x;
      }
      wall_rect.x = pos_x;
      wall_rect.y += cell_size_y;
   }
}
