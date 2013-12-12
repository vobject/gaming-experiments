#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include <memory>

class Level;
class Input;
class Player;
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
    void UpdateScene(int app_time, int elapsed_time);
    void RenderScene();

    bool mQuitRequested = false;

    std::shared_ptr<Renderer> mRenderer;
    std::shared_ptr<Level> mLevel;
    std::shared_ptr<Input> mInput;
    std::shared_ptr<Player> mPlayer;
};

#endif // RCDEMO_HPP
