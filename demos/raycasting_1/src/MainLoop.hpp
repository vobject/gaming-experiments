#ifndef MAINLOOP_HPP
#define MAINLOOP_HPP

#include <SDL.h>

#include <string>
#include <memory>

class LuaInterpreter;
class Renderer;
class World;

class MainLoop
{
public:
    MainLoop();
    ~MainLoop();

    void Run();

    void SetUpdateTime(int ms);
    void SetRenderer(const std::string& name);
    void SetRenderer(SDL_Scancode code);
    void SetWorld(const std::string& name);

    int GetUpdateTime() const;
    Renderer& GetRenderer() const;
    World& GetWorld() const;

private:
    MainLoop(const MainLoop&) = delete;
    MainLoop& operator=(const MainLoop&) = delete;

    void ProcessInput();
    void UpdateScene(long app_time, long elapsed_time);
    void RenderScene();

    void SelectRendererFromName(const std::string& name);
    void SelectRendererFromScanCode(SDL_Scancode scancode);

    bool mQuitRequested = false;

    int mUpdateDeltaTime = 2; // update game state every 2ms by default

    LuaInterpreter& mLua;
    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<World> mWorld;
};

#endif // MAINLOOP_HPP
