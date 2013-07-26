#include "Ball.hpp"
#include "Input.hpp"
#include "Field.hpp"

Ball::Ball(
   const float pos_x, const float pos_y,
   const float size_x, const float size_y
)
   : mPosX(pos_x)
   , mPosY(pos_y)
   , mSizeX(size_x)
   , mSizeY(size_y)
{

}

Ball::~Ball()
{

}

void Ball::Update(const int elapsed_time, const std::shared_ptr<Field>& field)
{
   UpdateMovementWalls(elapsed_time, *field);
//   UpdateMovementGoals(elapsed_time, *field);
}

void Ball::SetDirX(const float dir_x)
{
   if ((dir_x < -1.0f) || (dir_x > 1.0f)) {
      throw "Invalid X ball direction.";
   }
   mDirX = dir_x;
}

void Ball::SetDirY(const float dir_y)
{
   if ((dir_y < -1.0f) || (dir_y > 1.0f)) {
      throw "Invalid Y ball direction.";
   }
   mDirY = dir_y;
}

void Ball::SetOwner(std::shared_ptr<Player> owner)
{
   mOwner = owner;
}

std::shared_ptr<Player> Ball::GetOwner() const
{
   return mOwner;
}

void Ball::UpdateMovementWalls(const int elapsed_time, const Field& field)
{
   const auto move_speed = elapsed_time / 1000.f * mSpeed;

   if (mDirX < 0.0f)
   {
      const auto pos_left = mPosX - (mSizeX / 2.f);
      const auto new_pos_left = pos_left + (move_speed * mDirX);
      if (new_pos_left > field.GetLeftBorder())
      {
         mPosX += (move_speed * mDirX);
      }
      else
      {
         SetDirX(-mDirX);
      }
   }
   else if (mDirX > 0.0f)
   {
      const auto pos_right = mPosX + (mSizeX / 2.f);
      const auto new_pos_right = pos_right + (move_speed * mDirX);
      if (new_pos_right < field.GetRightBorder())
      {
         mPosX += (move_speed * mDirX);
      }
      else
      {
         SetDirX(-mDirX);
      }
   }

   if (mDirY < 0.0f)
   {
      const auto pos_top = mPosY - (mSizeY / 2.f);
      const auto new_pos_top = pos_top + (move_speed * mDirY);
      if (new_pos_top > field.GetTopBorder())
      {
         mPosY += (move_speed * mDirY);
      }
      else
      {
         SetDirY(-mDirY);
      }
   }
   else if (mDirY > 0.0f)
   {
      const auto pos_bottom = mPosY + (mSizeY / 2.f);
      const auto new_pos_bottom = pos_bottom + (move_speed * mDirY);
      if (new_pos_bottom < field.GetBottomBorder())
      {
         mPosY += (move_speed * mDirY);
      }
      else
      {
         SetDirY(-mDirY);
      }
   }
}

//void Ball::UpdateMovementGoals(const int elapsed_time, const Field& field)
//{
//   const auto move_speed = elapsed_time / 1000.f * mSpeed;

//   if (mDirX < 0.0f)
//   {
//      const auto pos_left = mPosX - (mSizeX / 2.f);
//      const auto new_pos_left = pos_left + (move_speed * mDirX);
//      if (new_pos_left > field.GetLeftBorder())
//      {
//         mPosX += (move_speed * mDirX);
//      }
//      else
//      {
//         SetDirX(-mDirX);
//      }
//   }
//   else if (mDirX > 0.0f)
//   {
//      const auto pos_right = mPosX + (mSizeX / 2.f);
//      const auto new_pos_right = pos_right + (move_speed * mDirX);
//      if (new_pos_right < field.GetRightBorder())
//      {
//         mPosX += (move_speed * mDirX);
//      }
//      else
//      {
//         SetDirX(-mDirX);
//      }
//   }

//   if (mDirY < 0.0f)
//   {
//      const auto pos_top = mPosY - (mSizeY / 2.f);
//      const auto new_pos_top = pos_top + (move_speed * mDirY);
//      if (new_pos_top > field.GetTopBorder())
//      {
//         mPosY += (move_speed * mDirY);
//      }
//      else
//      {
//         SetDirY(-mDirY);
//      }
//   }
//   else if (mDirY > 0.0f)
//   {
//      const auto pos_bottom = mPosY + (mSizeY / 2.f);
//      const auto new_pos_bottom = pos_bottom + (move_speed * mDirY);
//      if (new_pos_bottom < field.GetBottomBorder())
//      {
//         mPosY += (move_speed * mDirY);
//      }
//      else
//      {
//         SetDirY(-mDirY);
//      }
//   }
//}
