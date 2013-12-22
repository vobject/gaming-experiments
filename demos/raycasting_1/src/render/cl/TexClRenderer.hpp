#ifndef TEX_CL_RENDERER_HPP
#define TEX_CL_RENDERER_HPP

#ifdef WITH_OPENCL
#ifdef WITH_TEXTURE

#include "../Renderer.hpp"
#include "kernel_params.hpp"

#include <SDL.h>

#include <CL/cl.h>

#include <memory>

class Level;
class Player;
class ResourceCache;

class TexClRenderer : public Renderer
{
public:
    TexClRenderer(int res_x, int res_y, const std::string& app_name);
    virtual ~TexClRenderer();

    void Startup() override;
    void Shutdown() override;

    void PreRender() override;
    void PostRender() override;
    void DoRender(const Level& level, const Player& player) override;

private:
   void InitOpenCl();
   void ShutdownOpenCl();
   void InitLevelBuffer(const Level& level);

   cl::screen_params GetScreenKernelArg() const;
   cl::player_params GetPlayerKernelArg(const Player& player) const;
   cl::level_params GetLevelKernelArg(const Level& level) const;

   void InitMinimap(const Level& level);
   void DrawMinimap(const Level& level, const Player& player);

   // Screen variables
   SDL_Renderer* mRenderer = nullptr;
   SDL_Texture* mTexture = nullptr;
   SDL_Surface* mSurface = nullptr;

   SDL_Texture* mMinimapTexture = nullptr;
   SDL_Surface* mMinimapSurface = nullptr;

   // Must be initialized after the video system has been set up.
   std::unique_ptr<const ResourceCache> mResCache;

   // OpenCL variables
   cl_context mContext = nullptr;
   cl_command_queue mQueue = nullptr;
   cl_program mProgram = nullptr;
   cl_kernel mKernel = nullptr;

   cl_mem mPixelBuf = nullptr;
   size_t mPixelBufSize = 0;

   cl_mem mLevelBuf = nullptr;
   size_t mLevelBufSize = 0;

   size_t mWallBufSize = 0;
   cl_mem mWall1Buf = nullptr;
   cl_mem mWall2Buf = nullptr;
   cl_mem mWall3Buf = nullptr;
   cl_mem mWall4Buf = nullptr;
};

#endif // WITH_TEXTURE
#endif // WITH_OPENCL

#endif // TEX_CL_RENDERER_HPP
