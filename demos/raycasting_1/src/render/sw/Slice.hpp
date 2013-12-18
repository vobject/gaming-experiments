#ifndef SIMPLE_SLICE_HPP
#define SIMPLE_SLICE_HPP

#include "../../Ray.hpp"

#include <cstdint>

inline void draw_slice(const RaycastResult& r, int res_y,
                       uint32_t* slice_buf, uint32_t wall_color,
                       uint32_t ceiling_color, uint32_t floor_color)
{
    // calculate the height of the wall (vertical line) to draw on screen
    const double wall_height_d = std::abs(res_y / r.distance);
    const int wall_height = static_cast<int>(wall_height_d);

    // FIXME: line_height might be negative when the player is near a wall
    //  because distance was so small. This will crash later.

    // Set where the vertical line should be drawn.
    int wall_start = (res_y / 2) - (wall_height / 2);
    int wall_end = (res_y / 2) + (wall_height / 2);

    if (wall_start < 0) {
        wall_start = 0;
    }

    if (wall_end >= res_y) {
        wall_end = res_y - 1;
    }

    if (r.vertical_hit)
    {
        // give X- and Y-sides different brightness
        wall_color = ((uint8_t)((uint8_t)(wall_color >> 24) * .7f) << 24) +
                     ((uint8_t)((uint8_t)(wall_color >> 16) * .7f) << 16) +
                     ((uint8_t)((uint8_t)(wall_color >>  8) * .7f) <<  8) +
                     ((uint8_t)((uint8_t)(wall_color >>  0) * .7f) <<  0);
    }

    for (auto y = 0; y < res_y; y++)
    {
        if (y < wall_start) {
            *slice_buf = ceiling_color;
        } else if (y > wall_end) {
            *slice_buf = floor_color;
        } else {
            *slice_buf = wall_color;
        }
        slice_buf++;
    }
}

#endif // SIMPLE_SLICE_HPP
