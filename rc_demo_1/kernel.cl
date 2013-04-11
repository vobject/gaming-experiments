#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

struct screen_params
{
    int res_x;
    int res_y;
};

struct level_params
{
    int level_x;
    int level_y;
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
    uchar r;
    uchar g;
    uchar b;
    uchar unused;
};

__constant int world_map[24][24]=
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};




__kernel void rc_demo_1(
    __global struct pixel* data,
    const struct screen_params scr,
    const struct player_params player,
    const struct level_params lp,
    __global int* lvl
)
{
    const int x = (int)get_global_id(0);
    if (x >= scr.res_x) {
        return;
    }

    // Current column position relative to the center of the screen.
    // Left edge is -1, right edge is 1, and center is 0.
    const float cam_x = 2.f * x / scr.res_x - 1;

    // Starting direction and  positionof the current ray to be cast.
    const float ray_dir_x = player.dir_x + player.plane_x * cam_x;
    const float ray_dir_y = player.dir_y + player.plane_y * cam_x;
    const float ray_pos_x = player.pos_x;
    const float ray_pos_y = player.pos_y;

    // The direction to step in X or Y-direction (either +1 or -1).
    const int step_x = (ray_dir_x >= 0) ? 1 : -1;
    const int step_y = (ray_dir_y >= 0) ? 1 : -1;

    // Length of ray from one X and Y-side to next X and Y-side.
    const float delta_dist_x = sqrt(1 + pow(ray_dir_y, 2) / pow(ray_dir_x, 2));
    const float delta_dist_y = sqrt(1 + pow(ray_dir_x, 2) / pow(ray_dir_y, 2));

    // The player's current grid position inside the level.
    int map_x = ray_pos_x;
    int map_y = ray_pos_y;

    // Length of ray from its current position to next X- and Y-side.
    float side_dist_x = (step_x == 1) ? ((map_x + 1.0f - ray_pos_x) * delta_dist_x) :
                                        ((ray_pos_x - map_x) * delta_dist_x);
    float side_dist_y = (step_y == 1) ? ((map_y + 1.0f - ray_pos_y) * delta_dist_y) :
                                        ((ray_pos_y - map_y) * delta_dist_y);

    // Y walls (EW) will be drawn darker.
    bool y_side_hit;

    for (bool wall_hit = false; !wall_hit;) // Run the DDA algorithm.
    {
        if (side_dist_x < side_dist_y)
        {
            // Jump one square in X-direction.
            map_x += step_x;
            side_dist_x += delta_dist_x;
            y_side_hit = false;
        }
        else
        {
            // Jump one square in Y-direction.
            map_y += step_y;
            side_dist_y += delta_dist_y;
            y_side_hit = true;
        }

        // Check if the ray has hit a wall.
//        wall_hit = lvl[map_x + (map_y * lp.level_x)] > 0;
        wall_hit = world_map[map_x][map_y] > 0;
    }

    // Calculate the perpendicular distance projected on camera direction.
    // Oblique distance would give fisheye effect.
    const float perp_wall_dist = y_side_hit ?
                fabs((map_y - ray_pos_y + (1 - step_y) / 2) / ray_dir_y) :
                fabs((map_x - ray_pos_x + (1 - step_x) / 2) / ray_dir_x);

    // Calculate the height of the vertical line to draw on screen.
    const int line_height = abs((int)(scr.res_y / perp_wall_dist));

    // Set where the vertical line should be drawn.
    int line_start = (-line_height / 2) + (scr.res_y / 2);
    int line_end = line_height / 2 + scr.res_y / 2;

    if (line_start < 0) {
        line_start = 0;
    }

    if (line_end >= scr.res_y) {
        line_end = scr.res_y - 1;
    }

    struct pixel color;

//    switch (lvl[map_x + (map_y * lp.level_x)])
    switch (world_map[map_x][map_y])
    {
        case 1:  color = (struct pixel) { 0x00, 0x00, 0xff }; break; // red
        case 2:  color = (struct pixel) { 0x00, 0xff, 0x00 }; break; // green
        case 3:  color = (struct pixel) { 0xff, 0x00, 0x00 }; break; // blue
        case 4:  color = (struct pixel) { 0xff, 0xff, 0xff }; break; // white
        case 5:  color = (struct pixel) { 0x00, 0xff, 0xff }; break; // yellow
        default: color = (struct pixel) { 0x30, 0x30, 0x30 }; break; // dark gray
    }

    if (y_side_hit)
    {
        // Give X and Y-sides different brightness.
        color.r = color.r * .7f;
        color.g = color.g * .7f;
        color.b = color.b * .7f;
    }

    // DrawVerticalLine

    if (line_end < line_start)
    {
        // Make sure y_end is always greater than y_start.
        const int tmp = line_start;
        line_start = line_end;
        line_end = tmp;
    }

    if (line_end < 0 || line_start >= scr.res_y || x < 0 || x >= scr.res_x) {
        return; // Not a single point of the line is on screen.
    }

    if (line_start < 0)
    {
        line_start = 0; // clip
    }

    if (line_end >= scr.res_x)
    {
        line_end = scr.res_y - 1; // clip
    }

    // Ceiling
    for (int y = 0; y < line_start; y++)
    {
        const int xy = x + (y * scr.res_x);
        data[xy].b = 96; // B
        data[xy].g = 96; // G
        data[xy].r = 96; // R
    }

    // Wall
    for (int y = line_start; y <= line_end; y++)
    {
        const int xy = x + (y * scr.res_x);
        data[xy].b = color.b; // B
        data[xy].g = color.g; // G
        data[xy].r = color.r; // R
    }

    // Floor
    for (int y = line_end; y < scr.res_y; y++)
    {
        const int xy = x + (y * scr.res_x);
        data[xy].b = 128; // B
        data[xy].g = 128; // G
        data[xy].r = 128; // R
    }
}
