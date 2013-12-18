#include "Player.hpp"
#include "Level.hpp"
#include "Input.hpp"

#include <cmath>

Player::Player(const Level& level, const Input& input)
    : mLevel(level)
    , mInput(input)
{

}

Player::~Player()
{

}

void Player::Update(const int elapsed_time)
{
    UpdateRotation(elapsed_time);
    UpdateMovement(elapsed_time);
}

void Player::UpdateRotation(const int elapsed_time)
{
    const double rot_speed = elapsed_time / 1000. * 2.;

    if (auto distance = mInput.TestMotionLeft()) // Rotate to the left.
    {
        const auto rot_dist = rot_speed * distance;

        // Rotate direction plane.
        auto old_mDirX = mDirX;
        mDirX = old_mDirX * std::cos(rot_dist) - mDirY * std::sin(rot_dist);
        mDirY = old_mDirX * std::sin(rot_dist) + mDirY * std::cos(rot_dist);

        // Rotate camera plane.
        auto old_plane_x = mPlaneX;
        mPlaneX = old_plane_x * std::cos(rot_dist) - mPlaneY * std::sin(rot_dist);
        mPlaneY = old_plane_x * std::sin(rot_dist) + mPlaneY * std::cos(rot_dist);
    }

    if (auto distance = mInput.TestMotionRight()) // Rotate to the right.
    {
        const auto rot_dist = rot_speed * distance;

        // Rotate direction plane.
        auto old_mDirX = mDirX;
        mDirX = old_mDirX * std::cos(-rot_dist) - mDirY * std::sin(-rot_dist);
        mDirY = old_mDirX * std::sin(-rot_dist) + mDirY * std::cos(-rot_dist);

        // Rotate camera plane.
        auto old_plane_x = mPlaneX;
        mPlaneX = old_plane_x * std::cos(-rot_dist) - mPlaneY * std::sin(-rot_dist);
        mPlaneY = old_plane_x * std::sin(-rot_dist) + mPlaneY * std::cos(-rot_dist);
    }
}

void Player::UpdateMovement(const int elapsed_time)
{
    const double move_speed = elapsed_time / 1000. * 4.;

    if (mInput.TestUp()) // Move forward if no wall blocks our path.
    {
        const auto new_pos_x = mPosX + mDirX * move_speed;
        const auto new_pos_y = mPosY + mDirY * move_speed;

        if (!mLevel.IsBlocking(new_pos_x, new_pos_y)) {
            mPosX += mDirX * move_speed;
            mPosY += mDirY * move_speed;
        }
    }

    if (mInput.TestDown()) // Move backward if no wall blocks our path.
    {
        const auto new_pos_x = mPosX - mDirX * move_speed;
        const auto new_pos_y = mPosY - mDirY * move_speed;

        if (!mLevel.IsBlocking(new_pos_x, new_pos_y)) {
            mPosX -= mDirX * move_speed;
            mPosY -= mDirY * move_speed;
        }
    }

    // TODO: Check if player is too close to a wall before moving
    // TODO: Strafing
}
