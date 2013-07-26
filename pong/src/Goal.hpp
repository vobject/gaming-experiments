#ifndef GOAL_HPP
#define GOAL_HPP

#include <memory>

class Field;
class Player;

class Goal
{
   friend class SwRenderer;

public:
   Goal(float pos_x, float pos_y, float size_x, float size_y);
   ~Goal();

   void SetOwner(std::shared_ptr<Player> owner);
   std::shared_ptr<Player> GetOwner() const;

private:
   void UpdateMovement(int elapsed_time, const Field& field);

   float mPosX;
   float mPosY;
   float mSizeX;
   float mSizeY;

   std::shared_ptr<Player> mOwner;
};

#endif // GOAL_HPP
