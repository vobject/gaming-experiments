#include "Slice.hpp"
#include "Ray.hpp"
#include "Level.hpp"
#include "ResourceCache.hpp"
#include "Utils.hpp"

#include <cmath>

Slice::Slice(SDL_Surface* const surface, const int x)
   : mSurface(surface)
   , mXCoordinate(x)
{

}

Slice::~Slice()
{

}

void Slice::Draw(Ray ray, Vector pos, Vector dir, const Level& level, const ResourceCache& res)
{
   const int wall_height = std::abs(int(mSurface->h / ray.GetDistance()));

   int wall_start = (mSurface->h / 2) - (wall_height / 2);
   int wall_end = (mSurface->h / 2) + (wall_height / 2);

   if (wall_start < 0) {
      wall_start = 0;
   }

   if (wall_end >= mSurface->h) {
      wall_end = mSurface->h - 1;
   }

   // Get the texture that matches the cell type.
   const auto cell_id = level.mGrid[ray.GetMapIntersectionX()][ray.GetMapIntersectionY()] - 1;
   const auto wall_tex = res.GetWall(cell_id);

   // Where exactly the wall was hit.
   double wall_x = ray.GetIntersection().GetY();
   wall_x -= std::floor(wall_x);

   // X-coordinate on the texture.
   int tex_x = wall_x * wall_tex->w;

   if (( ray.VerticalSideHit() && dir.GetY() < 0) ||
       (!ray.VerticalSideHit() && dir.GetX() > 0))
   {
      tex_x = wall_tex->w - tex_x - 1;
   }

   for (int y = wall_start; y < wall_end; y++)
   {
      const int tex_y = (y * 2 - mSurface->h + wall_height) *
                        (wall_tex->h / 2) / wall_height;

      const auto tex_offset = (wall_tex->pitch * tex_y) + (tex_x * 4);
      const auto tex_ptr = static_cast<Uint8*>(wall_tex->pixels) + tex_offset;

      SDL_Color color = { tex_ptr[0], tex_ptr[1], tex_ptr[2] };

      if (ray.VerticalSideHit())
      {
         // Give X and Y-sides different brightness.
         color.r /= 2;
         color.g /= 2;
         color.b /= 2;
      }

      const auto scr_offset = (mSurface->pitch * y) + (mXCoordinate * 4);
      const auto scr_ptr = static_cast<Uint8*>(mSurface->pixels) + scr_offset;
      memcpy(scr_ptr, &color, sizeof(color));
   }

   // Get the texture for the ceiling and floor.
   const auto ceiling_tex = res.GetWall(5);
   const auto floor_tex = res.GetWall(4);

   // Position of the floor at the bottom of the wall.
   double floor_x_wall;
   double floor_y_wall;

   if (!ray.VerticalSideHit() && (dir.GetX() > 0)) {
      floor_x_wall = ray.GetMapIntersectionX();
      floor_y_wall = ray.GetMapIntersectionY() + wall_x;
   }
   else if (!ray.VerticalSideHit() && (dir.GetX() < 0)) {
      floor_x_wall = ray.GetMapIntersectionX() + 1.;
      floor_y_wall = ray.GetMapIntersectionY() + wall_x;
   }
   else if (ray.VerticalSideHit() && (dir.GetY() > 0)) {
      floor_x_wall = ray.GetMapIntersectionX() + wall_x;
      floor_y_wall = ray.GetMapIntersectionY();
   }
   else {
      floor_x_wall = ray.GetMapIntersectionX() + wall_x;
      floor_y_wall = ray.GetMapIntersectionY() + 1.;
   }

   const double dist_wall = ray.GetDistance();
   const double dist_player = .0;

   // Draw the floor from below the wall to the bottom of the screen.
   for (int y = wall_end; y < mSurface->h; y++)
   {
      const double cur_dist = mSurface->h / (2. * y - mSurface->h);
      const double weight = (cur_dist - dist_player) / (dist_wall - dist_player);

      const double cur_floor_x = weight * floor_x_wall + (1.0 - weight) * pos.GetX();
      const double cur_floor_y = weight * floor_y_wall + (1.0 - weight) * pos.GetY();

      const int floor_tex_x = int(cur_floor_x * floor_tex->w / 4) % floor_tex->w;
      const int floor_tex_y = int(cur_floor_y * floor_tex->h / 4) % floor_tex->h;

      const auto ceiling_tex_offset = (ceiling_tex->pitch * floor_tex_y) + (floor_tex_x * 4);
      const auto ceiling_tex_ptr = static_cast<Uint8*>(ceiling_tex->pixels) + ceiling_tex_offset;

      const auto floor_tex_offset = (floor_tex->pitch * floor_tex_y) + (floor_tex_x * 4);
      const auto floor_tex_ptr = static_cast<Uint8*>(floor_tex->pixels) + floor_tex_offset;

      const SDL_Color ceiling_color = { ceiling_tex_ptr[0], ceiling_tex_ptr[1], ceiling_tex_ptr[2] };
      SDL_Color floor_color = { floor_tex_ptr[0], floor_tex_ptr[1], floor_tex_ptr[2] };

      // Make the floor darker.
      floor_color.r /= 3;
      floor_color.g /= 3;
      floor_color.b /= 3;

      const auto ceiling_offset = (mSurface->pitch * (mSurface->h - y - 1)) + (mXCoordinate * 4);
      const auto ceiling_ptr = static_cast<Uint8*>(mSurface->pixels) + ceiling_offset;
      memcpy(ceiling_ptr, &ceiling_color, sizeof(ceiling_color));

      const auto floor_offset = (mSurface->pitch * y) + (mXCoordinate * 4);
      const auto floor_ptr = static_cast<Uint8*>(mSurface->pixels) + floor_offset;
      memcpy(floor_ptr, &floor_color, sizeof(floor_color));
   }
}
