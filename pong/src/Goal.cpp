#include "Goal.hpp"
#include "Field.hpp"

Goal::Goal(
   const float pos_x, const float pos_y,
   const float size_x, const float size_y
)
   : mPosX(pos_x)
   , mPosY(pos_y)
   , mSizeX(size_x)
   , mSizeY(size_y)
{

}

Goal::~Goal()
{

}

void Goal::SetOwner(std::shared_ptr<Player> owner)
{
   mOwner = owner;
}

std::shared_ptr<Player> Goal::GetOwner() const
{
   return mOwner;
}
