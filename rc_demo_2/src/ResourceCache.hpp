#ifndef RESOURCE_CACHE_HPP
#define RESOURCE_CACHE_HPP

#include <string>
#include <vector>

struct SDL_Surface;

class ResourceCache
{
public:
   //static const int WALL_SIZE_X = 64;
   //static const int WALL_SIZE_Y = 64;

   ResourceCache(const std::string& res_dir, int res_x, int res_y);
   ~ResourceCache();

   SDL_Surface* GetWall(int id) const;

private:
   void LoadWallResources();

   SDL_Surface* LoadTexture(const std::string& file, int width, int height);

   const std::string mResDir;
   const int mResX;
   const int mResY;
   std::vector<SDL_Surface*> mWallCache;
   std::vector<SDL_Surface*> mSurfaceCache;
};

#endif // RESOURCE_CACHE_HPP
