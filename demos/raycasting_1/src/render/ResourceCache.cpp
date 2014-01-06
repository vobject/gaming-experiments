#include "ResourceCache.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL2_rotozoom.h>

ResourceCache::ResourceCache(const std::string& res_dir, const int res_x, const int res_y, const Uint32 format)
    : mResDir(res_dir)
    , mResX(res_x)
    , mResY(res_y)
    , mPixelFormat(format)
{
    if (0 == IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) {
        throw "Failed to initialize SDL_image";
    }

    LoadWallResources();
    //LoadCeilingResources();
    LoadFloorResources();
    LoadSkyResources();
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

//SDL_Surface* ResourceCache::GetCeiling(const int id) const
//{
//   return mCeilingCache[id];
//}

SDL_Surface* ResourceCache::GetFloor(const int id) const
{
    return mFloorCache[id];
}

SDL_Surface* ResourceCache::GetSky(const int id) const
{
   return mSkyCache[id];
}

void ResourceCache::LoadWallResources()
{
    mWallCache.push_back(LoadTexture("walls_1.jpg", 1024, 1024));
    mWallCache.push_back(LoadTexture("walls_2.jpg", 1024, 1024));
    mWallCache.push_back(LoadTexture("walls_3.jpg", 1024, 1024));
    mWallCache.push_back(LoadTexture("walls_4.jpg", 1024, 1024));
}

//void ResourceCache::LoadCeilingResources()
//{
//}

void ResourceCache::LoadFloorResources()
{
    mFloorCache.push_back(LoadTexture("floor_0.jpg", 512, 512));
}

void ResourceCache::LoadSkyResources()
{
   // Sky textures have to be mResY / 2 pixels high.
   mSkyCache.push_back(LoadTexture("sky_0.jpg", 2048, mResY / 2));
   mSkyCache.push_back(LoadTexture("sky_1.jpg", 4096, mResY / 2));
   mSkyCache.push_back(LoadTexture("sky_2.jpg", 4096, mResY / 2));
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

    SDL_Surface* img_compat = SDL_ConvertSurfaceFormat(img_loaded, mPixelFormat, 0);
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
    if (SDL_SetColorKey(img_zoomed, SDL_RLEACCEL, colorkey)) {
        throw "SDL_SetColorKey failed";
    }

    mSurfaceCache.push_back(img_zoomed);
    return img_zoomed;
}
