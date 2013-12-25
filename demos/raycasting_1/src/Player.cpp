#include "Player.hpp"
#include "World.hpp"
#include "Input.hpp"
#include "Utils.hpp"

#include <algorithm>

Player::Player(const World& world)
    : mWorld(world)
    , mInput(Utils::make_unique<Input>(SDL_SCANCODE_W, SDL_SCANCODE_S,
                                       SDL_SCANCODE_A, SDL_SCANCODE_D,
                                       SDL_SCANCODE_E, SDL_SCANCODE_F))
{

}

Player::~Player()
{

}

Input& Player::GetInput() const
{
    return *mInput;
}

void Player::Update(const long elapsed_time)
{
    UpdateRays(elapsed_time);
    UpdateRotation(elapsed_time);
    UpdateMovement(elapsed_time);
}

void Player::SetHorizontalRayCount(const int ray_cnt)
{
    mRays.resize(ray_cnt);
}

const std::vector<RaycastResult>& Player::GetRaycastResults() const
{
    return mRays;
}

void Player::UpdateRays(const long elapsed_time)
{
    (void) elapsed_time;

    auto is_level_blocking = [&](const int map_x, const int map_y) {
        return mWorld.IsBlocking(map_x, map_y);
    };

    const auto ray_count = mRays.size();

    for (auto x = 0u; x < ray_count; x++)
    {
        // Current column position relative to the center of the screen.
        // Left edge is -1, right edge is 1, and center is 0.
        const double cam_x = 2.0 * x / ray_count - 1;

        // Starting direction of the current ray to be cast.
        const double ray_dir_x = mDirX + (mPlaneX * cam_x);
        const double ray_dir_y = mDirY + (mPlaneY * cam_x);

        cast_ray(mPosX, mPosY, ray_dir_x, ray_dir_y, is_level_blocking, mRays[x]);
    }
}

void Player::UpdateRotation(const long elapsed_time)
{
    const double rot_speed = elapsed_time / 1000. * 2.0;

    if (auto distance = mInput->TestMotionLeft()) // rotate to the left
    {
        const auto rot_dist = rot_speed * distance;

        // Rotate direction plane.
        const auto old_mDirX = mDirX;
        mDirX = old_mDirX * std::cos(rot_dist) - mDirY * std::sin(rot_dist);
        mDirY = old_mDirX * std::sin(rot_dist) + mDirY * std::cos(rot_dist);

        // Rotate camera plane.
        const auto old_plane_x = mPlaneX;
        mPlaneX = old_plane_x * std::cos(rot_dist) - mPlaneY * std::sin(rot_dist);
        mPlaneY = old_plane_x * std::sin(rot_dist) + mPlaneY * std::cos(rot_dist);
    }

    if (auto distance = mInput->TestMotionRight()) // rotate to the right
    {
        const auto rot_dist = rot_speed * distance;

        // Rotate direction plane.
        const auto old_mDirX = mDirX;
        mDirX = old_mDirX * std::cos(-rot_dist) - mDirY * std::sin(-rot_dist);
        mDirY = old_mDirX * std::sin(-rot_dist) + mDirY * std::cos(-rot_dist);

        // Rotate camera plane.
        const auto old_plane_x = mPlaneX;
        mPlaneX = old_plane_x * std::cos(-rot_dist) - mPlaneY * std::sin(-rot_dist);
        mPlaneY = old_plane_x * std::sin(-rot_dist) + mPlaneY * std::cos(-rot_dist);
    }

    if (auto distance = mInput->TestMotionUp()) // look up
    {
        mVerticalLook -= distance;
    }

    if (auto distance = mInput->TestMotionDown()) // look down
    {
        mVerticalLook += distance;
    }
}

void Player::UpdateMovement(const long elapsed_time)
{
    const double move_speed = elapsed_time / 1000. * 4.0;

    if (mInput->TestUp()) // move forward
    {
        const auto new_pos_x = mPosX + (mDirX * move_speed);
        const auto new_pos_y = mPosY + (mDirY * move_speed);
        MoveTo(new_pos_x, new_pos_y);
    }

    if (mInput->TestDown()) // move backward
    {
        const auto new_pos_x = mPosX - (mDirX * move_speed);
        const auto new_pos_y = mPosY - (mDirY * move_speed);
        MoveTo(new_pos_x, new_pos_y);
    }

    if (mInput->TestLeft()) // strafe left
    {
        const auto rad = 90.0 * M_PI / 180.0;

        const auto dir_x = mDirX * std::cos(rad) - mDirY * std::sin(rad);
        const auto dir_y = mDirX * std::sin(rad) + mDirY * std::cos(rad);

        const auto new_pos_x = mPosX + (dir_x * move_speed);
        const auto new_pos_y = mPosY + (dir_y * move_speed);
        MoveTo(new_pos_x, new_pos_y);
    }

    if (mInput->TestRight()) // strafe right
    {
        const auto rad = 90.0 * M_PI / 180.0;

        const auto dir_x = mDirX * std::cos(-rad) - mDirY * std::sin(-rad);
        const auto dir_y = mDirX * std::sin(-rad) + mDirY * std::cos(-rad);

        const auto new_pos_x = mPosX + (dir_x * move_speed);
        const auto new_pos_y = mPosY + (dir_y * move_speed);
        MoveTo(new_pos_x, new_pos_y);
    }
}

