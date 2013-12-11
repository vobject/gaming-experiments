#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>

#include <string>
#include <sstream>

class Level;
class Player;

class Renderer
{
public:
    Renderer(const std::string& app_name) : mAppName(app_name), mCaption(app_name)
    {
        if (0 > SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
            throw "Cannot init SDL video and timer subsystem.";
        }
        atexit(SDL_Quit);

        mFrameTimer = SDL_AddTimer(1000, FrameTimerCB, this);
        if (!mFrameTimer) {
            throw "SDL_AddTimer() failed.";
        }
    }

    virtual ~Renderer()
    {
        SDL_RemoveTimer(mFrameTimer);
    }

    virtual void PreRender() = 0;
    virtual void PostRender() = 0;
    virtual void DoRender(const Level& level, const Player& player) = 0;

protected:
    const std::string mAppName;
    std::string mCaption;
    int mFrameCount = 0;
    bool mRefreshCaption = false;

private:
    static Uint32 FrameTimerCB(Uint32 interval, void* param)
    {
        Renderer* obj = static_cast<Renderer*>(param);

        obj->mFPS = obj->mFrameCount;
        obj->mFrameCount = 0;

        std::ostringstream caption;
        caption << obj->mAppName;
#ifdef NDEBUG
        caption << " (Release) - ";
#else
        caption << " (Debug) - ";
#endif
        caption << "render:Software - ";
        caption << obj->mFPS << " FPS";

        std::string captionStr(caption.str());
        obj->mCaption.swap(captionStr);
        obj->mRefreshCaption = true;
        return interval;
    }

    SDL_TimerID mFrameTimer = 0;
    int mFPS = 0;
};

#endif // RENDERER_HPP
