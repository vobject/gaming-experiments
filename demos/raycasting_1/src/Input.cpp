#include "Input.hpp"

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

void Input::Press(const SDL_Scancode key)
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

void Input::Release(const SDL_Scancode key)
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

void Input::MouseMotion(const int xrel, const int yrel)
{
    if (mOldMotionXRel == xrel) {
        mMotionLeft = 0;
        mMotionRight = 0;
    } else {
        mMotionLeft = xrel < 0;
        mMotionRight = xrel > 0;
        mOldMotionXRel = xrel;
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

bool Input::TestMotionLeft() const
{
    return mMotionLeft;
}

bool Input::TestMotionRight() const
{
    return mMotionRight;
}
