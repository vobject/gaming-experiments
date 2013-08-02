#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include <memory>
#include <vector>

class MainFrame;
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

   void NextRenderer();

   bool mQuitRequested = false;

   std::shared_ptr<MainFrame> mMainFrame;
   std::shared_ptr<Level> mLevel;
   std::shared_ptr<Input> mInput;
   std::shared_ptr<Player> mPlayer;

   std::vector<std::shared_ptr<Renderer>> mRenderers;
   unsigned int mActiveRenderer = 0;
};

#endif // RCDEMO_HPP
