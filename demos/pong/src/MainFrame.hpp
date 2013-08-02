#ifndef MAIN_FRAME_HPP
#define MAIN_FRAME_HPP

#include <SDL.h>

#include <string>

class MainFrame
{
public:
   MainFrame(const std::string appname);
   ~MainFrame();

   void UpdateDone();
   void FrameDone();

   void SetRendererName(const std::string& name);

private:
   static Uint32 DefaultFrameTimerCallback(Uint32 interval, void* param);

   const std::string mAppName;
   std::string mCaption;
   std::string mRenderer = "noname";

   const int mFrameTimerUpdateRate = 1000; // 1 second
   SDL_TimerID mFrameTimer = nullptr;

   int mUpdateCount = 0; // Internal game state updates.
   int mFrameCount = 0; // Rendered frames.

   int mUPS = 0;
   int mFPS = 0;
};

#endif // MAIN_FRAME_HPP
