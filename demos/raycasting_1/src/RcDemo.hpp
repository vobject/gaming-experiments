#ifndef RCDEMO_HPP
#define RCDEMO_HPP


#include <memory>

class Renderer;
class World;

class RcDemo
{
public:
    RcDemo();
    ~RcDemo();

   void Start();

   const Renderer& GetRenderer() const;
   const World& GetWorld() const;

private:
    void Mainloop();

    void Initialize();
    void ProcessInput();
    void UpdateScene(long app_time, long elapsed_time);
    void RenderScene();

    bool mQuitRequested = false;

    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<World> mWorld;
};

#endif // RCDEMO_HPP
