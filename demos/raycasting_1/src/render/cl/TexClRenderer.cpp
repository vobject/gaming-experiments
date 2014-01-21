#ifdef WITH_OPENCL
#ifdef WITH_TEXTURE

#include "TexClRenderer.hpp"
#include "../ResourceCache.hpp"
#include "../../Level.hpp"
#include "../../Player.hpp"
#include "../../Utils.hpp"

#include <SDL.h>

#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <cstdint>
#include <cstring>

TexClRenderer::TexClRenderer()
    : Renderer("TexOpenCL")
{
    Startup();
}

TexClRenderer::~TexClRenderer()
{
    Shutdown();
}

void TexClRenderer::Startup()
{
    Renderer::Startup();

    mRenderer = SDL_CreateRenderer(mScreen, -1, 0);
    if (!mRenderer) {
        throw "SDL_CreateRenderer() failed.";
    }

    mTexture = SDL_CreateTexture(mRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        mResX, mResY);
    if (!mTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mSurface = SDL_CreateRGBSurface(0, mResX, mResY, 32, 0, 0, 0, 0);
    if (!mSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }

    mResCache = Utils::make_unique<ResourceCache>("res", mResX, mResY);

    // The SDL screen must be successfully initialized before we call this.
    InitOpenCl();
}

void TexClRenderer::Shutdown()
{
    ShutdownOpenCl();

    if (mMinimapSurface) {
        SDL_FreeSurface(mMinimapSurface);
        mMinimapSurface = nullptr;
    }

    if (mMinimapTexture) {
        SDL_DestroyTexture(mMinimapTexture);
        mMinimapTexture = nullptr;
    }

    if (mSurface) {
        SDL_FreeSurface(mSurface);
        mSurface = nullptr;
    }

    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }

    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }

    Renderer::Shutdown();
}

void TexClRenderer::PreRender()
{
    Renderer::PreRender();
}

void TexClRenderer::PostRender()
{
    Renderer::PostRender();

    SDL_UpdateTexture(mTexture, nullptr, mSurface->pixels, mResX * sizeof(Uint32));
    SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);

    if (mMinimapTexture)
    {
        SDL_UpdateTexture(mMinimapTexture, nullptr, mMinimapSurface->pixels, mMinimapSurface->w * sizeof(Uint32));
        SDL_Rect mmDst = { 0,
            0,
            mMinimapSurface->w * 4,
            mMinimapSurface->h * 4 };
        SDL_RenderCopy(mRenderer, mMinimapTexture, nullptr, &mmDst);
    }

    SDL_RenderPresent(mRenderer);
}

void TexClRenderer::DoRender(const Level& level, const Player& player)
{
   if (!mLevelBuf)
   {
      // We did not yet initialize the level buffer. Do it now.
      InitLevelBuffer(level);
   }

   cl_int rc;
   const cl::screen_params sp = GetScreenKernelArg();
   const cl::player_params pp = GetPlayerKernelArg(player);
   const cl::level_params lp = GetLevelKernelArg(level);
   const cl::wall_params wp = { 1024, 1024 };

   rc  = clSetKernelArg(mKernel, 0, sizeof(cl_mem), &mPixelBuf);
   rc |= clSetKernelArg(mKernel, 1, sizeof(cl::screen_params), &sp);
   rc |= clSetKernelArg(mKernel, 2, sizeof(cl::player_params), &pp);
   rc |= clSetKernelArg(mKernel, 3, sizeof(cl::level_params), &lp);
   rc |= clSetKernelArg(mKernel, 4, sizeof(cl_mem), &mLevelBuf);

   rc |= clSetKernelArg(mKernel, 5, sizeof(cl::wall_params), &wp);
   rc |= clSetKernelArg(mKernel, 6, sizeof(cl_mem), &mWall1Buf);
   rc |= clSetKernelArg(mKernel, 7, sizeof(cl_mem), &mWall2Buf);
   rc |= clSetKernelArg(mKernel, 8, sizeof(cl_mem), &mWall3Buf);
   rc |= clSetKernelArg(mKernel, 9, sizeof(cl_mem), &mWall4Buf);

   if (CL_SUCCESS != rc) {
      throw "clSetKernelArg() failed.";
   }

   const size_t global_work_size = mResX;
   const size_t local_work_size = 64;
   rc = clEnqueueNDRangeKernel(mQueue, mKernel, 1, nullptr, &global_work_size,
                               &local_work_size, 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueNDRangeKernel() failed.";
   }

   if(SDL_MUSTLOCK(mScreen)) {
      SDL_LockSurface(mScreen);
   }

   rc = clEnqueueReadBuffer(mQueue, mPixelBuf, CL_TRUE, 0, mPixelBufSize,
                            mScreen->pixels, 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueReadBuffer() failed.";
   }

   if(SDL_MUSTLOCK(mScreen)) {
      SDL_UnlockSurface(mScreen);
   }

   DrawMinimap(level, player);
}

