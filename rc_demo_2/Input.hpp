#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL_keysym.h>

class Input
{
public:
   Input(SDLKey up_keycode,
         SDLKey down_keycode,
         SDLKey left_keycode,
         SDLKey right_keycode,
         SDLKey action1_keycode,
         SDLKey action2_keycode);
   ~Input();

   void Press(SDLKey key);
   void Release(SDLKey key);

   bool TestUp() const;
   bool TestDown() const;
   bool TestLeft() const;
   bool TestRight() const;
   bool TestAction1() const;
   bool TestAction2() const;

private:
   const int mUpKey;
   const int mDownKey;
   const int mLeftKey;
   const int mRightKey;
   const int mAction1Key;
   const int mAction2Key;

   bool mUpKeyPressed = false;
   bool mDownKeyPressed = false;
   bool mLeftKeyPressed = false;
   bool mRightKeyPressed = false;
   bool mAction1KeyPressed = false;
   bool mAction2KeyPressed = false;
};

#endif // INPUT_HPP
