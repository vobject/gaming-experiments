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
   Slice(int screen_height, Ray ray, Vector pos, Vector dir, const Level& level, const ResourceCache& res);
   ~Slice();

   SDL_Color GetPixel(int y) const;

private:
   const int mHeight;
   std::vector<SDL_Color> mBuf;
};

#endif // SLICE_HPP