void TexClRenderer::InitOpenCl()
{
   cl_int rc;

   cl_platform_id platform;
   rc = clGetPlatformIDs(1, &platform, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clGetPlatformIDs() failed.";
   }

   cl_device_id device;
   rc = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clGetDeviceIDs() failed.";
   }

   cl_context_properties ctx_props[3] = {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)platform,
      0
   };
   mContext = clCreateContext(ctx_props, 1, &device, nullptr, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateContext() failed.";
   }

   mQueue = clCreateCommandQueue(mContext, device, 0, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateCommandQueue() failed.";
   }

   const auto cl_path = "cl_tex_renderer.cl";
   std::ifstream cl_file(cl_path, std::ios::binary);
   std::vector<char> cl_data((std::istreambuf_iterator<char>(cl_file)), std::istreambuf_iterator<char>());
   cl_data.push_back('\0');
   cl_file.close();

   const auto* prog = cl_data.data();
   mProgram = clCreateProgramWithSource(mContext, 1, &prog, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateProgramWithSource() failed.";
   }

   rc = clBuildProgram(mProgram, 0, nullptr, "-cl-fast-relaxed-math", nullptr, nullptr);
   if (CL_SUCCESS != rc)
   {
      size_t buidlog_size;
      clGetProgramBuildInfo (mProgram, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &buidlog_size);

      std::vector<char> buildlog(buidlog_size);
      clGetProgramBuildInfo(mProgram, device, CL_PROGRAM_BUILD_LOG, buidlog_size, &buildlog[0], nullptr);

      std::cerr << "Could not build " << cl_path << ":" << std::endl << buildlog.data() << std::endl;
      throw "clBuildProgram() failed.";
   }

   mKernel = clCreateKernel(mProgram, "rc_2", &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateKernel(\"rc_2\") failed.";
   }

   mPixelBufSize = mResX * mResY * mScreen->format->BytesPerPixel;
   mPixelBuf = clCreateBuffer(mContext, CL_MEM_WRITE_ONLY, mPixelBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(mPixelBuf) failed.";
   }

   const auto wall1_tex = mResCache->GetWall(0);
   const auto wall2_tex = mResCache->GetWall(1);
   const auto wall3_tex = mResCache->GetWall(2);
   const auto wall4_tex = mResCache->GetWall(3);
   mWallBufSize = wall1_tex->w * wall1_tex->h * wall1_tex->format->BytesPerPixel;

   mWall1Buf = clCreateBuffer(mContext, CL_MEM_READ_ONLY, mWallBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(mWall1Buf) failed.";
   }
   rc = clEnqueueWriteBuffer(mQueue, mWall1Buf, CL_TRUE, 0, mWallBufSize,
                             wall1_tex->pixels, 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueWriteBuffer(wall1) failed.";
   }

   mWall2Buf = clCreateBuffer(mContext, CL_MEM_READ_ONLY, mWallBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(mWall2Buf) failed.";
   }
   rc = clEnqueueWriteBuffer(mQueue, mWall2Buf, CL_TRUE, 0, mWallBufSize,
                             wall2_tex->pixels, 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueWriteBuffer(wall2) failed.";
   }

   mWall3Buf = clCreateBuffer(mContext, CL_MEM_READ_ONLY, mWallBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(mWall3Buf) failed.";
   }
   rc = clEnqueueWriteBuffer(mQueue, mWall3Buf, CL_TRUE, 0, mWallBufSize,
                             wall3_tex->pixels, 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueWriteBuffer(wall3) failed.";
   }

   mWall4Buf = clCreateBuffer(mContext, CL_MEM_READ_ONLY, mWallBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(mWall4Buf) failed.";
   }
   rc = clEnqueueWriteBuffer(mQueue, mWall4Buf, CL_TRUE, 0, mWallBufSize,
                             wall4_tex->pixels, 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueWriteBuffer(wall4) failed.";
   }
}

