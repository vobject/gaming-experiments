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
   Renderer(int res_x, int res_y, const std::string& app_name,
                                  const std::string& renderer_name);
   virtual ~Renderer();

   Renderer(Renderer&) = delete;
   Renderer& operator=(Renderer&) = delete;

    int GetResX() const;
    int GetResY() const;
    std::string GetAppName() const;

    const std::string& GetName() const;
    const int& getFPS() const;

    virtual void Startup() = 0;
    virtual void Shutdown() = 0;

    virtual void PreRender() = 0;
    virtual void PostRender() = 0;
    virtual void DoRender(const Level& level, const Player& player) = 0;

protected:
    const int mResX;
    const int mResY;
    SDL_Window* mScreen = nullptr;

private:
    static Uint32 FrameTimerCB(Uint32 interval, void* param);

    const std::string mAppName;
    const std::string mRendererName;
    std::string mCaption;
    SDL_TimerID mFrameTimer = 0;
    int mFrameCount = 0;
    bool mRefreshCaption = false;
    int mFPS = 0;

};

#endif // RENDERER_HPP


