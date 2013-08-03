#ifndef CL_RENDERER_HPP
#define CL_RENDERER_HPP

#ifdef WITH_OPENCL

#include "Renderer.hpp"
#include "kernel_params.hpp"

#include <CL/cl.h>

#include <memory>

struct SDL_Surface;
class Level;
class Player;

class ClRenderer : public Renderer
{
public:
   ClRenderer(int res_x, int res_y);
   virtual ~ClRenderer();

   void PreRender() override;
   void DoRender(const Level& level, const Player& player) override;
   void PostRender() override;

   std::string GetName() const override;

private:
   void InitOpenCl();
   void ShutdownOpenCl();
   void InitLevelBuffer(const Level& level);

   cl::screen_params GetScreenKernelArg() const;
   cl::player_params GetPlayerKernelArg(const Player& player) const;
   cl::level_params GetLevelKernelArg(const Level& level) const;

   void DrawMinimap(const Level& level, const Player& player);

   // Screen variables
   const int mResX;
   const int mResY;
   SDL_Surface* mScreen = nullptr;

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
