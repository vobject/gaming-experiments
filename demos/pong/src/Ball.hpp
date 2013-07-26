#ifndef BALL_HPP
#define BALL_HPP

#include <memory>

class Field;
class Player;

class Ball
{
   friend class Player;
   friend class SwRenderer;

public:
   Ball(float pos_x, float pos_y, float size_x, float size_y);
   ~Ball();

   void Update(int elapsed_time, const std::shared_ptr<Field>& field);

   void SetDirX(float dir_x);
   void SetDirY(float dir_y);

   // set and get the last player that touched the ball (used for scoring).
   void SetOwner(std::shared_ptr<Player> owner);
   std::shared_ptr<Player> GetOwner() const;

private:
   void UpdateMovementWalls(int elapsed_time, const Field& field);
//   void UpdateMovementGoals(int elapsed_time, const Field& field);

   float mPosX;
   float mPosY;
   float mSizeX;
   float mSizeY;

   // default values. will be changed by walls and players.
   float mDirX = 1.0f;
   float mDirY = 0.0f;
   float mSpeed = 0.4f;

   std::shared_ptr<Player> mOwner;
};

#endif // BALL_HPP
