#include "SwRenderer.hpp"
#include "Slice.hpp"
#include "../../Ray.hpp"
#include "../../World.hpp"
#include "../../Player.hpp"
#include "../../Utils.hpp"

#include <algorithm>

SwRenderer::SwRenderer(const int res_x, const int res_y, const std::string& app_name)
    : SwRenderer(res_x, res_y, app_name, "Software")
{

}

SwRenderer::SwRenderer(const int res_x, const int res_y, const std::string& app_name, const std::string& renderer_name)
    : Renderer(res_x, res_y, app_name, renderer_name)
{
    Startup();
}

SwRenderer::~SwRenderer()
{
    Shutdown();
}

void SwRenderer::Startup()
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

void SwRenderer::Shutdown()
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

void SwRenderer::PreRender()
{
    Renderer::PreRender();
}

void SwRenderer::PostRender()
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
                           mMinimapSurface->w * 4,
                           mMinimapSurface->h * 4 };
        SDL_RenderCopy(mRenderer, mMinimapTexture, nullptr, &mmDst);
    }

    SDL_RenderPresent(mRenderer);

    Renderer::PostRender();
}

void SwRenderer::DoRender(const World& world)
{
    DrawPlayerView(world, world.GetPlayer());
    DrawMinimap(world, world.GetPlayer());
}

void SwRenderer::InitMinimap(const World& world)
{
    const auto width = world.GetWidth();
    const auto height = world.GetHeight();

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
    mMinimapPlayerColor = SDL_MapRGBA(mMinimapSurface->format, 192, 96, 96, 0);
    mMinimapRayColor = SDL_MapRGBA(mMinimapSurface->format, 255, 128, 128, 0);
}

void SwRenderer::DrawPlayerView(const World& world, const Player& player)
{
    const auto& rays = player.GetRaycastResults();

    for (auto x = 0; x < mResX; x++)
    {
        const auto& ray = rays[x];

        const auto wall_type = world.GetCellType(ray.map_pos_x, ray.map_pos_y);
        const auto wall_color = mWallColors[wall_type];
        auto slice = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * x);

        draw_slice(ray, mResY, slice, wall_color, mCeilingColor, mFloorColor);
    }


//    const auto& sprites = world.GetSprites();
////     const auto& sprites_orig = world.GetSprites();
////     auto sprites(sprites_orig);
////     std::sort(sprites.begin(), sprites.end(), [](const Sprite& s1, const Sprite& s2){
////         return s1.
////     });

//#define texWidth 16
//#define texHeight 16

//    for (const auto& sprite : sprites)
//    {
//        //translate sprite position to relative to camera
//        double sprite_relpos_x = sprite.x - player.mPosX;
//        double sprite_relpos_y = sprite.y - player.mPosY;

//        //transform sprite with the inverse camera matrix
//        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
//        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
//        // [ planeY   dirY ]                                          [ -planeY  planeX ]

//        double inv_det = 1.0 / ((player.mPlaneX * player.mDirY) - (player.mDirX * player.mPlaneY)); //required for correct matrix multiplication

//        double transform_x = inv_det * ((player.mDirY * sprite_relpos_x) - (player.mDirX * sprite_relpos_y));
//        double transform_y = inv_det * ((-player.mPlaneY * sprite_relpos_x) + (player.mPlaneX * sprite_relpos_y)); //this is actually the depth inside the screen, that what Z is in 3D

//        int sprite_screen_pos_x = int((mResX / 2) * (1 + transform_x / transform_y));

//        //parameters for scaling and moving the sprites
//#define uDiv 1
//#define vDiv 1
//#define vMove 0.0
//        int vMoveScreen = int(vMove / transform_y);

//        //calculate height of the sprite on screen
//        int spriteHeight = std::abs(int(mResY / (transform_y))) / vDiv; //using "transformY" instead of the real distance prevents fisheye
//        //calculate lowest and highest pixel to fill in current stripe
//        int drawStartY = -spriteHeight / 2 + mResY / 2 + vMoveScreen;
//        if (drawStartY < 0) drawStartY = 0;
//        int drawEndY = spriteHeight / 2 + mResY / 2 + vMoveScreen;
//        if (drawEndY >= mResY) drawEndY = mResY - 1;

//        //calculate width of the sprite
//        int spriteWidth = std::abs(int(mResY / (transform_y))) / uDiv;
//        int drawStartX = -spriteWidth / 2 + sprite_screen_pos_x;
//        if (drawStartX < 0) drawStartX = 0;
//        int drawEndX = spriteWidth / 2 + sprite_screen_pos_x;
//        if (drawEndX >= mResX) drawEndX = mResX - 1;

//        //loop through every vertical stripe of the sprite on screen
//        for (int stripe = drawStartX; stripe < drawEndX; stripe++)
//        {
//            //int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
//            //the conditions in the if are:
//            //1) it's in front of camera plane so you don't see things behind you
//            //2) it's on the screen (left)
//            //3) it's on the screen (right)
//            //4) ZBuffer, with perpendicular distance
//            if (transform_y > 0 && stripe > 0 && stripe < mResX && transform_y < rays[stripe].distance)
//            for (int y = drawStartY; y < drawEndY; y++) //for every pixel of the current stripe
//            {
//                auto slice = static_cast<uint32_t*>(mSurface->pixels) + (mSurface->w * stripe) + y;
//                *slice = sprite.color;
//            }
//        }
//    }
}

void SwRenderer::DrawMinimap(const World& world, const Player& player)
{
    if (!mMinimapTexture) {
        InitMinimap(world);
    }

    const auto cells_x = world.GetWidth();
    const auto cells_y = world.GetHeight();
    const int player_cell_x = static_cast<int>(player.mPosY);
    const int player_cell_y = static_cast<int>(player.mPosX);
    auto const pixels = static_cast<Uint32*>(mMinimapSurface->pixels);

    // draw the minimap's floor and walls
    for (int cell_y = 0; cell_y < cells_y; cell_y++)
    {
        const auto offset_y = mMinimapSurface->w * cell_y;
        const auto pixels_y = pixels + offset_y;

        for (int cell_x = 0; cell_x < cells_x; cell_x++)
        {
            auto bufp = pixels_y + cell_x;

            if (world.GetCellType(cell_y, cell_x) != 0)
            {
                // This cell is a wall. Mark it on the minimap.
                *bufp = mMinimapWallColor;
            }
            else
            {
                *bufp = mMinimapFloorColor;
            }
        }
    }

    // draw the player's visibility ray
    const auto rays = player.GetRaycastResults();
    const auto& ray = rays[rays.size() / 2];
    Utils::SDL_DrawLine32(player_cell_x, player_cell_y,
                          ray.map_pos_x, ray.map_pos_y,
                          mMinimapRayColor, mMinimapSurface);

    // draw the player on the minimap
    Utils::SDL_DrawPixel32(player_cell_x, player_cell_y, mMinimapPlayerColor, mMinimapSurface);
}
