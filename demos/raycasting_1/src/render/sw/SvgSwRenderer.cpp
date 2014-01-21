#ifdef WITH_SVG

#include "SvgSwRenderer.hpp"
#include "Slice.hpp"
#include "../../Ray.hpp"
#include "../../World.hpp"
#include "../../Player.hpp"

#include "../../klib/gfx/svg/SVGFile.h"
#include "../../klib/gfx/svg/SVGLayer.h"
#include "../../klib/gfx/svg/SVGPath.h"

#include <algorithm>

using namespace K;
K::SVGFile svgFile("level1_.svg");

void DrawPixel(int x, int y, const unsigned int color, SDL_Surface* surface)
{
    const auto bpp = surface->format->BytesPerPixel;
    const auto offset = (surface->pitch * y) + (x * bpp);

    memcpy(static_cast<char*>(surface->pixels) + offset, &color, bpp);
}

void DrawLine(int x, int y, int dx, int dy, const unsigned int color, SDL_Surface* surface)
{
    // based on http://alawibaba.com/projects/whiteboard/drawing-SDL.c

#define SGN(x) ((x)>0 ? 1 : ((x)==0 ? 0:(-1)))
#define ABS(x) ((x)>0 ? (x) : (-x))

    int x1 = x;
    int y1 = y;
    int x2 = dx;
    int y2 = dy;

    int lg_delta;
    int sh_delta;
    int cycle;
    int lg_step;
    int sh_step;

    lg_delta = x2 - x1;
    sh_delta = y2 - y1;
    lg_step = SGN(lg_delta);
    lg_delta = ABS(lg_delta);
    sh_step = SGN(sh_delta);
    sh_delta = ABS(sh_delta);

    if (sh_delta < lg_delta)
    {
        cycle = lg_delta >> 1;
        while (x1 != x2)
        {
            DrawPixel(x1, y1, color, surface);

            cycle += sh_delta;
            if (cycle > lg_delta)
            {
                cycle -= lg_delta;
                y1 += sh_step;
            }
            x1 += lg_step;
        }
        DrawPixel(x1, y1, color, surface);
    }

    cycle = sh_delta >> 1;
    while (y1 != y2)
    {
        DrawPixel(x1, y1, color, surface);

        cycle += lg_delta;
        if (cycle > sh_delta)
        {
            cycle -= sh_delta;
            x1 += lg_step;
        }
        y1 += sh_step;
    }
    DrawPixel(x1, y1, color, surface);
}


SvgSwRenderer::SvgSwRenderer()
: SvgSwRenderer("SvgSoftware")
{
    
}

SvgSwRenderer::SvgSwRenderer(const std::string& renderer_name)
: Renderer(renderer_name)
{
    Startup();
}

SvgSwRenderer::~SvgSwRenderer()
{
    Shutdown();
}

