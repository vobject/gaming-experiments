#include "Renderer.hpp"
#include "../LuaInterpreter.hpp"
#include "../MainLoop.hpp"

#include <lua.hpp>

namespace {

std::vector<luaL_Reg> initialize_api();
std::vector<luaL_Reg> api = initialize_api();

} // unnamed namespace

std::string Renderer::GetModuleName()
{
    return "video";
}

std::vector<luaL_Reg> Renderer::GetAPI()
{
    return api;
}

namespace {

std::vector<luaL_Reg> initialize_api()
{
    std::vector<luaL_Reg> api;

    api.push_back({ "set_caption", [](lua_State* L) {
        if (!lua_isstring(L, -1)) {
            throw "set_caption() must specify a string.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const char* const name = lua_tostring(L, -1);
        app.GetRenderer().SetAppName(name);
        return 0;
    } });

    api.push_back({ "set_resolution", [](lua_State* L) {
        if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2)) {
            throw "set_resolution() must specify x and y dimensions.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const int height = static_cast<int>(lua_tointeger(L, -1));
        const int width = static_cast<int>(lua_tointeger(L, -2));
        app.GetRenderer().SetResolution(width, height);
        return 0;
    } });

    api.push_back({ "set_show_minimap", [](lua_State* L) {
        if (!lua_isboolean(L, -1)) {
            throw "show_minimap() must specifiy a boolean.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const bool show = lua_toboolean(L, -1) != 0;
        app.GetRenderer().ShowMinimap(show);
        return 0;
    } });

    api.push_back({ nullptr, nullptr });
    return api;
}

} // unnamed namespace
