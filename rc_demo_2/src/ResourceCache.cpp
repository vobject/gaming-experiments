#include "ResourceCache.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_rotozoom.h>

ResourceCache::ResourceCache(const std::string& res_dir)
   : mResDir(res_dir)
{
   if (0 == IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) {
       throw "Failed to initialize SDL_image";
   }

   LoadWallResources();
}

ResourceCache::~ResourceCache()
{
   for (auto& surface : mSurfaceCache) {
      SDL_FreeSurface(surface);
   }
}

SDL_Surface* ResourceCache::GetWall(const int id) const
{
   return mWallCache[id];
}

void ResourceCache::LoadWallResources()
{
   mWallCache.push_back(LoadTexture("walls_1.jpg", 1024, 1024));
   mWallCache.push_back(LoadTexture("walls_2.jpg", 1024, 1024));
   mWallCache.push_back(LoadTexture("walls_3.jpg", 1024, 1024));
   mWallCache.push_back(LoadTexture("walls_4.jpg", 512, 512));
   mWallCache.push_back(LoadTexture("walls_5.jpg", 512, 512));
   mWallCache.push_back(LoadTexture("walls_6.jpg", 512, 512));
}

SDL_Surface* ResourceCache::LoadTexture(
   const std::string& file,
   const int width,
   const int height
)
{
   const auto full_path = mResDir + "/" + file;

   SDL_Surface* img_loaded = IMG_Load(full_path.c_str());
   if (!img_loaded) {
      throw "Failed to load texture";
   }

   SDL_Surface* img_compat = SDL_DisplayFormat(img_loaded);
   if (!img_compat) {
      throw "Failed to convert animation frame to display format";
   }
   SDL_FreeSurface(img_loaded);
   img_loaded = nullptr;

   const auto x_zoom = static_cast<double>(width) / img_compat->w;
   const auto y_zoom = static_cast<double>(height) / img_compat->h;
   const auto img_zoomed = zoomSurface(img_compat, x_zoom, y_zoom, 1);
   SDL_FreeSurface(img_compat);
   img_compat = nullptr;

   const auto colorkey = SDL_MapRGB(img_zoomed->format, 0xff, 0, 0xff);
   if (SDL_SetColorKey(img_zoomed, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey)) {
      throw "SDL_SetColorKey failed";
   }

   mSurfaceCache.push_back(img_zoomed);
   return img_zoomed;
}
