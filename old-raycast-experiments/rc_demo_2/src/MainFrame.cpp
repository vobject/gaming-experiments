#include "MainFrame.hpp"

#include <sstream>

MainFrame::MainFrame(const std::string title)
   : mTitle(title)
{
   SDL_WM_SetCaption(mTitle.c_str(), NULL);

#ifdef WIN32
   if (0 > SDL_Init(SDL_INIT_TIMER)) {
#else
   if (0 > SDL_Init(SDL_INIT_EVENTTHREAD | SDL_INIT_TIMER)) {
#endif
      throw "Cannot init SDL timer subsystem.";
   }
   mFrameTimer = SDL_AddTimer(mFrameTimerUpdateRate,
                              DefaultFrameTimerCallback,
                              this);
}

MainFrame::~MainFrame()
{
   SDL_RemoveTimer(mFrameTimer);
}

void MainFrame::UpdateDone()
{
   mUpdateCount++;
}

void MainFrame::FrameDone()
{
   mFrameCount++;
}

void MainFrame::SetRendererName(const std::string& name)
{
   mRenderer = name;
}

Uint32 MainFrame::DefaultFrameTimerCallback(const Uint32 interval, void* param)
{
   MainFrame* obj = static_cast<MainFrame*>(param);

   obj->mUPS = (obj->mUpdateCount / static_cast<float>(interval)) * obj->mFrameTimerUpdateRate;
   obj->mUpdateCount = 0;

   obj->mFPS = (obj->mFrameCount / static_cast<float>(interval)) * obj->mFrameTimerUpdateRate;
   obj->mFrameCount = 0;

   std::ostringstream caption;
   caption << obj->mTitle;
#ifdef NDEBUG
   caption << " (Release) - ";
#else
   caption << " (Debug) - ";
#endif
   caption << "render:" << obj->mRenderer << " - ";
   caption << obj->mUPS << " UPS";
   caption << " - ";
   caption << obj->mFPS << " FPS";

   SDL_WM_SetCaption(caption.str().c_str(), NULL);
   return interval;
}