void TexClRenderer::ShutdownOpenCl()
{
   clReleaseMemObject(mWall4Buf);
   clReleaseMemObject(mWall3Buf);
   clReleaseMemObject(mWall2Buf);
   clReleaseMemObject(mWall1Buf);

   clReleaseMemObject(mLevelBuf);
   clReleaseMemObject(mPixelBuf);
   clReleaseKernel(mKernel);
   clReleaseProgram(mProgram);
   clReleaseCommandQueue(mQueue);
   clReleaseContext(mContext);
}

void TexClRenderer::InitLevelBuffer(const Level& level)
{
   cl_int rc;

   mLevelBufSize = level.GetWidth() * level.GetHeight() * sizeof(int32_t);
   mLevelBuf = clCreateBuffer(mContext, CL_MEM_READ_ONLY, mLevelBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(level) failed.";
   }

   // Write the level data into the OpenCL memory.
   rc = clEnqueueWriteBuffer(mQueue, mLevelBuf, CL_TRUE, 0, mLevelBufSize,
       level.GetGrid(), 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
       throw "clEnqueueWriteBuffer(level) failed.";
   }
}

cl::screen_params TexClRenderer::GetScreenKernelArg() const
{
    return{ static_cast<uint32_t>(mResX), static_cast<uint32_t>(mResY) };
}

cl::player_params TexClRenderer::GetPlayerKernelArg(const Player& player) const
{
    return{
        static_cast<float>(player.mPosX), static_cast<float>(player.mPosY),
        static_cast<float>(player.mDirX), static_cast<float>(player.mDirY),
        static_cast<float>(player.mPlaneX), static_cast<float>(player.mPlaneY)
    };
}

cl::level_params TexClRenderer::GetLevelKernelArg(const Level& level) const
{
    return{ static_cast<uint32_t>(level.GetWidth()),
            static_cast<uint32_t>(level.GetHeight()) };
}

void TexClRenderer::InitMinimap(const Level& level)
{
    mMinimapTexture = SDL_CreateTexture(mRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        level.GetWidth(), level.GetHeight());
    if (!mMinimapTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mMinimapSurface = SDL_CreateRGBSurface(0, level.GetWidth(), level.GetHeight(),
        32, 0, 0, 0, 0);
    if (!mMinimapSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }
}

void TexClRenderer::DrawMinimap(const Level& level, const Player& player)
{
    if (!mMinimapTexture) {
        InitMinimap(level);
    }

    const auto cells_x = level.GetWidth();
    const auto cells_y = level.GetHeight();
    const unsigned int player_cell_x = player.mPosY;
    const unsigned int player_cell_y = player.mPosX;

    const auto color_floor = SDL_MapRGBA(mMinimapSurface->format, 255, 255, 255, 0);
    const auto color_wall = SDL_MapRGBA(mMinimapSurface->format, 0, 0, 0, 0);
    const auto color_player = SDL_MapRGBA(mMinimapSurface->format, 255, 128, 128, 0);

    auto const pixels = static_cast<Uint32*>(mMinimapSurface->pixels);

    for (unsigned int cell_y = 0; cell_y < cells_y; cell_y++)
    {
        const auto offset_y = mMinimapSurface->w * cell_y;

        for (unsigned int cell_x = 0; cell_x < cells_x; cell_x++)
        {
            auto bufp = pixels + offset_y + cell_x;

            if (level.GetBlockType(cell_y, cell_x) != 0)
            {
                // This cell is a wall. Mark it on the minimap.
                *bufp = color_wall;
            }
            else if ((cell_y == player_cell_y) && (cell_x == player_cell_x))
            {
                // Draw player to minimap.
                *bufp = color_player;
            }
            else
            {
                *bufp = color_floor;
            }
        }
    }
}

#endif // WITH_TEXTURE
#endif // WITH_OPENCL
