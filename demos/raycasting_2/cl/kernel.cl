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
    float rotation;
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

struct ray
{
    int map_x;
    int map_y;
    float intersect_x;
    float intersect_y;
    float distance;
    bool verticalhit;
};

__constant struct pixel ceiling_color = { 0x60, 0x60, 0x60 };
__constant struct pixel floor_color = { 0x80, 0x80, 0x80 };


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

struct ray cast_ray(
    const struct screen_params scr,
    const int x,
    struct player_params player,
    const struct level_params lp,
    __global int* lvl
)
{
    struct ray r;
    r.map_x = player.pos_x;
    r.map_y = player.pos_y;

    // Current column position relative to the center of the screen.
    // Left edge is -1, right edge is 1, and center is 0.
    const float cam_x = 2.f * x / scr.res_x - 1;

    // Starting direction of the current ray to be cast.
    const float ray_dir_x = player.dir_x + (player.plane_x * cam_x);
    const float ray_dir_y = player.dir_y + (player.plane_y * cam_x);

    // Length of the ray from one X and Y-side to next X and Y-side.
    const float delta_dist_x = sqrt(pow(ray_dir_y, 2) / pow(ray_dir_x, 2) + 1);
    const float delta_dist_y = sqrt(pow(ray_dir_x, 2) / pow(ray_dir_y, 2) + 1);

    int step_x; // +1 if heading north, -1 if heading south.
    int step_y; // +1 if heading east, -1 if heading west.
    float side_dist_x;
    float side_dist_y;

    if (ray_dir_x < .0f) {
        step_x = -1;
        side_dist_x = (player.pos_x - r.map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (r.map_x + 1.f - player.pos_x) * delta_dist_x;
    }

    if (ray_dir_y < .0f) {
        step_y = -1;
        side_dist_y = (player.pos_y - r.map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = (r.map_y + 1.f - player.pos_y) * delta_dist_y;
    }

    for (;;)
    {
        if (side_dist_x < side_dist_y) {
            // Jump one square in X-direction.
            r.map_x += step_x;
            side_dist_x += delta_dist_x;
            r.verticalhit = false;
        } else {
            // Jump one square in Y-direction.
            r.map_y += step_y;
            side_dist_y += delta_dist_y;
            r.verticalhit = true;
        }

        // Check if the ray has hit a wall.
        if ((r.map_x < 0) || (r.map_x >= lp.level_x) ||
            (r.map_y < 0) || (r.map_y >= lp.level_y) ||
            (lvl[r.map_y + (r.map_x * lp.level_y)] != 0))
        {
            break;
        }
    }

    if (!r.verticalhit)
    {
        const float unit_x = r.map_x + (1 - step_x) / 2;
        const float intersect_x = player.pos_y + ((unit_x - player.pos_x) / ray_dir_x) * ray_dir_y;

        r.intersect_x = unit_x;
        r.intersect_y = intersect_x;

        // FIXME: May be to small.
        r.distance = fabs((unit_x - player.pos_x) / ray_dir_x);
    }
    else
    {
        const float unit_y = r.map_y + (1 - step_y) / 2;
        const float intersect_y = player.pos_x + ((unit_y - player.pos_y) / ray_dir_y) * ray_dir_x;

        r.intersect_x = unit_y;
        r.intersect_y = intersect_y;

        // FIXME: May be too small.
        r.distance = fabs((unit_y - player.pos_y) / ray_dir_y);
    }
    return r;
}

void draw_slice(
    __global struct pixel* data,
    const struct screen_params scr,
    const int x,
    const struct ray ray,
    const struct player_params player,
    const struct level_params lp,
    __global int* lvl,

    const struct wall_params wp,
    __global struct pixel* wall1,
    __global struct pixel* wall2,
    __global struct pixel* wall3,
    __global struct pixel* wall4
)
{
    // Current column position relative to the center of the screen.
    // Left edge is -1, right edge is 1, and center is 0.
    const float cam_x = 2.f * x / scr.res_x - 1;

    // Starting direction of the current ray to be cast.
    const float ray_dir_x = player.dir_x + (player.plane_x * cam_x);
    const float ray_dir_y = player.dir_y + (player.plane_y * cam_x);

    // Calculate the height of the vertical line to draw on screen.
    const int wall_height = abs((int)(scr.res_y / ray.distance));

    // Set where the vertical line should be drawn.
    int wall_start = (scr.res_y / 2) - (wall_height / 2);
    int wall_end = (scr.res_y / 2) + (wall_height / 2);

    if (wall_start < 0) {
        wall_start = 0;
    }

    if (wall_end >= scr.res_y) {
        wall_end = scr.res_y - 1;
    }

    __global struct pixel* mywall;
    switch (lvl[ray.map_y + (ray.map_x * lp.level_y)])
    {
        case 1:  mywall = wall1; break;
        case 2:  mywall = wall2; break;
        case 3:  mywall = wall3; break;
        case 4:  mywall = wall4; break;
        default: mywall = wall1; break;
    }

    // Where exactly the wall was hit.
    float wall_x = ray.intersect_y;
    wall_x -= floor(wall_x);

    // X-coordinate on the texture.
    int tex_x = wall_x * wp.res_x;

    if (( ray.verticalhit && ray_dir_y < 0) ||
        (!ray.verticalhit && ray_dir_x > 0))
    {
        tex_x = wp.res_x - tex_x - 1;
    }

    for (int y = 0; y < wall_start; y++)
    {
        const int xy = x + (y * scr.res_x);
        data[xy] = ceiling_color;
    }

    for (int y = wall_start; y < wall_end; y++)
    {
        const int tex_y = (y * 2 - scr.res_y + wall_height) *
                           (wp.res_y / 2) / wall_height;
        const int tex_xy = tex_x + (tex_y * wp.res_x);

        struct pixel wall_px = mywall[tex_xy];

        if (ray.verticalhit)
        {
            // Give X and Y-sides different brightness.
            wall_px.r /= 2;
            wall_px.g /= 2;
            wall_px.b /= 2;
        }

        const int xy = x + (y * scr.res_x);
        data[xy] = wall_px;
    }

    __global struct pixel* myfloor = wall3;

    // Position of the floor at the bottom of the wall.
    float floor_x_wall;
    float floor_y_wall;

    if (!ray.verticalhit && (ray_dir_x > 0)) {
        floor_x_wall = ray.intersect_x;
        floor_y_wall = ray.intersect_y + wall_x;
    }
    else if (!ray.verticalhit && (ray_dir_x < 0)) {
        floor_x_wall = ray.intersect_x + 1.f;
        floor_y_wall = ray.intersect_y + wall_x;
    }
    else if (ray.verticalhit && (ray_dir_y > 0)) {
        floor_x_wall = ray.intersect_x + wall_x;
        floor_y_wall = ray.intersect_y;
    }
    else {
        floor_x_wall = ray.intersect_x + wall_x;
        floor_y_wall = ray.intersect_y + 1.f;
    }

   const float dist_wall = ray.distance;
   const float dist_player = .0f;

    // Draw the floor from below the wall to the bottom of the screen.
    for (int y = wall_end; y < scr.res_y; y++)
    {
        const float cur_dist = scr.res_y / (2.f * y - scr.res_y);
        const float weight = (cur_dist - dist_player) / (dist_wall - dist_player);

        const float cur_floor_x = weight * floor_x_wall + (1.f - weight) * player.pos_x;
        const float cur_floor_y = weight * floor_y_wall + (1.f - weight) * player.pos_y;

        const int floor_tex_x = (int)(cur_floor_x * wp.res_x / 4) % wp.res_x;
        const int floor_tex_y = (int)(cur_floor_y * wp.res_y / 4) % wp.res_y;

        const int floor_tex_xy = floor_tex_x + (floor_tex_y * wp.res_x);
        struct pixel floor_px = myfloor[floor_tex_xy];

        // Make the floor darker.
        floor_px.r /= 2;
        floor_px.g /= 2;
        floor_px.b /= 2;

        const int xy = x + (y * scr.res_x);
        data[xy] = floor_px;
    }
}


__kernel void rc_demo_2(
    __global struct pixel* data,
    const struct screen_params scr,
    const struct player_params player,
    const struct level_params lp,
    __global int* lvl,

    const struct wall_params wp,
    __global struct pixel* wall1,
    __global struct pixel* wall2,
    __global struct pixel* wall3,
    __global struct pixel* wall4
)
{
    const int x = (int)get_global_id(0);
    if (x >= scr.res_x) {
        return;
    }

    const struct ray ray = cast_ray(scr, x, player, lp, lvl);
    draw_slice(data, scr, x, ray, player, lp, lvl, wp, wall1, wall2, wall3, wall4);
/*
    for (int y = 0; y < scr.res_y; y++)
    {
        const int scr_xy = x + (y * scr.res_x);
        const int tex_xy = x + (y * wp.res_x);

        data[scr_xy] = wall2[tex_xy];
    }
*/
}
