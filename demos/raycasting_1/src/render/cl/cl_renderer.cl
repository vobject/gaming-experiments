#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

struct screen_params
{
    uint res_x;
    uint res_y;
};

struct level_params
{
    uint level_x;
    uint level_y;
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

struct wall_params
{
   int res_x;
   int res_y;
};

struct pixel
{
    uchar r;
    uchar g;
    uchar b;
    uchar unused;
};

__constant struct pixel ceiling_color = { 96, 96, 96 };
__constant struct pixel floor_color = { 128, 128, 128 };


void swap(int* i, int* j)
{
    const int tmp = *i;
    *i = *j;
    *j = tmp;
}

void draw_vertical_line(
    __global struct pixel* data,
    const struct screen_params scr,
    const int x,
    int y_start,
    int y_end,
    const struct pixel wall_color
)
{
    // Make sure y_end is always be greater than y_start.
    if (y_end < y_start) { swap(&y_start, &y_end); }

    // Check if any point of the line appears on screen.
    if ((x < 0) || (x >= scr.res_x) || (y_end < 0) || (y_start >= scr.res_y)) { return; }

    // Clip start and end coordinates of the line onto the screen.
    if (y_start < 0)        { y_start = 0; }
    if (y_end >= scr.res_x) { y_end = scr.res_y - 1; }

    for (int y = 0; y < scr.res_y; y++)
    {
        const int xy = x + (y * scr.res_x);

        if (y < y_start)      { data[xy] = ceiling_color; } // Ceiling
        else if (y <= y_end)  { data[xy] = wall_color; }    // Wall
        else                  { data[xy] = floor_color; }   // Floor
    }
}

__kernel void rc_1(
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

    for (;;) // Run the DDA algorithm.
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
        if (lvl[map_y + (map_x * lp.level_y)] != 0) {
            break;
        }
    }

    // Calculate the perpendicular distance projected on camera direction.
    // Oblique distance would give fisheye effect.
    const float perp_wall_dist = y_side_hit ?
                fabs((map_y - ray_pos_y + (1 - step_y) / 2) / ray_dir_y) :
                fabs((map_x - ray_pos_x + (1 - step_x) / 2) / ray_dir_x);

    // Calculate the height of the vertical line to draw on screen.
    const int wall_height = abs((int)(scr.res_y / perp_wall_dist));

    // Set where the vertical line should be drawn.
    int wall_start = (-wall_height / 2) + (scr.res_y / 2);
    int wall_end = wall_height / 2 + scr.res_y / 2;

    if (wall_start < 0) {
        wall_start = 0;
    }

    if (wall_end >= scr.res_y) {
        wall_end = scr.res_y - 1;
    }

    struct pixel color;
    switch (lvl[map_y + (map_x * lp.level_y)])
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
        color.r *= .7f;
        color.g *= .7f;
        color.b *= .7f;
    }

    draw_vertical_line(data, scr, x, wall_start, wall_end, color);
}
