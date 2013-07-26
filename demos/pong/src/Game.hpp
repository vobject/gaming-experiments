#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <vector>

class Field;
class Goal;
class Player;
class Ball;

class Game
{
public:
   Game();
   ~Game();

   void Update(int elapsed_time);

   void SetField(std::shared_ptr<Field> field);
   void SetPlayer1(std::shared_ptr<Player> player);
   void SetPlayer2(std::shared_ptr<Player> player);
   void SetPlayer1Goal(std::shared_ptr<Goal> goal);
   void SetPlayer2Goal(std::shared_ptr<Goal> goal);
   void AddBall(std::shared_ptr<Ball> ball);

   std::shared_ptr<Field> GetField() const;
   std::shared_ptr<Player> GetPlayer1() const;
   std::shared_ptr<Player> GetPlayer2() const;
   std::shared_ptr<Goal> GetPlayer1Goal() const;
   std::shared_ptr<Goal> GetPlayer2Goal() const;
   std::vector<std::shared_ptr<Ball>> GetBalls() const;

private:
   std::shared_ptr<Field> mField;
   std::vector<std::shared_ptr<Ball>> mBalls;
   std::shared_ptr<Player> mPlayer1;
   std::shared_ptr<Player> mPlayer2;
   std::shared_ptr<Goal> mPlayer1Goal;
   std::shared_ptr<Goal> mPlayer2Goal;

   // TODO: Goals, Score, Time
};

#endif // GAME_HPP
