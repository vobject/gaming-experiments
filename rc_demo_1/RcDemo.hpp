#ifndef RCDEMO_HPP
#define RCDEMO_HPP

#include <memory>

class MainFrame;
class Level;
class Input;
class Player;
class Render;

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

   std::shared_ptr<MainFrame> mMainFrame;
   std::shared_ptr<Level> mLevel;
   std::shared_ptr<Input> mInput;
   std::shared_ptr<Player> mPlayer;
   std::shared_ptr<Render> mRenderer;
//   std::shared_ptr<Logic> mLogic;
};

#endif // RCDEMO_HPP
