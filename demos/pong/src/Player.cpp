#include "Player.hpp"
#include "Input.hpp"
#include "Field.hpp"
#include "Ball.hpp"

#include <iostream>

Player::Player(
   const std::string& name, const Input& input,
   const float pos_x, const float pos_y,
   const float size_x, const float size_y
)
   : mName(name)
   , mInput(input)
   , mPosX(pos_x)
   , mPosY(pos_y)
   , mSizeX(size_x)
   , mSizeY(size_y)
{

}

Player::~Player()
{

}

void Player::Update(
   const int elapsed_time,
   const std::shared_ptr<Field>& field,
   std::vector<std::shared_ptr<Ball>>& balls
)
{
   UpdateMovement(elapsed_time, *field);

   for (auto& obj : balls) {
      UpdateHit(*obj);
   }
}

//void Player::SetOpponent(const std::shared_ptr<Player>& opponent)
//{
//   mOpponent = opponent;
//}

//std::shared_ptr<Player> Player::GetOpponent() const
//{
//   return mOpponent;
//}

void Player::IncreaseScore()
{
   mScore++;
}

int Player::GetScore() const
{
   return mScore;
}

void Player::UpdateMovement(const int elapsed_time, const Field& field)
{
   const auto move_speed = elapsed_time / 1000.f / 2.f;

   if (mInput.TestUp())
   {
      const auto pos_top = mPosY - (mSizeY / 2.f);
      const auto new_pos_top = pos_top - move_speed;

      if (new_pos_top > field.GetTopBorder())
      {
         mPosY -= move_speed;
         mDir = MovementDir::Up;
         return;
      }
   }

   if (mInput.TestDown())
   {
      const auto pos_bottom = mPosY + (mSizeY / 2.f);
      const auto new_pos_bottom = pos_bottom + move_speed;

      if (new_pos_bottom < field.GetBottomBorder())
      {
         mPosY += move_speed;
         mDir = MovementDir::Down;
         return;
      }
   }

   mDir = MovementDir::None;
   return;
}

void Player::UpdateHit(Ball& ball)
{
   const auto pos_top = mPosY - (mSizeY / 2.f);
   const auto pos_bottom = mPosY + (mSizeY / 2.f);
   const auto pos_left = mPosX - (mSizeX / 2.f);
   const auto pos_right = mPosX + (mSizeX / 2.f);

   const auto ball_pos_top = ball.mPosY - (ball.mSizeY / 2.f);
   const auto ball_pos_bottom = ball.mPosY + (ball.mSizeY / 2.f);
   const auto ball_pos_left = ball.mPosX - (ball.mSizeX / 2.f);
   const auto ball_pos_right = ball.mPosX + (ball.mSizeX / 2.f);

   if ((ball_pos_bottom < pos_top) || (ball_pos_top > pos_bottom)) {
      // the ball is not even horizontally aligned with the player.
      return;
   }

   if ((ball_pos_right > pos_left) && (ball_pos_left < pos_left))
   {
      ball.SetDirX(-1.0f);
      ball.SetOwner(shared_from_this());

      if (mDir == MovementDir::Up) {
         ball.SetDirY(-1.0f);
      } else if (mDir == MovementDir::Down) {
         ball.SetDirY(1.0f);
      }
      return;
   }

   if ((ball_pos_left < pos_right) && (ball_pos_right > pos_right))
   {
      ball.SetDirX(1.0f);
      ball.SetOwner(shared_from_this());

      if (mDir == MovementDir::Up) {
         ball.SetDirY(-1.0f);
      } else if (mDir == MovementDir::Down) {
         ball.SetDirY(1.0f);
      }
      return;
   }
}
