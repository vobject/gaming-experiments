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