void SvgSwRenderer::Startup()
{
    Renderer::Startup();

    mRenderer = SDL_CreateRenderer(mScreen, -1, 0);
    if (!mRenderer) {
        throw "SDL_CreateRenderer() failed.";
    }

    mTexture = SDL_CreateTexture(mRenderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 mResY, mResX);
    if (!mTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mSurface = SDL_CreateRGBSurface(0, mResY, mResX, 32, 0, 0, 0, 0);
    if (!mSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }

    // initialize colors for ceiling and floor
    mCeilingColor = SDL_MapRGBA(mSurface->format, 128, 128, 128, 0); // ceiling
    mFloorColor = SDL_MapRGBA(mSurface->format, 96, 96, 96, 0); // floor

    // initialize wall colors
    mWallColors[0] = SDL_MapRGBA(mSurface->format, 0, 0, 0, 0); // black (fallback)
    mWallColors[1] = SDL_MapRGBA(mSurface->format, 255, 0, 0, 0); // red
    mWallColors[2] = SDL_MapRGBA(mSurface->format, 0, 255, 0, 0); // green
    mWallColors[3] = SDL_MapRGBA(mSurface->format, 0, 0, 255, 0); // blue
    mWallColors[4] = SDL_MapRGBA(mSurface->format, 255, 255, 255, 0); // white
    mWallColors[5] = SDL_MapRGBA(mSurface->format, 255, 255, 0, 0); // yellow
}

void SvgSwRenderer::Shutdown()
{
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

void SvgSwRenderer::PreRender()
{
    Renderer::PreRender();
}

void SvgSwRenderer::PostRender()
{
    auto res_diff = (std::max(mResX, mResY) - std::min(mResX, mResY)) / 2;
    SDL_Rect dest = { res_diff, -res_diff, mResY, mResX };

    SDL_UpdateTexture(mTexture, nullptr, mSurface->pixels, mSurface->pitch);
    SDL_RenderCopyEx(mRenderer, mTexture, nullptr, &dest, -90.0, nullptr, SDL_FLIP_NONE);

    if (mMinimapTexture)
    {
        SDL_UpdateTexture(mMinimapTexture, nullptr, mMinimapSurface->pixels, mMinimapSurface->w * sizeof(Uint32));
        SDL_Rect mmDst = { 0,
                           0,
                           mMinimapSurface->w * 2,
                           mMinimapSurface->h * 2 };
        //SDL_RenderCopy(mRenderer, mMinimapTexture, nullptr, &mmDst);
        SDL_RenderCopyEx(mRenderer, mMinimapTexture, nullptr, &mmDst, 0.0, nullptr, SDL_FLIP_NONE);
    }

    SDL_RenderPresent(mRenderer);

    Renderer::PostRender();
}

void SvgSwRenderer::DoRender(const World& level, const Player& player)
{
    DrawPlayerView(level, player);
    DrawMinimap(level, player);
}

void SvgSwRenderer::InitMinimap(const World& level)
{
    const auto width = svgFile.width;
    const auto height = svgFile.height;

    mMinimapTexture = SDL_CreateTexture(mRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        width, height);
    if (!mMinimapTexture) {
        throw "SDL_CreateTexture() failed.";
    }

    mMinimapSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!mMinimapSurface) {
        throw "SDL_CreateRGBSurface() failed.";
    }

    // initialize minimap colors
    mMinimapFloorColor = SDL_MapRGBA(mMinimapSurface->format, 255, 255, 255, 0);
    mMinimapWallColor = SDL_MapRGBA(mMinimapSurface->format, 0, 0, 0, 0);
    mMinimapPlayerColor = SDL_MapRGBA(mMinimapSurface->format, 255, 128, 128, 0);
}

void SvgSwRenderer::DrawPlayerView(const World& level, const Player& player)
{
    SVGLayer* composite = (SVGLayer*)(svgFile.getLayers()->getChilds().at(0));

    Point sun(0, 0);

    for (auto x = 0; x < mResX; x++)
    {
        double minlen = 999999;
        uint32_t color = 0x0;

        for (const SVGElement* elem : composite->getChilds())
        {
            SVGPath* path = (SVGPath*)elem;
            for (Line line : path->getLines())
            {
                // Current column position relative to the center of the screen.
                // Left edge is -1, right edge is 1, and center is 0.
                const double cam_x = 2.0 * x / mResX - 1;

                // Starting direction of the current ray to be cast.
                const double ray_dir_x = player.mDirX +(player.mPlaneX * cam_x);
                const double ray_dir_y = player.mDirY +(player.mPlaneY * cam_x);

                /*
                if (!(x % 32)) {
                    printf("playerdirx=%f, playerdiry=%f\n", player.mDirX, player.mDirY);
                }

                if (!(x % 32)) {
                    //printf("raydirx=%f, raydiry=%f\n", ray_dir_x, ray_dir_y);
                }
                */

                Line ray(player.mPosX, player.mPosY, player.mPosX + ray_dir_x*1000, player.mPosY + ray_dir_y*1000);
                Point result;
                if (line.getSegmentIntersection(ray, result)) {
                    double len = Line(player.mPosX, player.mPosY, result.x, result.y).getLength();
                    if (len < minlen) {
                        Point vec1(line.p2.x - result.x, line.p2.y - result.y);
                        Point vec2(sun.x - result.x, sun.y - result.y);
                        double cosa = vec2.x * vec1.x + vec2.y * vec1.y;
                        cosa /= sqrt(vec1.x*vec1.x + vec1.y*vec1.y);
                        cosa /= sqrt(vec2.x*vec2.x + vec2.y*vec2.y);
                        double angle = (cosa);
                        double fade = (angle/2+0.5);
                        minlen = len;
                        Color c = path->getLineColor();
                        color = ((int)(c.r*fade) << 16) | ((int)(c.g*fade) << 8) | ((int)(c.b*fade) << 0);
                    }
                }
            }
        }

        auto wall_color = color;
        auto slice = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * x);

        //printf("len=%f\n", minlen);
        draw_slice({0, 0, 0, 0, minlen, false}, mResY, slice, wall_color, mCeilingColor, mFloorColor);
    }
}

void SvgSwRenderer::DrawMinimap(const World& level, const Player& player)
{
    if (!mMinimapTexture) {
        InitMinimap(level);
    }

    const auto cells_x = svgFile.width;
    const auto cells_y = svgFile.height;
    const int player_cell_x = static_cast<int>(player.mPosY);
    const int player_cell_y = static_cast<int>(player.mPosX);

    auto const pixels = static_cast<Uint32*>(mMinimapSurface->pixels);

    SDL_Rect rect{ 0, 0, cells_x, cells_y };
    SDL_FillRect(mMinimapSurface, &rect, 0xebebeb);

    SVGLayer* composite = (SVGLayer*)(svgFile.getLayers()->getChilds().at(0));

    for (const SVGElement* elem : composite->getChilds())
    {
        const SVGPath* path = (SVGPath*)elem;
        for (Line line : path->getLines())
        {
            DrawLine(line.p1.x, line.p1.y, line.p2.x, line.p2.y, 0xff, mMinimapSurface);
        }
    }

    DrawPixel(player_cell_x, player_cell_y, 0x00, mMinimapSurface);
}

#endif // WITH_SVG
