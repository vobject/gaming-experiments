#ifndef CL_RENDERER_HPP
#define CL_RENDERER_HPP

#ifdef WITH_OPENCL

#include "../Renderer.hpp"
#include "kernel_params.hpp"

#include <SDL.h>

#include <CL/cl.h>

class World;
class Player;
struct lua_State;

class ClRenderer : public Renderer
{
public:
    ClRenderer(lua_State* L);
    virtual ~ClRenderer();

    void Startup() override;
    void Shutdown() override;

    void PreRender() override;
    void PostRender() override;
    void DoRender(const World& world) override;

private:
    void InitOpenCl();
    void ShutdownOpenCl();
    void InitLevelBuffer(const World& world);

    cl::screen_params GetScreenKernelArg() const;
    cl::player_params GetPlayerKernelArg(const Player& player) const;
    cl::level_params GetLevelKernelArg(const Level& level) const;

    void InitMinimap(const World& world);
    void DrawMinimap(const World& world, const Player& player);

    // Screen variables
    SDL_Renderer* mRenderer = nullptr;
    SDL_Texture* mTexture = nullptr;
    SDL_Surface* mSurface = nullptr;

    SDL_Texture* mMinimapTexture = nullptr;
    SDL_Surface* mMinimapSurface = nullptr;

    // OpenCL variables
    cl_context mContext = nullptr;
    cl_command_queue mQueue = nullptr;
    cl_program mProgram = nullptr;
    cl_kernel mKernel = nullptr;

    cl_mem mPixelBuf = nullptr;
    size_t mPixelBufSize = 0;

    cl_mem mLevelBuf = nullptr;
    size_t mLevelBufSize = 0;
};

#endif // WITH_OPENCL

#endif // CL_RENDERER_HPP
