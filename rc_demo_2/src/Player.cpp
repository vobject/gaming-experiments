#include "Player.hpp"
#include "Level.hpp"
#include "Input.hpp"
#include "Ray.hpp"
#include "Utils.hpp"

#include <cmath>
#include <cstdio>

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

double Player::GetRotation() const
{
   if (mDirY > .0) {
      return std::acos(mDirX) * 180. / M_PI;
   }
   else {
      return 360. - std::acos(mDirX) * 180. / M_PI;
   }
}

Vector Player::GetPosition() const
{
   return { mPosX, mPosY };
}

Vector Player::GetDirection() const
{
   return { mDirX, mDirY };
}

Vector Player::GetPlane() const
{
   return { mPlaneX, mPlaneY };
}

void Player::UpdateRotation(const int elapsed_time)
{
   const double rot_speed = elapsed_time / 1000. * 3.;

   if (mInput.TestLeft()) // Rotate to the left.
   {
      // Rotate direction plane.
      const auto old_dir_x = mDirX;
      mDirX = old_dir_x * std::cos(rot_speed) - mDirY * std::sin(rot_speed);
      mDirY = old_dir_x * std::sin(rot_speed) + mDirY * std::cos(rot_speed);

      // Rotate camera plane.
      const auto old_plane_x = mPlaneX;
      mPlaneX = old_plane_x * std::cos(rot_speed) - mPlaneY * std::sin(rot_speed);
      mPlaneY = old_plane_x * std::sin(rot_speed) + mPlaneY * std::cos(rot_speed);
   }

   if (mInput.TestRight()) // Rotate to the right.
   {
      // Rotate direction plane.
      const auto old_mDirX = mDirX;
      mDirX = old_mDirX * std::cos(-rot_speed) - mDirY * std::sin(-rot_speed);
      mDirY = old_mDirX * std::sin(-rot_speed) + mDirY * std::cos(-rot_speed);

      // Rotate camera plane.
      const auto old_plane_x = mPlaneX;
      mPlaneX = old_plane_x * std::cos(-rot_speed) - mPlaneY * std::sin(-rot_speed);
      mPlaneY = old_plane_x * std::sin(-rot_speed) + mPlaneY * std::cos(-rot_speed);
   }
}

void Player::UpdateMovement(const int elapsed_time)
{
   const double move_speed = elapsed_time / 1000. * 4.;

   if (mInput.TestUp()) // Move forward if no wall blocks our path.
   {
      //      printf("dirX=%f, dirY=%f\n", mDirX, mDirY); fflush(stdout);

      // Do not let the player get too close to the wall.
      const Ray ray(GetPosition(), GetDirection(), mLevel);
      if (ray.GetDistance() < .5) {
         return;
      }

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

//bool Player::TooCloseToWall() const
//{
//   const Ray ray_1(GetPosition(), {1, 0}, mLevel);
//   if (ray_1.GetDistance() < .5) {
//      return true;
//   }

//   const Ray ray_2(GetPosition(), {0, 1}, mLevel);
//   if (ray_2.GetDistance() < .5) {
//      return true;
//   }

//   const Ray ray_3(GetPosition(), {-1, 0}, mLevel);
//   printf("ray_3=%f\n", ray_3.GetDistance()); fflush(stdout);
//   if (ray_3.GetDistance() < .5) {
//      return true;
//   }

//   const Ray ray_4(GetPosition(), {0, -1}, mLevel);
//   if (ray_4.GetDistance() < .5) {
//      return true;
//   }

//   return false;
//}
