#include "Input.hpp"

Input::Input(
   const SDLKey up_keycode,
   const SDLKey down_keycode
)
   : mUpKey(up_keycode)
   , mDownKey(down_keycode)
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
}

bool Input::TestUp() const
{
   return mUpKeyPressed;
}

bool Input::TestDown() const
{
   return mDownKeyPressed;
}
