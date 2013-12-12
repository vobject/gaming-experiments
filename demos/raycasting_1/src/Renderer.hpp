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
    Renderer(int res_x, int res_y, const std::string& app_name);
    virtual ~Renderer();

    int GetResX() const;
    int GetResY() const;
    std::string GetAppName() const;

    virtual void Startup() = 0;
    virtual void Shutdown() = 0;

    virtual void PreRender() = 0;
    virtual void PostRender() = 0;
    virtual void DoRender(const Level& level, const Player& player) = 0;

    virtual const std::string& GetName() const = 0;
    virtual const int& getFPS() const;

protected:
    const int mResX;
    const int mResY;
    const std::string mAppName;
    SDL_Window* mScreen = nullptr;

private:
    static Uint32 FrameTimerCB(Uint32 interval, void* param);

    std::string mCaption;
    SDL_TimerID mFrameTimer = 0;
    int mFrameCount = 0;
    bool mRefreshCaption = false;
    int mFPS = 0;

};

#endif // RENDERER_HPP


