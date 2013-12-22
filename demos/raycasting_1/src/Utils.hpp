#ifndef UTILS_HPP
#define UTILS_HPP

#include <SDL.h>

#include <memory>
#include <utility>
#include <algorithm>

namespace Utils
{
template<size_t> bool Is64BitHelper();
template<> inline bool Is64BitHelper<4>() { return false; }
template<> inline bool Is64BitHelper<8>() { return true; }
inline bool Is64Bit() { return Is64BitHelper<sizeof(size_t)>(); }

// Source:
//  http://herbsutter.com/gotw/_102/
//  http://stackoverflow.com/questions/10149840/c-arrays-and-make-unique
template<typename T, typename ...Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&& ...args)
{
   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(std::size_t n)
{
   typedef typename std::remove_extent<T>::type RT;
   return std::unique_ptr<T>(new RT[n]);
}

// Source:
// http://channel9.msdn.com/Events/GoingNative/GoingNative-2012/C-11-VC-11-and-Beyond
template<class T>
auto cbegin(const T& t) -> decltype(t.cbegin()) { return t.cbegin(); }

template<class T>
auto cend(const T& t) -> decltype(t.cend()) { return t.cend(); }


// http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}


//////////////////////////////////////////////////////////////////////////
// SDL Helper functions

inline void SDL_DrawPixel32(const int x, const int y, const Uint32 color, SDL_Surface* const surface)
{
    const auto offset = (surface->w * y) + x;
    auto const pixels = static_cast<Uint32*>(surface->pixels);
    auto const buf = pixels + offset;
    *buf = color;
}

inline void SDL_DrawLine32(int x1, int y1, int x2, int y2, const Uint32 color, SDL_Surface* const surface)
{
    // based on http://alawibaba.com/projects/whiteboard/drawing-SDL.c

    int lg_delta;
    int sh_delta;
    int cycle;
    int lg_step;
    int sh_step;

    lg_delta = x2 - x1;
    sh_delta = y2 - y1;
    lg_step = sgn(lg_delta);
    lg_delta = std::abs(lg_delta);
    sh_step = sgn(sh_delta);
    sh_delta = std::abs(sh_delta);

    if (sh_delta < lg_delta)
    {
        cycle = lg_delta >> 1;
        while (x1 != x2)
        {
            SDL_DrawPixel32(x1, y1, color, surface);

            cycle += sh_delta;
            if (cycle > lg_delta)
            {
                cycle -= lg_delta;
                y1 += sh_step;
            }
            x1 += lg_step;
        }
        SDL_DrawPixel32(x1, y1, color, surface);
    }

    cycle = sh_delta >> 1;
    while (y1 != y2)
    {
        SDL_DrawPixel32(x1, y1, color, surface);

        cycle += lg_delta;
        if (cycle > sh_delta)
        {
            cycle -= sh_delta;
            x1 += lg_step;
        }
        y1 += sh_step;
    }
    SDL_DrawPixel32(x1, y1, color, surface);
}

} // Utils namespace

#endif // UTILS_HPP
