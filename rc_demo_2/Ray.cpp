#include "Ray.hpp"
#include "Level.hpp"

#include <cmath>

Ray::Ray(const Vector pos, const Vector dir, const Level& level)
{
   // The player's current grid position inside the level.
   mMapX = pos.GetX();
   mMapY = pos.GetY();

   // Length of the ray from one X and Y-side to next X and Y-side.
   const double delta_dist_x = std::sqrt(std::pow(dir.GetY(), 2) / std::pow(dir.GetX(), 2) + 1);
   const double delta_dist_y = std::sqrt(std::pow(dir.GetX(), 2) / std::pow(dir.GetY(), 2) + 1);

   int step_x; // +1 if heading north, -1 if heading south.
   int step_y; // +1 if heading east, -1 if heading west.
   double side_dist_x;
   double side_dist_y;

   if (dir.GetX() < 0) {
      step_x = -1;
      side_dist_x = (pos.GetX() - mMapX) * delta_dist_x;
   }
   else {
      step_x = 1;
      side_dist_x = (mMapX + 1. - pos.GetX()) * delta_dist_x;
   }

   if (dir.GetY() < 0) {
      step_y = -1;
      side_dist_y = (pos.GetY() - mMapY) * delta_dist_y;
   }
   else {
      step_y = 1;
      side_dist_y = (mMapY + 1. - pos.GetY()) * delta_dist_y;
   }

   for (;;)
   {
      if (side_dist_x < side_dist_y)
      {
         // Jump one square in X-direction.
         mMapX += step_x;
         side_dist_x += delta_dist_x;
         mVerticalSideHit = false;
      }
      else
      {
         // Jump one square in Y-direction.
         mMapY += step_y;
         side_dist_y += delta_dist_y;
         mVerticalSideHit = true;
      }

      // Check if the ray has hit a wall.
      if ((mMapX < 0) || (mMapX >= level.GetWidth()) ||
          (mMapY < 0) || (mMapY >= level.GetHeight()) ||
          level.IsBlocking(mMapX, mMapY))
      {
         break;
      }
   }

   if (!mVerticalSideHit)
   {
      const double unit_x = mMapX + (1 - step_x) / 2;
      const double intersect_x = pos.GetY() + ((unit_x - pos.GetX()) / dir.GetX()) * dir.GetY();
      mIntersection = { unit_x, intersect_x };
      mDistance = std::fabs((unit_x - pos.GetX()) / dir.GetX());
   }
   else
   {
      const double unit_y = mMapY + (1 - step_y) / 2;
      const double intersect_y = pos.GetX() + ((unit_y - pos.GetY()) / dir.GetY()) * dir.GetX();
      mIntersection = { unit_y, intersect_y };
      mDistance = std::fabs((unit_y - pos.GetY()) / dir.GetY());
   }
}

Ray::~Ray()
{

}

int Ray::GetMapIntersectionX() const
{
   return mMapX;
}

int Ray::GetMapIntersectionY() const
{
   return mMapY;
}

Vector Ray::GetIntersection() const
{
   return mIntersection;
}

double Ray::GetDistance() const
{
   return mDistance;
}

bool Ray::VerticalSideHit() const
{
   return mVerticalSideHit;
}
