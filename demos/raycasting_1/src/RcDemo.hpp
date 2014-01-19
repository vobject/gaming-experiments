#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include <string>
#include <memory>

class Renderer;
class World;
class LuaInterpreter;

class RcDemo
{
public:
    static RcDemo& Instance();
    ~RcDemo();

   void Start();

   void SetAppName(const std::string& name);
   void SetResolution(int width, int height);

   std::string GetAppName() const;
   int GetWindowWidth() const;
   int GetWindowHeight() const;
   const Renderer& GetRenderer() const;
   const World& GetWorld() const;

   void RegisterLua(LuaInterpreter& lua);

private:
    RcDemo();
    RcDemo(const RcDemo&) = delete;
    RcDemo& operator=(const RcDemo&) = delete;

    void Mainloop();

    void Initialize();
    void ProcessInput();
    void UpdateScene(long app_time, long elapsed_time);
    void RenderScene();

    bool mQuitRequested = false;

    int mResX = 640;
    int mResY = 480;
    std::string mAppName = "RcDemo";
    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<World> mWorld;
};

#endif // RCDEMO_HPP
