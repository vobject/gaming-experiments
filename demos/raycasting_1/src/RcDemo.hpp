#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include "World.hpp"
#include "Input.hpp"
#include "Player.hpp"

#include <memory>

class Renderer;

class RcDemo
{
public:
    RcDemo();
    ~RcDemo();

   void Start();

private:
    void Mainloop();

    void Initialize();
    void ProcessInput();
    void UpdateScene(long app_time, long elapsed_time);
    void RenderScene();

    bool mQuitRequested = false;

    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<World> mWorld;
    std::unique_ptr<Input> mInput;
    std::unique_ptr<Player> mPlayer;
};

#endif // RCDEMO_HPP
