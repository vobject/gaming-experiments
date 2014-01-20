#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include <SDL.h>

#include <string>
#include <memory>

class LuaInterpreter;
class Renderer;
class World;

class RcDemo
{
public:
    RcDemo();
    ~RcDemo();

    void Start();

    void SetAppName(const std::string& name);
    void SetResolution(int width, int height);
    void SetUpdateTime(int ms);
    void SetRenderer(const std::string& name);
    void SetRenderer(SDL_Scancode code);

    std::string GetAppName() const;
    int GetWindowWidth() const;
    int GetWindowHeight() const;
    int GetUpdateTime() const;
    Renderer& GetRenderer() const;
    World& GetWorld() const;

private:
    RcDemo(const RcDemo&) = delete;
    RcDemo& operator=(const RcDemo&) = delete;

    void Mainloop();
    void ProcessInput();
    void UpdateScene(long app_time, long elapsed_time);
    void RenderScene();

    void SelectRendererFromName(const std::string& name);
    void SelectRendererFromScanCode(SDL_Scancode scancode);

    bool mQuitRequested = false;

    int mResX = 640;
    int mResY = 480;
    int mUpdateDeltaTime = 2; // update game state every 2ms by default
    std::string mAppName = "RcDemo";

    LuaInterpreter& mLua;
    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<World> mWorld;
};

#endif // RCDEMO_HPP
