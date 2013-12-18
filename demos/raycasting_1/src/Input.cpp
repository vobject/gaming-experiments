#include "Input.hpp"

#include <algorithm>

Input::Input(
    const SDL_Scancode up_keycode,
    const SDL_Scancode down_keycode,
    const SDL_Scancode left_keycode,
    const SDL_Scancode right_keycode,
    const SDL_Scancode action1_keycode,
    const SDL_Scancode action2_keycode
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

void Input::Update()
{
    SDL_PumpEvents();
    const Uint8* kbState = SDL_GetKeyboardState(nullptr);

    // update key press states
    mUpKeyPressed = kbState[mUpKey] != 0;
    mDownKeyPressed = kbState[mDownKey] != 0;
    mLeftKeyPressed = kbState[mLeftKey] != 0;
    mRightKeyPressed = kbState[mRightKey] != 0;
    mAction1KeyPressed = kbState[mAction1Key] != 0;
    mAction2KeyPressed = kbState[mAction2Key] != 0;

    // reset mouse motion states
    int mouse_rel_x;
    int mouse_rel_y;
    SDL_GetRelativeMouseState(&mouse_rel_x, &mouse_rel_y);

    mMotionLeft = (mouse_rel_x < 0) ? std::abs(mouse_rel_x) : 0;
    mMotionRight = (mouse_rel_x > 0) ? std::abs(mouse_rel_x) : 0;
    mMotionUp = (mouse_rel_y < 0) ? std::abs(mouse_rel_y) : 0;
    mMotionDown = (mouse_rel_y > 0) ? std::abs(mouse_rel_y) : 0;
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

int Input::TestMotionUp() const
{
    return mMotionUp;
}

int Input::TestMotionDown() const
{
    return mMotionDown;
}

int Input::TestMotionLeft() const
{
    return mMotionLeft;
}

int Input::TestMotionRight() const
{
    return mMotionRight;
}
