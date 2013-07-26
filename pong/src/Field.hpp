#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>
#include <cstdint>

class Field
{
   friend class SwRenderer;

public:
   Field(float pos_x, float pos_y, float size_x, float size_y);
   ~Field();

   float GetTopBorder() const;
   float GetBottomBorder() const;
   float GetLeftBorder() const;
   float GetRightBorder() const;

private:
   const float mPosX;
   const float mPosY;
   const float mSizeX;
   const float mSizeY;
};

#endif // FIELD_HPP
