#ifdef WITH_OPENCL

#include "ClRenderer.hpp"
#include "../../Level.hpp"
#include "../../Player.hpp"

#include <SDL.h>

#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <cstdint>

ClRenderer::ClRenderer(const int res_x, const int res_y, const std::string& app_name)
    : Renderer(res_x, res_y, app_name)
{
    Startup();
}

ClRenderer::~ClRenderer()
{
    Shutdown();
}

void ClRenderer::Startup()
{
    Shutdown();
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

    // The SDL screen must be successfully initialized before we call this.
    InitOpenCl();
}

void ClRenderer::Shutdown()
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

void ClRenderer::PreRender()
{
    Renderer::PreRender();
}

void ClRenderer::PostRender()
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
    const size_t local_work_size = 64;
    rc = clEnqueueNDRangeKernel(mQueue, mKernel, 1, nullptr, &global_work_size,
                                &local_work_size, 0, nullptr, nullptr);
    if (CL_SUCCESS != rc) {
        throw "clEnqueueNDRangeKernel() failed.";
    }

    if(SDL_MUSTLOCK(mSurface)) {
        SDL_LockSurface(mSurface);
    }

    rc = clEnqueueReadBuffer(mQueue, mPixelBuf, CL_TRUE, 0, mPixelBufSize,
                             mSurface->pixels, 0, nullptr, nullptr);
    if (CL_SUCCESS != rc) {
        throw "clEnqueueReadBuffer() failed.";
    }

    if(SDL_MUSTLOCK(mSurface)) {
        SDL_UnlockSurface(mSurface);
    }

    DrawMinimap(level, player);
}

const std::string& ClRenderer::GetName() const
{
    static std::string name("OpenCL");
    return name;
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

    const auto cl_path = "cl_renderer.cl";
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

    mKernel = clCreateKernel(mProgram, "rc_1", &rc);
    if (CL_SUCCESS != rc) {
        throw "clCreateKernel(\"rc_1\") failed.";
    }

    mPixelBufSize = mResX * mResY * mSurface->format->BytesPerPixel;
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

    mLevelBufSize = level.GetWidth() * level.GetHeight() * sizeof(int32_t);
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

void ClRenderer::InitMinimap(const Level& level)
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

void ClRenderer::DrawMinimap(const Level& level, const Player& player)
{
    if (!mMinimapTexture) {
        InitMinimap(level);
    }

    const auto cells_x = level.mGrid.at(0).size();
    const auto cells_y = level.mGrid.size();
    const unsigned int player_cell_x = player.mPosY;
    const unsigned int player_cell_y = player.mPosX;

    const auto color_floor = SDL_MapRGB(mMinimapSurface->format, 0xff, 0xff, 0xff);
    const auto color_wall = SDL_MapRGB(mMinimapSurface->format, 0x00, 0x00, 0x00);
    const auto color_player = SDL_MapRGB(mMinimapSurface->format, 0xff, 0x1f, 0x1f);

    auto const pixels = static_cast<Uint32*>(mMinimapSurface->pixels);

    for (unsigned int cell_y = 0; cell_y < cells_y; cell_y++)
    {
        const auto offset_y = mMinimapSurface->w * cell_y;

        for (unsigned int cell_x = 0; cell_x < cells_x; cell_x++)
        {
            auto bufp = pixels + offset_y + cell_x;

            if (level.mGrid[cell_y][cell_x] != 0)
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

#endif // WITH_OPENCL
