#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL.h>

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

    Input(Input&) = delete;
    Input& operator=(Input&) = delete;

    void Update();

//    void MouseMotion(int xrel, int yrel);

    bool TestUp() const;
    bool TestDown() const;
    bool TestLeft() const;
    bool TestRight() const;
    bool TestAction1() const;
    bool TestAction2() const;

    int TestMotionLeft() const;
    int TestMotionRight() const;
    int TestMotionUp() const;
    int TestMotionDown() const;

private:
    const SDL_Scancode mUpKey;
    const SDL_Scancode mDownKey;
    const SDL_Scancode mLeftKey;
    const SDL_Scancode mRightKey;
    const SDL_Scancode mAction1Key;
    const SDL_Scancode mAction2Key;

    bool mUpKeyPressed = false;
    bool mDownKeyPressed = false;
    bool mLeftKeyPressed = false;
    bool mRightKeyPressed = false;
    bool mAction1KeyPressed = false;
    bool mAction2KeyPressed = false;

    int mMotionLeft = 0;
    int mMotionRight = 0;
    int mMotionUp = 0;
    int mMotionDown = 0;
};

#endif // INPUT_HPP
