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
    const double rot_speed = elapsed_time / 1000. * 3.;

    if (mInput.TestLeft()) // Rotate to the left.
    {
        // Rotate direction plane.
        auto old_mDirX = mDirX;
        mDirX = old_mDirX * std::cos(rot_speed) - mDirY * std::sin(rot_speed);
        mDirY = old_mDirX * std::sin(rot_speed) + mDirY * std::cos(rot_speed);

        // Rotate camera plane.
        auto old_plane_x = mPlaneX;
        mPlaneX = old_plane_x * std::cos(rot_speed) - mPlaneY * std::sin(rot_speed);
        mPlaneY = old_plane_x * std::sin(rot_speed) + mPlaneY * std::cos(rot_speed);
    }

    if (mInput.TestRight()) // Rotate to the right.
    {
        // Rotate direction plane.
        auto old_mDirX = mDirX;
        mDirX = old_mDirX * std::cos(-rot_speed) - mDirY * std::sin(-rot_speed);
        mDirY = old_mDirX * std::sin(-rot_speed) + mDirY * std::cos(-rot_speed);

        // Rotate camera plane.
        auto old_plane_x = mPlaneX;
        mPlaneX = old_plane_x * std::cos(-rot_speed) - mPlaneY * std::sin(-rot_speed);
        mPlaneY = old_plane_x * std::sin(-rot_speed) + mPlaneY * std::cos(-rot_speed);
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
}
