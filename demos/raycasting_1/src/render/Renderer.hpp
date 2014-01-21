#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>

#include <string>
#include <vector>

class World;
struct luaL_Reg;

class Renderer
{
public:
    Renderer(const std::string& renderer_name);
    virtual ~Renderer();

   Renderer(Renderer&) = delete;
    Renderer& operator=(Renderer&) = delete;

    std::string GetAppName() const;
    int GetResX() const;
    int GetResY() const;

    const std::string& GetName() const;
    const int& getFPS() const;

    void SetAppName(const std::string& name);
    void SetResolution(int width, int height);
    void ShowMinimap(bool show);

    virtual void Startup() = 0;
    virtual void Shutdown() = 0;

    virtual void PreRender() = 0;
    virtual void PostRender() = 0;
    virtual void DoRender(const World& world) = 0;

    static std::string GetModuleName();
    static std::vector<luaL_Reg> GetAPI();

protected:
    int mResX = 320;
    int mResY = 240;
    SDL_Window* mScreen = nullptr;
    bool mShowMinimap = false;

private:
    static Uint32 FrameTimerCB(Uint32 interval, void* param);

    const std::string mRendererName;
    std::string mAppName;
    std::string mCaption;
    SDL_TimerID mFrameTimer = 0;
    int mFrameCount = 0;
    bool mRefreshCaption = false;
    int mFPS = 0;
};

#endif // RENDERER_HPP


