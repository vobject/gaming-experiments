#ifndef RESOURCE_CACHE_HPP
#define RESOURCE_CACHE_HPP

#include <SDL.h>

#include <string>
#include <vector>

class ResourceCache
{
public:
    ResourceCache(const std::string& res_dir, int res_x, int res_y, Uint32 format);
    ~ResourceCache();

    ResourceCache(ResourceCache&) = delete;
    ResourceCache& operator=(ResourceCache&) = delete;

    SDL_Surface* GetWall(int id) const;
    //SDL_Surface* GetCeiling(int id) const;
    SDL_Surface* GetFloor(int id) const;
    SDL_Surface* GetSky(int id) const;

private:
    void LoadWallResources();
    //void LoadCeilingResources();
    void LoadFloorResources();
    void LoadSkyResources();

    SDL_Surface* LoadTexture(const std::string& file, int width, int height);

    const std::string mResDir;
    const int mResX;
    const int mResY;
    const Uint32 mPixelFormat;
    std::vector<SDL_Surface*> mWallCache;
    //std::vector<SDL_Surface*> mCeilingCache;
    std::vector<SDL_Surface*> mFloorCache;
    std::vector<SDL_Surface*> mSkyCache;
    std::vector<SDL_Surface*> mSurfaceCache;
};

#endif // RESOURCE_CACHE_HPP
