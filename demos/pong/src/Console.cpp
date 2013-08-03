#include "Console.hpp"
#include "Renderer.hpp"

#include <iostream>

// ugly hack! is there any nicer way to access Consoles members?
static std::map<ConsoleInformation*, Console*> gConsoleMap;

static void ConsoleCmdHandler(ConsoleInformation* console, char* args)
{
    gConsoleMap[console]->ExecuteCommand(args);
}

static char* ConsoleTabHandler(char* command)
{
    (void) command;
    return nullptr;
}

Console::Console(const std::string& font, Renderer& r)
{
    const auto screen = GetSdlSurface(r);

    mSdlConsole = CON_Init(font.c_str(), screen, LINE_COUNT,
                             { 0, 0,
                               static_cast<Uint16>(screen->w),
                               static_cast<Uint16>(screen->h * .75f) });
    if (!mSdlConsole) {
        throw "Unable to create console.";
    }

    CON_Alpha(mSdlConsole, ALPHA_LEVEL);
    CON_Topmost(mSdlConsole);

    CON_SetPrompt(mSdlConsole, "> ");
    CON_SetHideKey(mSdlConsole, SDLK_BACKQUOTE);

    CON_SetExecuteFunction(mSdlConsole, ConsoleCmdHandler);
    CON_SetTabCompletion(mSdlConsole, ConsoleTabHandler);

    r.RegisterPostRenderHook([&](void*){ CON_DrawConsole(mSdlConsole); });

//    gConsole = this;
    gConsoleMap[mSdlConsole] = this;
}

Console::~Console()
{
    gConsoleMap[mSdlConsole] = nullptr;
    CON_Destroy(mSdlConsole);
}

bool Console::ProcessInput(SDL_Event* event)
{
    return CON_Events(event);
}

void Console::ToggleVisible()
{
    if (CON_isVisible(mSdlConsole)){
        CON_Hide(mSdlConsole);
    } else {
        CON_Show(mSdlConsole);
    }
}

void Console::Print(const std::string& msg)
{
    CON_Out(mSdlConsole, msg.c_str());
}

void Console::RegisterCommand(const std::string& cmd, CmdCallback cb)
{
    mCmdCallbacks[cmd] = cb;
}

void Console::ExecuteCommand(const std::string& cmd)
{
    const auto it = mCmdCallbacks.find(cmd);
    if (it == mCmdCallbacks.end()) {
        CON_Out(mSdlConsole, (std::string("Unknown command: \"") + cmd + "\"").c_str());
        return;
    }
    it->second(*this, cmd);
}

SDL_Surface *Console::GetSdlSurface(const Renderer& r) const
{
    // TODO: Check if the renderer actually DOES return a SDL_Surface.
    return static_cast<SDL_Surface*>(r.GetUnderlying());
}
