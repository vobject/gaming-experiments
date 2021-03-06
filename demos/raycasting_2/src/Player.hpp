#ifndef PLAYER_HPP
#define PLAYER_HPP

class Level;
class Input;
class Vector;

class Player
{
public:
   Player(const Level& level, const Input& input);
   ~Player();

   void Update(int elapsed_time);

   double GetRotation() const;
   Vector GetPosition() const;
   Vector GetDirection() const;
   Vector GetPlane() const;

private:
   void UpdateRotation(int elapsed_time);
   void UpdateMovement(int elapsed_time);

//   bool TooCloseToWall() const;

   const Level& mLevel;
   const Input& mInput;

   // Position vector of the player.
   double mPosX = 20.;
   double mPosY = 10.;

   // Direction vector of the player.
   double mDirX = -1.;
   double mDirY = 0.;

   // Camera plane of the player.
   // Must be perpendicular to the direction (but can change its length).
   // The ratio between direction length and the camera plane determinates FOV.
   double mPlaneX = .0;
   double mPlaneY = .66;
};

#endif // PLAYER_HPP
