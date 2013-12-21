#ifndef SIMPLE_SLICE_HPP
#define SIMPLE_SLICE_HPP

#include "ResourceCache.hpp"
#include "../../Ray.hpp"
#include "../../World.hpp"

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

inline void draw_slice(
    const RaycastResult& r,
    int x,
    int res_y,
    const double player_pos_x, const double player_pos_y,
    const double player_dir_x, const double player_dir_y,
    //uint32_t* slice_buf,
    SDL_Surface* wall,
    const World& lvl,
    const ResourceCache& res
)
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

    if (wall_end >= wall->h) {
        wall_end = wall->h - 1;
    }

    // Get the texture that matches the cell type.
    const auto cell_id = lvl.GetCellType(r.map_pos_x, r.map_pos_y);
    const auto wall_tex = res.GetWall(cell_id - 1);
    const auto ceiling_tex = res.GetWall(1);
    const auto floor_tex = res.GetFloor(0);

    // where exactly the wall was hit?
    double wall_x = r.intersect_y;
    wall_x -= std::floor(wall_x);

    // X-coordinate on the texture.
    int tex_x = wall_x * wall_tex->w;

    if ((r.vertical_hit && player_dir_y < 0) ||
        (!r.vertical_hit && player_dir_x > 0))
    {
        tex_x = wall_tex->w - tex_x - 1;
    }


    // Position of the floor at the bottom of the wall.
    double floor_x_wall;
    double floor_y_wall;

    if (!r.vertical_hit && (player_dir_x > 0)) {
        floor_x_wall = r.map_pos_x;
        floor_y_wall = r.map_pos_y + wall_x;
    }
    else if (!r.vertical_hit && (player_dir_x < 0)) {
        floor_x_wall = r.map_pos_x + 1.;
        floor_y_wall = r.map_pos_y + wall_x;
    }
    else if (r.vertical_hit && (player_dir_y > 0)) {
        floor_x_wall = r.map_pos_x + wall_x;
        floor_y_wall = r.map_pos_y;
    }
    else {
        floor_x_wall = r.map_pos_x + wall_x;
        floor_y_wall = r.map_pos_y + 1.;
    }

    const double dist_wall = r.distance;
    const double dist_player = .0;


    auto slice_buf = static_cast<uint32_t*>(wall->pixels) + (wall->w * x);

    auto tex_buf = static_cast<uint32_t*>(wall_tex->pixels);
    auto ceiling_buf = static_cast<uint32_t*>(ceiling_tex->pixels);
    auto floor_buf = static_cast<uint32_t*>(floor_tex->pixels);

    for (auto y = 0; y < res_y; y++)
    {
        if (y < wall_start) {
            const double cur_dist = -(res_y / (2. * y - res_y));
            const double weight = (cur_dist + dist_player) / (dist_wall - dist_player);

            double cur_floor_x = weight * floor_x_wall + (1.0 - weight) * player_pos_x;
            double cur_floor_y = weight * floor_y_wall + (1.0 - weight) * player_pos_y;
            if (cur_floor_x < 0.) { cur_floor_x = 0.; }
            if (cur_floor_y < 0.) { cur_floor_y = 0.; }

            const int floor_tex_x = int(cur_floor_x * floor_tex->w / 4) % floor_tex->w;
            const int floor_tex_y = int(cur_floor_y * floor_tex->h / 4) % floor_tex->h;

            const auto floor_tex_offset = (floor_tex->w * floor_tex_y) + floor_tex_x;
            *slice_buf = *(floor_buf + floor_tex_offset);
        }
        else if (y >= wall_end) {
            const double cur_dist = res_y / (2. * y - res_y);
            const double weight = (cur_dist + dist_player) / (dist_wall - dist_player);

            double cur_ceiling_x = weight * floor_x_wall + (1.0 - weight) * player_pos_x;
            double cur_ceiling_y = weight * floor_y_wall + (1.0 - weight) * player_pos_y;
            if (cur_ceiling_x < 0.) { cur_ceiling_x = 0.; }
            if (cur_ceiling_y < 0.) { cur_ceiling_y = 0.; }

            const int ceiling_tex_x = int(cur_ceiling_x * ceiling_tex->w / 4) % ceiling_tex->w;
            const int ceiling_tex_y = int(cur_ceiling_y * ceiling_tex->h / 4) % ceiling_tex->h;

            const auto ceiling_tex_offset = (ceiling_tex->w * ceiling_tex_y) + ceiling_tex_x;
            *slice_buf = *(ceiling_buf + ceiling_tex_offset);
        }
        else {
            const int tex_y = (y * 2 - res_y + wall_height) * (wall_tex->w / 2) / wall_height;
            const auto tex_offset = (wall_tex->w * tex_y) + tex_x;
            uint32_t tex_color = *(tex_buf + tex_offset);

            if (r.vertical_hit) {
                tex_color = ((uint8_t)((uint8_t)(tex_color >> 24) * .7f) << 24) +
                            ((uint8_t)((uint8_t)(tex_color >> 16) * .7f) << 16) +
                            ((uint8_t)((uint8_t)(tex_color >>  8) * .7f) <<  8) +
                            ((uint8_t)((uint8_t)(tex_color >>  0) * .7f) <<  0);
            }

            *slice_buf = tex_color;
        }
        slice_buf++;
    }
}

#endif // SIMPLE_SLICE_HPP
