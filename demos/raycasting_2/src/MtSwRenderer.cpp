#ifdef WITH_MTSWRENDERER

#include "MtSwRenderer.hpp"
#include "ResourceCache.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Ray.hpp"
#include "Slice.hpp"
#include "Utils.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>
#include <sstream>
#include <cmath>

MtSwRenderer::MtSwRenderer(const int res_x, const int res_y, const int threads)
   : SwRenderer(res_x, res_y)
   , mThreadCnt(threads)
{

}

MtSwRenderer::~MtSwRenderer()
{

}

std::string MtSwRenderer::GetName() const
{
   std::stringstream os;
   os << "Software(threads:" << mThreadCnt << ")";
   return os.str();
}

void MtSwRenderer::DrawPlayerView(const Level& level, const Player& player)
{
   const int thread_slice = mResX / mThreadCnt;
   std::vector<std::thread> threads(mThreadCnt);

   auto render = [&](int resx_start, int resx_stop) {
       const Vector player_pos = player.GetPosition();
       const Vector player_dir = player.GetDirection();
       const Vector player_plane = player.GetPlane();

       for (auto x = resx_start; x < resx_stop; x++)
       {
          // Current column position relative to the center of the screen.
          // Left edge is -1, right edge is 1, and center is 0.
          const double cam_x = 2. * x / mScreen->w - 1;

          // Starting direction of the current ray to be cast.
          const auto ray_dir = player_dir + (player_plane * cam_x);

          const Ray ray(player_pos, ray_dir, level);

          Slice slice (mScreen, x);
          slice.Draw(ray, player_pos, ray_dir, level, *mResCache);
       }
   };

   if (SDL_MUSTLOCK(mScreen)) {
      // Scene rendering will only be done via pixel manipulation.
      SDL_LockSurface(mScreen);
   }

   for (int i = 0; i < mThreadCnt; i++)
   {
      const auto slice_start = i * thread_slice;
      const auto slice_stop = slice_start + thread_slice;

      threads[i] = std::thread{render, slice_start, slice_stop};
   }

   for (auto& t : threads)
   {
      t .join();
   }

   if (SDL_MUSTLOCK(mScreen)) {
      SDL_UnlockSurface(mScreen);
   }
}

#endif // WITH_MTSWRENDERER
