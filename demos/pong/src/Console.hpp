#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "SDL_console.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

class Console;
class Renderer;

typedef std::function<void(Console& con, const std::string& val)> CVarCallback;

// TODO: Make this a singleton!
class Console
{
public:
   Console(const std::string& font, Renderer& r);
   ~Console();

   bool ProcessInput(SDL_Event* event);
   void ToggleVisible();
   void Print(const std::string& msg);

   void RegisterCVar(const std::string& var, CVarCallback cb);

private:
   static const int LINE_COUNT = 128;
   static const int ALPHA_LEVEL = 128;

   SDL_Surface* GetSdlSurface(const Renderer& r) const;

   ConsoleInformation* mSdlConsole = nullptr;
   std::map<std::string, CVarCallback> mCVarCallbacks;
};

#endif // CONSOLE_HPP
