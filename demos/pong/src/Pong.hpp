#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include "Console.hpp"

#include <map>
#include <memory>
#include <vector>

class MainFrame;
class Input;
class Game;
class Renderer;

class Pong
{
public:
    Pong();
    ~Pong();

    void Start();

private:
    void Mainloop();

    void Initialize();
    void ProcessInput();
    void UpdateScene(int app_time, int elapsed_time);
    void RenderScene();

    std::vector<std::pair<std::string, CmdCallbackWithArgs>> InitConsoleCommands();

    bool mQuitRequested = false;

    std::shared_ptr<Renderer> mRenderer;
    std::shared_ptr<MainFrame> mMainFrame;
    std::map<int, std::shared_ptr<Input>> mInputs;
    std::unique_ptr<Game> mGame;
    std::unique_ptr<Console> mConsole;
};

#endif // RCDEMO_HPP
