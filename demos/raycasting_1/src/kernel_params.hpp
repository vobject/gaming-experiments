#ifndef KERNEL_PARAMS_HPP
#define KERNEL_PARAMS_HPP

#ifdef WITH_OPENCL

#include <cstdint>

namespace cl {

struct screen_params
{
    int32_t res_x;
    int32_t res_y;
};

struct level_params
{
    int32_t level_x;
    int32_t level_y;
};

struct player_params
{
    float pos_x;
    float pos_y;
    float dir_x;
    float dir_y;
    float plane_x;
    float plane_y;
};

struct pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char unused;
};

} // namespace cl

#endif // WITH_OPENCL

#endif // KERNEL_PARAMS_HPP
