#include "ClRenderer.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <SDL.h>

#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <cstdint>
#include <cmath>

ClRenderer::ClRenderer(const int res_x, const int res_y)
   : mResX(res_x)
   , mResY(res_y)
{
   if (0 > SDL_Init(SDL_INIT_VIDEO)) {
      throw "Cannot init SDL video subsystem.";
   }
   atexit(SDL_Quit);

   mScreen = SDL_SetVideoMode(mResX, mResY, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
   if (!mScreen) {
      throw "SDL_SetVideoMode() failed.";
   }

   // The SDL screen must be successfully initialized before we call this.
   InitOpenCl();
}

ClRenderer::~ClRenderer()
{
   ShutdownOpenCl();
}

void ClRenderer::PreRender()
{
   // Screen size might have changed.
   mScreen = SDL_GetVideoSurface();
}

void ClRenderer::DoRender(const Level& level, const Player& player)
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

   rc  = clSetKernelArg(mKernel, 0, sizeof(cl_mem), &mPixelBuf);
   rc |= clSetKernelArg(mKernel, 1, sizeof(cl::screen_params), &sp);
   rc |= clSetKernelArg(mKernel, 2, sizeof(cl::player_params), &pp);
   rc |= clSetKernelArg(mKernel, 3, sizeof(cl::level_params), &lp);
   rc |= clSetKernelArg(mKernel, 4, sizeof(cl_mem), &mLevelBuf);

   if (CL_SUCCESS != rc) {
      throw "clSetKernelArg() failed.";
   }

   const size_t global_work_size = mResX;
   const size_t local_work_size = 128;
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

   //DrawMinimap(level, player);
}

void ClRenderer::PostRender()
{
   SDL_Flip(mScreen);
}

std::string ClRenderer::GetName() const
{
   return "OpenCL";
}

void ClRenderer::InitOpenCl()
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

   const auto cl_path = "kernel.cl";
   std::ifstream cl_file(cl_path, std::ios::binary);
   std::vector<char> cl_data((std::istreambuf_iterator<char>(cl_file)), std::istreambuf_iterator<char>());
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

   mKernel = clCreateKernel(mProgram, "rc_demo_1", &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateKernel(\"rc_demo_1\") failed.";
   }

   mPixelBufSize = mResX * mResY * mScreen->format->BytesPerPixel;
   mPixelBuf = clCreateBuffer(mContext, CL_MEM_WRITE_ONLY, mPixelBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer() failed.";
   }
}

void ClRenderer::ShutdownOpenCl()
{
   clReleaseMemObject(mLevelBuf);
   clReleaseMemObject(mPixelBuf);
   clReleaseKernel(mKernel);
   clReleaseProgram(mProgram);
   clReleaseCommandQueue(mQueue);
   clReleaseContext(mContext);
}

void ClRenderer::InitLevelBuffer(const Level& level)
{
   cl_int rc;

   mLevelBufSize = level.mGrid.size() * level.mGrid[0].size() * sizeof(int32_t);
   mLevelBuf = clCreateBuffer(mContext, CL_MEM_READ_ONLY, mLevelBufSize, nullptr, &rc);
   if (CL_SUCCESS != rc) {
      throw "clCreateBuffer(level) failed.";
   }

   // Copy the level data into one continuous array.
   std::vector<int32_t> buf;
   for (const auto& it : level.mGrid)
   {
      std::copy(std::begin(it), std::end(it), std::back_inserter(buf));
   }

   // Write the level data into the OpenCL memory.
   rc = clEnqueueWriteBuffer(mQueue, mLevelBuf, CL_TRUE, 0, mLevelBufSize,
                             &buf[0], 0, nullptr, nullptr);
   if (CL_SUCCESS != rc) {
      throw "clEnqueueWriteBuffer(level) failed.";
   }
}

cl::screen_params ClRenderer::GetScreenKernelArg() const
{
   return { mResX, mResY };
}

cl::player_params ClRenderer::GetPlayerKernelArg(const Player& player) const
{
   return {
      static_cast<float>(player.mPosX), static_cast<float>(player.mPosY),
      static_cast<float>(player.mDirX), static_cast<float>(player.mDirY),
      static_cast<float>(player.mPlaneX), static_cast<float>(player.mPlaneY)
   };
}

cl::level_params ClRenderer::GetLevelKernelArg(const Level& level) const
{
   return { level.mGrid.size(), level.mGrid[0].size() };
}

void ClRenderer::DrawMinimap(const Level& level, const Player& player)
{
   const auto cells_x = level.mGrid.at(0).size();
   const auto cells_y = level.mGrid.size();

   const Uint16 cell_size_x = (mResX / 4) / cells_x;
   const Uint16 cell_size_y = (mResY / 4) / cells_y;
   const Uint16 map_size_x = cell_size_x * cells_x;
   const Uint16 map_size_y = cell_size_y * cells_y;
   const Sint16 pos_x = mResX - map_size_x;
   const Sint16 pos_y = mResY - map_size_y;

   const auto color_floor = SDL_MapRGB(mScreen->format, 0xff, 0xff, 0xff);
   const auto color_wall = SDL_MapRGB(mScreen->format, 0x5f, 0x5f, 0x5f);
   const auto color_player = SDL_MapRGB(mScreen->format, 0xff, 0x1f, 0x1f);

   // Draw the minimap area.
   SDL_Rect map_rect = { pos_x, pos_y, map_size_x, map_size_y };
   SDL_FillRect(mScreen, &map_rect, color_floor);

   // Draw the individual walls to the minimap.
   SDL_Rect wall_rect = { map_rect.x, map_rect.y, cell_size_x, cell_size_y };
   const unsigned int player_cell_x = player.mPosY;
   const unsigned int player_cell_y = player.mPosX;

   for (unsigned int cell_y = 0; cell_y < level.mGrid.size(); cell_y++)
   {
      for (unsigned int cell_x = 0; cell_x < level.mGrid[cell_y].size(); cell_x++)
      {
         if (level.mGrid[cell_y][cell_x] != 0)
         {
            //Draw only cells of wall type.
            SDL_FillRect(mScreen, &wall_rect, color_wall);
         }

         if ((cell_y == player_cell_y) && (cell_x == player_cell_x))
         {
            //Draw player to minimap.
            SDL_FillRect(mScreen, &wall_rect, color_player);
         }

         wall_rect.x += cell_size_x;
      }
      wall_rect.x = pos_x;
      wall_rect.y += cell_size_y;
   }
}
