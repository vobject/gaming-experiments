#ifndef MAINLOOP_HPP
#define MAINLOOP_HPP

#include <SDL.h>

#include <string>
#include <memory>
#include <vector>

class LuaInterpreter;
class Renderer;
class World;
struct luaL_Reg;

class MainLoop
{
public:
    MainLoop(LuaInterpreter& lua);
    ~MainLoop();

    void Run();
    void RequestQuit();

    void SetUpdateTime(int ms);
    void SetRenderer(const std::string& name);
    void SetWorld(const std::string& name);

    int GetUpdateTime() const;
    Renderer& GetRenderer() const;
    World& GetWorld() const;

    static std::string GetModuleName();
    static std::vector<luaL_Reg> GetAPI();

private:
    MainLoop(const MainLoop&) = delete;
    MainLoop& operator=(const MainLoop&) = delete;

    void ProcessInput();
    void UpdateScene(long app_time, long elapsed_time);
    void RenderScene();

    void SetRenderer(SDL_Scancode code);
    void SelectRendererFromScanCode(SDL_Scancode scancode);
    void SelectRendererFromName(const std::string& name);

    void LuaOnStart();
    void LuaOnQuit();
    void LuaOnInput();
    void LuaOnUpdate();
    void LuaOnRender();

    bool mQuitRequested = false;
    int mUpdateDeltaTime = 2; // update game state every 2ms by default

    LuaInterpreter& mLua;
    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<World> mWorld;
};

#endif // MAINLOOP_HPP
