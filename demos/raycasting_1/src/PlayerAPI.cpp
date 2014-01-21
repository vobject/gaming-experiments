#include "Player.hpp"
#include "LuaInterpreter.hpp"
#include "MainLoop.hpp"
#include "World.hpp"

#include <lua.hpp>

namespace {

std::vector<luaL_Reg> initialize_api();
std::vector<luaL_Reg> api = initialize_api();

} // unnamed namespace

std::string Player::GetModuleName()
{
    return "player";
}

std::vector<luaL_Reg> Player::GetAPI()
{
    return api;
}

namespace {

std::vector<luaL_Reg> initialize_api()
{
    std::vector<luaL_Reg> api;

    api.push_back({ "set_rays", [](lua_State* L) {
        if (!lua_isnumber(L, -1)) {
            throw "set_rays() specify an integer.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const int ray_count = static_cast<int>(lua_tointeger(L, -1));
        app.GetWorld().GetPlayer().SetHorizontalRayCount(ray_count);
        return 0;
    } });

    api.push_back({ "set_position", [](lua_State* L) {
        if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2)) {
            throw "set_position() must specify x and y coordinates.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const double y = lua_tonumber(L, -1);
        const double x = lua_tonumber(L, -2);
        app.GetWorld().GetPlayer().SetPosition(x, y);
        return 0;
    } });

    api.push_back({ "set_direction", [](lua_State* L) {
        if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2)) {
            throw "set_direction() must specify x and y.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const double y = lua_tonumber(L, -1);
        const double x = lua_tonumber(L, -2);
        app.GetWorld().GetPlayer().SetDirection(x, y);
        return 0;
    } });

    api.push_back({ "set_plane", [](lua_State* L) {
        if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2)) {
            throw "set_plane() must specify x and y.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const double y = lua_tonumber(L, -1);
        const double x = lua_tonumber(L, -2);
        app.GetWorld().GetPlayer().SetPlane(x, y);
        return 0;
    } });

    api.push_back({ nullptr, nullptr });
    return api;
}

} // unnamed namespace
