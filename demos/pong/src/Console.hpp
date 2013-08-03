#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "SDL_console.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

class Console;
class Renderer;

//template<typename... Args>
//class Callback {
//    Callback(Console& con, const std::string& val) {

//    }
//};

typedef std::function<void(Console&, const std::string&)> CmdCallback;

//template<typename ...Args> using CVarCallback2 =
//std::function<void(Console&, const std::string&, Args&&...)>;

//static void regvar(CVarCallback&& s) {

//}

class Console
{
public:
    Console(const std::string& font, Renderer& r);
    ~Console();

    bool ProcessInput(SDL_Event* event);
    void ToggleVisible();
    void Print(const std::string& msg);

    void RegisterCommand(const std::string& cmd, CmdCallback cb);
    void ExecuteCommand(const std::string& cmd);

//    template<typename... Args>
//    void RegisterCVar2(const std::string& var, Args&&... args) {
////        regvar(std::forward<Args>(args)...);
//    }

//    template<typename ...Args>
//    void RegisterCVar3(const std::string& var, Args&&... args) {
////        regvar(std::forward<Args>(args)...);
//        std::function<void(Console&, const std::string&)> f;
//        CVarCallback2<Args...> f2(*this, "", std::forward<Args>(args)...);
//    }

private:
    static const int LINE_COUNT = 128;
    static const int ALPHA_LEVEL = 128;

    SDL_Surface* GetSdlSurface(const Renderer& r) const;

    ConsoleInformation* mSdlConsole = nullptr;
    std::map<std::string, CmdCallback> mCmdCallbacks;
//    std::map<std::string, CVarCallback2> mCVarCallbacks2;
};

#endif // CONSOLE_HPP
