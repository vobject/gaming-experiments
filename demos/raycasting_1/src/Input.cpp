#include "Input.hpp"

#include <algorithm>

// the one and only supported type of controller (atm)
const std::string Input::XBOX360_CONTROLLER_NAME("X360 Controller");

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
    if (0 > SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC)) {
        throw "Cannot init SDL game controller and haptic subsystem.";
    }

    for (auto i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (!SDL_IsGameController(i)) {
            continue;
        }

        SDL_GameController* controller = SDL_GameControllerOpen(i);
        if (!controller) {
            continue;
        }

        const std::string name = SDL_GameControllerName(controller);
        if (name == XBOX360_CONTROLLER_NAME) {
            // only XBOX360 controller are supported
            mController = controller;

            // xbox360 controllers are haptic
            auto* const js = SDL_GameControllerGetJoystick(mController);
            if (SDL_JoystickIsHaptic(js)) {
                mControllerHaptic = SDL_HapticOpenFromJoystick(js);
            }
            break;
        }

        SDL_GameControllerClose(controller);
    }
}

Input::~Input()
{
    if (mControllerHaptic) {
        SDL_HapticClose(mControllerHaptic);
        mControllerHaptic = nullptr;
    }

    if (mController) {
        SDL_GameControllerClose(mController);
        mController = nullptr;
    }
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
    mMouseLookLeft = (mouse_rel_x < 0) ? std::abs(mouse_rel_x) : 0;
    mMouseLookRight = (mouse_rel_x > 0) ? std::abs(mouse_rel_x) : 0;
    mMouseLookUp = (mouse_rel_y < 0) ? std::abs(mouse_rel_y) : 0;
    mMouseLookDown = (mouse_rel_y > 0) ? std::abs(mouse_rel_y) : 0;

    // update controller input
    if (mController && SDL_GameControllerGetAttached(mController))
    {
        const auto trigger_positive = std::numeric_limits<Sint16>::max() / 3;
        const auto trigger_negative = std::numeric_limits<Sint16>::min() / 3;

        // controller movement axis
        const auto move_x = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTX);
        const auto move_y = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTY);
        mControllerMoveUp = move_y < trigger_negative;
        mControllerMoveDown = move_y > trigger_positive;
        mControllerMoveLeft = move_x < trigger_negative;
        mControllerMoveRight = move_x > trigger_positive;

        // controller look axis
        const auto look_x = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_RIGHTX);
        const auto look_y = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_RIGHTY);
        mControllerLookUp = (look_y < trigger_negative) ? 1 : 0;
        mControllerLookDown = (look_y > trigger_positive) ? 1 : 0;
        mControllerLookLeft = (look_x < trigger_negative) ? 1 : 0;
        mControllerLookRight = (look_x > trigger_positive) ? 1 : 0;
    }
}

bool Input::TestUp() const
{
    return mUpKeyPressed ? mUpKeyPressed : mControllerMoveUp;
}

bool Input::TestDown() const
{
    return mDownKeyPressed ? mDownKeyPressed : mControllerMoveDown;
}

bool Input::TestLeft() const
{
    return mLeftKeyPressed ? mLeftKeyPressed : mControllerMoveLeft;
}

bool Input::TestRight() const
{
    return mRightKeyPressed ? mRightKeyPressed : mControllerMoveRight;
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
    return mMouseLookUp ? mMouseLookUp : mControllerLookUp;
}

int Input::TestMotionDown() const
{
    return mMouseLookDown ? mMouseLookDown : mControllerLookDown;
}

int Input::TestMotionLeft() const
{
    return mMouseLookLeft ? mMouseLookLeft : mControllerLookLeft;
}

int Input::TestMotionRight() const
{
    return mMouseLookRight ? mMouseLookRight : mControllerLookRight;
}

void Input::Rumble()
{
    SDL_HapticRumbleStop(mControllerHaptic);

    SDL_HapticRumbleInit(mControllerHaptic);
    SDL_HapticRumblePlay(mControllerHaptic, 0.25, 250);
}
