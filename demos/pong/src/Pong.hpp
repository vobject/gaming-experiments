#ifndef RCDEMO_HPP
#define RCDEMO_HPP

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

   bool mQuitRequested = false;

   std::shared_ptr<MainFrame> mMainFrame;
   std::vector<std::shared_ptr<Input>> mInputs;
   std::unique_ptr<Game> mGame;

   std::vector<std::shared_ptr<Renderer>> mRenderers;
   unsigned int mActiveRenderer = 0;
};

#endif // RCDEMO_HPP
