#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "SDL_console.h"

#include <functional>
#include <map>
#include <string>
#include <vector>

class Console;
class Renderer;

typedef std::function<void(Console&, const std::string&, const std::vector<std::string>&)> CmdCallbackWithArgs;

// Works only when using an SDL-based renderer.
class Console
{
public:
    Console(const std::string& font, Renderer& r);
    ~Console();

    bool ProcessInput(SDL_Event* event);
    void ToggleVisible();
    void Print(const std::string& msg);

    void RegisterCommand(const std::string& cmd, CmdCallbackWithArgs cb);

    void ExecuteCommand(const std::string& cmd);
    void ExecuteCommand(const std::string& cmd, const std::vector<std::string>& args);

private:
    static const int LINE_COUNT = 128;
    static const int ALPHA_LEVEL = 128;

    SDL_Surface* GetSdlSurface(const Renderer& r) const;

    ConsoleInformation* mSdlConsole = nullptr;
    std::map<std::string, CmdCallbackWithArgs> mCmdCallbacks;
};

#endif // CONSOLE_HPP
