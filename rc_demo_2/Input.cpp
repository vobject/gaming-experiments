#include "Input.hpp"

Input::Input(
   const SDLKey up_keycode,
   const SDLKey down_keycode,
   const SDLKey left_keycode,
   const SDLKey right_keycode,
   const SDLKey action1_keycode,
   const SDLKey action2_keycode
)
   : mUpKey(up_keycode)
   , mDownKey(down_keycode)
   , mLeftKey(left_keycode)
   , mRightKey(right_keycode)
   , mAction1Key(action1_keycode)
   , mAction2Key(action2_keycode)
{

}

Input::~Input()
{

}

void Input::Press(const SDLKey key)
{
   if (key == mUpKey)
   {
      mUpKeyPressed = true;
   }
   else if (key == mDownKey)
   {
      mDownKeyPressed = true;
   }
   else if (key == mLeftKey)
   {
      mLeftKeyPressed = true;
   }
   else if (key == mRightKey)
   {
      mRightKeyPressed = true;
   }
   else if (key == mAction1Key)
   {
      mAction1KeyPressed = true;
   }
   else if (key == mAction2Key)
   {
      mAction2KeyPressed = true;
   }
}

void Input::Release(const SDLKey key)
{
   if (key == mUpKey)
   {
      mUpKeyPressed = false;
   }
   else if (key == mDownKey)
   {
      mDownKeyPressed = false;
   }
   else if (key == mLeftKey)
   {
      mLeftKeyPressed = false;
   }
   else if (key == mRightKey)
   {
      mRightKeyPressed = false;
   }
   else if (key == mAction1Key)
   {
      mAction1KeyPressed = false;
   }
   else if (key == mAction2Key)
   {
      mAction2KeyPressed = false;
   }
}

bool Input::TestUp() const
{
   return mUpKeyPressed;
}

bool Input::TestDown() const
{
   return mDownKeyPressed;
}

bool Input::TestLeft() const
{
   return mLeftKeyPressed;
}

bool Input::TestRight() const
{
   return mRightKeyPressed;
}

bool Input::TestAction1() const
{
   return mAction1KeyPressed;
}

bool Input::TestAction2() const
{
   return mAction2KeyPressed;
}
