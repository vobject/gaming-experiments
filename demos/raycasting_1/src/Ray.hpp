#ifndef RAY_HPP
#define RAY_HPP

#include <algorithm>
#include <functional>

struct RaycastResult {
    const int map_pos_x;
    const int map_pos_y;
    const double intersect_x;
    const double intersect_y;
    const double distance;
    const bool vertical_hit;

    RaycastResult(const int map_pos_x, const int map_pos_y,
                  const double intersect_x, const double intersect_y,
                  const double distance, const bool vertical_hit)
        : map_pos_x{ map_pos_x }
        , map_pos_y{ map_pos_y }
        , intersect_x{ intersect_x }
        , intersect_y{ intersect_y }
        , distance{ distance }
        , vertical_hit{ vertical_hit }
    { }

    RaycastResult(RaycastResult&) = delete;
    RaycastResult& operator=(RaycastResult&) = delete;
};

inline RaycastResult cast_ray(const double ray_pos_x, const double ray_pos_y,
                              const double ray_dir_x, const double ray_dir_y,
                              std::function<bool(int, int)> is_level_blocking)
{
    // player's current grid position inside the level
    int map_x = static_cast<int>(ray_pos_x);
    int map_y = static_cast<int>(ray_pos_y);

    // length of the ray from one X- and Y-side to next X- and Y-side.
    const double delta_dist_x = std::sqrt(std::pow(ray_dir_y, 2) / std::pow(ray_dir_x, 2) + 1);
    const double delta_dist_y = std::sqrt(std::pow(ray_dir_x, 2) / std::pow(ray_dir_y, 2) + 1);

    int step_x; // +1 if heading north, -1 if heading south.
    int step_y; // +1 if heading east, -1 if heading west.
    double side_dist_x;
    double side_dist_y;

    if (ray_dir_x < 0) {
        step_x = -1;
        side_dist_x = (ray_pos_x - map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (map_x + 1. - ray_pos_x) * delta_dist_x;
    }

    if (ray_dir_y < 0) {
        step_y = -1;
        side_dist_y = (ray_pos_y - map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = (map_y + 1. - ray_pos_y) * delta_dist_y;
    }

    bool vertical_side_hit = false;

    for (;;)
    {
        if (side_dist_x < side_dist_y)
        {
            // Jump one square in X-direction.
            map_x += step_x;
            side_dist_x += delta_dist_x;
            vertical_side_hit = false;
        }
        else
        {
            // Jump one square in Y-direction.
            map_y += step_y;
            side_dist_y += delta_dist_y;
            vertical_side_hit = true;
        }

        // check if the ray has hit a wall
        if (is_level_blocking(map_x, map_y)) {
            break;
        }
    }

    double intersect_x;
    double intersect_y;
    double distance;

    if (!vertical_side_hit)
    {
        intersect_x = map_x + (1 - step_x) / 2;
        intersect_y = ray_pos_y + ((intersect_x - ray_pos_x) / ray_dir_x) * ray_dir_y;

        // FIXME: May be to small.
        distance = std::abs((intersect_x - ray_pos_x) / ray_dir_x);
    }
    else
    {
        intersect_x = map_y + (1 - step_y) / 2;
        intersect_y = ray_pos_x + ((intersect_x - ray_pos_y) / ray_dir_y) * ray_dir_x;

        // FIXME: May be too small.
        distance = std::abs((intersect_x - ray_pos_y) / ray_dir_y);
    }

    return { map_x, map_y, intersect_x, intersect_y, distance, vertical_side_hit };
}

#endif // RAY_HPP
