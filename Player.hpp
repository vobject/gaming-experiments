#ifndef PLAYER_HPP
#define PLAYER_HPP

class Level;
class Input;

class Player
{
   friend class Render;

public:
   Player(const Level& level, const Input& input);
   ~Player();

   void Update(int elapsed_time);

private:
   const Level& mLevel;
   const Input& mInput;

   double mPosX = 20.;
   double mPosY = 10.;

   double mDirX = -1.;
   double mDirY = 0.;

   double mPlaneX = 0.;
   double mPlaneY = 0.66;
};

#endif // PLAYER_HPP