void Player::MoveTo(const double x, const double y)
{
    const double radius = 0.3;

    auto new_pos_x = x;
    auto new_pos_y = y;
    auto new_cell_x = static_cast<int>(new_pos_x);
    auto new_cell_y = static_cast<int>(new_pos_y);

    if (mWorld.IsBlocking(new_cell_x, new_cell_y)) {
        return;
    }

    mPosX = new_pos_x;
    mPosY = new_pos_y;

    const auto blockTop = mWorld.IsBlocking(new_cell_x, new_cell_y - 1);
    if (blockTop && ((new_pos_y - new_cell_y) < radius)) {
        new_pos_y = new_cell_y + radius;
        mPosY = new_pos_y;
    }

    const auto blockBottom = mWorld.IsBlocking(new_cell_x, new_cell_y + 1);
    if (blockBottom && (((new_cell_y + 1) - new_pos_y) < radius)) {
        new_pos_y = new_cell_y + 1 - radius;
        mPosY = new_pos_y;
    }

    const auto blockLeft = mWorld.IsBlocking(new_cell_x - 1, new_cell_y);
    if (blockLeft && ((new_pos_x - new_cell_x) < radius)) {
        new_pos_x = new_cell_x + radius;
        mPosX = new_pos_x;
    }

    const auto blockRight = mWorld.IsBlocking(new_cell_x + 1, new_cell_y);
    if (blockRight && (((new_cell_x + 1) - new_pos_x) < radius)) {
        new_pos_x = new_cell_x + 1 - radius;
        mPosX = new_pos_x;
    }

    // check top left
    if (mWorld.IsBlocking(new_cell_x - 1, new_cell_y - 1) && !(blockTop && blockLeft))
    {
        const auto dx = new_pos_x - new_cell_x;
        const auto dy = new_pos_y - new_cell_y;

        const auto dxdx = std::pow(dx, 2);
        const auto dydy = std::pow(dy, 2);

        if ((dxdx + dydy) < std::pow(radius, 2))
        {
            if (dxdx > dydy) {
                new_pos_x = new_cell_x + radius;
                mPosX = new_pos_x;
            }
            else {
                new_pos_y = new_cell_y + radius;
                mPosY = new_pos_y;
            }
        }
    }

    // check top right
    if (mWorld.IsBlocking(new_cell_x + 1, new_cell_y - 1) && !(blockTop && blockRight))
    {
        const auto dx = new_pos_x - (new_cell_x + 1);
        const auto dy = new_pos_y - new_cell_y;

        const auto dxdx = std::pow(dx, 2);
        const auto dydy = std::pow(dy, 2);

        if ((dxdx + dydy) < std::pow(radius, 2))
        {
            if (dxdx > dydy) {
                new_pos_x = new_cell_x + 1 - radius;
                mPosX = new_pos_x;
            }
            else {
                new_pos_y = new_cell_y + radius;
                mPosY = new_pos_y;
            }
        }
    }

    // check bottom left
    if (mWorld.IsBlocking(new_cell_x - 1, new_cell_y + 1) && !(blockBottom && blockLeft))
    {
        const auto dx = new_pos_x - new_cell_x;
        const auto dy = new_pos_y - (new_cell_y + 1);

        const auto dxdx = std::pow(dx, 2);
        const auto dydy = std::pow(dy, 2);

        if ((dxdx + dydy) < std::pow(radius, 2))
        {
            if (dxdx > dydy) {
                new_pos_x = new_cell_x + radius;
                mPosX = new_pos_x;
            }
            else {
                new_pos_y = new_cell_y + 1 - radius;
                mPosY = new_pos_y;
            }
        }
    }

    // check bottom right
    if (mWorld.IsBlocking(new_cell_x + 1, new_cell_y + 1) && !(blockBottom && blockRight))
    {
        const auto dx = new_pos_x - (new_cell_x + 1);
        const auto dy = new_pos_y - (new_cell_y + 1);

        const auto dxdx = std::pow(dx, 2);
        const auto dydy = std::pow(dy, 2);

        if ((dxdx + dydy) < std::pow(radius, 2))
        {
            if (dxdx > dydy) {
                new_pos_x = new_cell_x + 1 - radius;
                mPosX = new_pos_x;
            }
            else {
                new_pos_y = new_cell_y + 1 - radius;
                mPosY = new_pos_y;
            }
        }
    }
}
