#ifndef SLICE_HPP
#define SLICE_HPP

#include <SDL.h>

#include <vector>

class Ray;
class Level;
class ResourceCache;
class Vector;

class Slice
{
public:
   Slice(SDL_Surface* surface, int x);
   ~Slice();

   void Draw(Ray ray, Vector pos, Vector dir, const Level& level, const ResourceCache& res);

private:
   SDL_Surface* const mSurface;
   const int mXCoordinate;
};

#endif // SLICE_HPP
