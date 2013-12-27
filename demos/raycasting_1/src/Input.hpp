#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL.h>

#include <string>

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

    void Rumble();

private:
    const SDL_Scancode mUpKey;
    const SDL_Scancode mDownKey;
    const SDL_Scancode mLeftKey;
    const SDL_Scancode mRightKey;
    const SDL_Scancode mAction1Key;
    const SDL_Scancode mAction2Key;

    static const std::string XBOX360_CONTROLLER_NAME;
    SDL_GameController* mController = nullptr;
    SDL_Haptic* mControllerHaptic = nullptr;

    bool mUpKeyPressed = false;
    bool mDownKeyPressed = false;
    bool mLeftKeyPressed = false;
    bool mRightKeyPressed = false;
    bool mAction1KeyPressed = false;
    bool mAction2KeyPressed = false;

    int mMouseLookLeft = 0;
    int mMouseLookRight = 0;
    int mMouseLookUp = 0;
    int mMouseLookDown = 0;

    bool mControllerMoveUp = false;
    bool mControllerMoveDown = false;
    bool mControllerMoveLeft = false;
    bool mControllerMoveRight = false;
    int mControllerLookUp = 0;
    int mControllerLookDown = 0;
    int mControllerLookLeft = 0;
    int mControllerLookRight = 0;
};

#endif // INPUT_HPP
