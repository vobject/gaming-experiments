#include "MainLoop.hpp"
#include "LuaInterpreter.hpp"

#include <lua.hpp>

namespace {

std::vector<luaL_Reg> initialize_api();
std::vector<luaL_Reg> api = initialize_api();

} // unnamed namespace

std::string MainLoop::GetModuleName()
{
    return "mainloop";
}

std::vector<luaL_Reg> MainLoop::GetAPI()
{
    return api;
}

namespace {

std::vector<luaL_Reg> initialize_api()
{
    std::vector<luaL_Reg> api;

    api.push_back({ "run", [](lua_State* L) {
        LuaInterpreter::GetMainLoop(L).Run();
        return 0;
    } });

    api.push_back({ "request_quit", [](lua_State* L) {
        LuaInterpreter::GetMainLoop(L).RequestQuit();
        return 0;
    } });

    api.push_back({ "set_update_time", [](lua_State* L) {
        if (!lua_isnumber(L, -1)) {
            throw "set_update_time() must return an integer.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const int update_time = static_cast<int>(lua_tointeger(L, -1));
        app.SetUpdateTime(update_time);
        return 0;
    } });

    api.push_back({ "set_renderer", [](lua_State* L) {
        if (!lua_isstring(L, -1)) {
            throw "set_renderer() must specify a string.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const char* const name = lua_tostring(L, -1);
        app.SetRenderer(name);
        return 0;
    } });

    api.push_back({ "set_world", [](lua_State* L) {
        if (!lua_isstring(L, -1)) {
            throw "set_world() must specify a string.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const char* const name = lua_tostring(L, -1);
        app.SetWorld(name);
        return 0;
    } });

    api.push_back({ nullptr, nullptr });
    return api;
}

} // unnamed namespace
