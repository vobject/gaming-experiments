#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL_keycode.h>

class Input
{
public:
    Input(SDL_Keycode up_keycode,
          SDL_Keycode down_keycode,
          SDL_Keycode left_keycode,
          SDL_Keycode right_keycode,
          SDL_Keycode action1_keycode,
          SDL_Keycode action2_keycode);
    ~Input();

    void Press(SDL_Keycode key);
    void Release(SDL_Keycode key);

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
