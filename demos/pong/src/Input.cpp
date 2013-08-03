#include "Input.hpp"

Input::Input(
    const int id,
    const SDLKey up_keycode,
    const SDLKey down_keycode
)
    : mId(id)
    , mUpKey(up_keycode)
    , mDownKey(down_keycode)
{

}

Input::~Input()
{

}

int Input::GetId() const
{
    return mId;
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
