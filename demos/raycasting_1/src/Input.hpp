#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL_scancode.h>

class Input
{
public:
    Input(SDL_Scancode up_keycode,
          SDL_Scancode down_keycode,
          SDL_Scancode left_keycode,
          SDL_Scancode right_keycode,
          SDL_Scancode action1_keycode,
          SDL_Scancode action2_keycode);
    ~Input();

    void Press(SDL_Scancode key);
    void Release(SDL_Scancode key);

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
