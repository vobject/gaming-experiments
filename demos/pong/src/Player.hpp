#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
#include <string>
#include <vector>

class Input;
class Field;
class Ball;

enum class MovementDir
{
   None,
   Up,
   Down
};

class Player : public std::enable_shared_from_this<Player>
{
   friend class SwRenderer;

public:
   Player(const std::string& name, const Input& input,
          float pos_x, float pos_y, float size_x, float size_y);
   ~Player();

   void Update(int elapsed_time,
               const std::shared_ptr<Field>& field,
               std::vector<std::shared_ptr<Ball>>& balls);

//   void SetOpponent(const std::shared_ptr<Player>& opponent);
//   std::shared_ptr<Player> GetOpponent() const;

   void IncreaseScore();
   int GetScore() const;

private:
   void UpdateMovement(int elapsed_time, const Field& field);
   void UpdateHit(Ball& ball);

   const std::string mName;
   const Input& mInput;

   float mPosX;
   float mPosY;
   float mSizeX;
   float mSizeY;

   MovementDir mDir = MovementDir::None;

   int mScore = 0;
//   std::shared_ptr<Player> mOpponent;
};

#endif // PLAYER_HPP
