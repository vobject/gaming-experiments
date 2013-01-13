#ifndef RAY_HPP
#define RAY_HPP

#include "Utils.hpp"

class Level;

class Ray
{
public:
   Ray(const Vector pos, const Vector dir, const Level& level);
   ~Ray();

   int GetMapIntersectionX() const;
   int GetMapIntersectionY() const;
   Vector GetIntersection() const;
   double GetDistance() const;
   bool VerticalSideHit() const;

private:
   Vector mIntersection = { .0, .0 };
   double mDistance = .0;
   bool mVerticalSideHit = false;
   int mMapX = 0;
   int mMapY = 0;
};

#endif // RAY_HPP
