#include "Field.hpp"

Field::Field(const float pos_x, const float pos_y,
             const float size_x, const float size_y)
   : mPosX(pos_x)
   , mPosY(pos_y)
   , mSizeX(size_x)
   , mSizeY(size_y)
{

}

Field::~Field()
{

}

float Field::GetTopBorder() const
{
   return mPosY;
}

float Field::GetBottomBorder() const
{
   return (mPosY + mSizeY);
}

float Field::GetLeftBorder() const
{
   return mPosX;
}

float Field::GetRightBorder() const
{
   return (mPosX + mSizeX);
}
