#include "Input.hpp"

Input::Input(
    const SDL_Keycode up_keycode,
    const SDL_Keycode down_keycode,
    const SDL_Keycode left_keycode,
    const SDL_Keycode right_keycode,
    const SDL_Keycode action1_keycode,
    const SDL_Keycode action2_keycode
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

void Input::Press(const SDL_Keycode key)
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

void Input::Release(const SDL_Keycode key)
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
