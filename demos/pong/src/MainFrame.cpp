#include "MainFrame.hpp"

MainFrame::MainFrame(const std::string appname)
   : mAppName(appname)
   , mCaption(mAppName)
{
   SDL_WM_SetCaption(mCaption.c_str(), NULL);

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

   SDL_WM_SetCaption(mCaption.c_str(), nullptr);
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

   std::stringstream caption;
   caption << obj->mAppName;
#ifdef NDEBUG
   caption << " (Release) - ";
#else
   caption << " (Debug) - ";
#endif
   caption << "render:" << obj->mRenderer << " - ";
   caption << obj->mUPS << " UPS";
   caption << " - ";
   caption << obj->mFPS << " FPS";

   std::string captionStr(caption.str());
   obj->mCaption.swap(captionStr);
   return interval;
}
