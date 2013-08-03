#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL_keysym.h>

class Input
{
public:
   Input(int id, SDLKey up_keycode, SDLKey down_keycode);
   ~Input();

   int GetId() const;

   void Press(SDLKey key);
   void Release(SDLKey key);

   bool TestUp() const;
   bool TestDown() const;

private:
   const int mId;
   const int mUpKey;
   const int mDownKey;

   bool mUpKeyPressed = false;
   bool mDownKeyPressed = false;
};

#endif // INPUT_HPP
