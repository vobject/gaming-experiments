#include "Game.hpp"
#include "Field.hpp"
#include "Goal.hpp"
#include "Ball.hpp"
#include "Player.hpp"

Game::Game()
{

}

Game::~Game()
{

}

void Game::Update(const int elapsed_time)
{
   mPlayer1->Update(elapsed_time, mField, mBalls);
   mPlayer2->Update(elapsed_time, mField, mBalls);

   for (auto& obj : mBalls) {
      obj->Update(elapsed_time, mField);
   }
}

void Game::SetField(std::shared_ptr<Field> field)
{
   mField = field;
}

void Game::SetPlayer1(std::shared_ptr<Player> player)
{
   mPlayer1 = player;

   if (mPlayer2)
   {
      mPlayer1->SetOpponent(mPlayer2);
      mPlayer2->SetOpponent(mPlayer1);
   }
}

void Game::SetPlayer2(std::shared_ptr<Player> player)
{
   mPlayer2 = player;

   if (mPlayer1)
   {
      mPlayer2->SetOpponent(mPlayer1);
      mPlayer1->SetOpponent(mPlayer2);
   }
}

void Game::SetPlayer1Goal(std::shared_ptr<Goal> goal)
{
   mPlayer1Goal = goal;
   mPlayer1Goal->SetOwner(mPlayer1);
}

void Game::SetPlayer2Goal(std::shared_ptr<Goal> goal)
{
   mPlayer2Goal = goal;
   mPlayer2Goal->SetOwner(mPlayer2);
}

void Game::AddBall(std::shared_ptr<Ball> ball)
{
   mBalls.push_back(ball);
}

std::shared_ptr<Field> Game::GetField() const
{
   return mField;
}

std::shared_ptr<Player> Game::GetPlayer1() const
{
   return mPlayer1;
}

std::shared_ptr<Player> Game::GetPlayer2() const
{
   return mPlayer2;
}

std::shared_ptr<Goal> Game::GetPlayer1Goal() const
{
   return mPlayer1Goal;
}

std::shared_ptr<Goal> Game::GetPlayer2Goal() const
{
   return mPlayer2Goal;
}

std::vector<std::shared_ptr<Ball>> Game::GetBalls() const
{
   return mBalls;
}
