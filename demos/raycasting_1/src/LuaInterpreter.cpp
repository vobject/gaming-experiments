#include "LuaInterpreter.hpp"
#include "MainLoop.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "render/Renderer.hpp"
#include "Utils.hpp"

#include <map>
#include <memory>
#include <vector>
#include <iostream>

namespace {

std::map<lua_State* const, std::unique_ptr<LuaInterpreter>> interpreters;

void initialize_api();
void register_api(lua_State* L);
bool api_initialized = false;

} // unnamed namespace

LuaInterpreter& LuaInterpreter::Create(MainLoop& mainloop)
{
    if (!api_initialized) {
        initialize_api();
    }

    lua_State* const L = luaL_newstate();
    if (!L) {
        throw "Unable to create a new Lua state.";
    }
    luaL_openlibs(L);
    register_api(L);

    interpreters.insert({ L, Utils::make_unique<LuaInterpreter>(L, mainloop) });
    return LuaInterpreter::GetInterpreter(L);
}

LuaInterpreter& LuaInterpreter::GetInterpreter(lua_State* L)
{
    const auto iter = interpreters.find(L);
    if (iter == interpreters.end()) {
        throw "Trying to access non-existing Lua interpreter.";
    }
    return *iter->second;
}

MainLoop& LuaInterpreter::GetMainLoop(lua_State* L)
{
    return GetInterpreter(L).GetMainLoop();
}

void LuaInterpreter::RunScript(lua_State* L, const std::string& file)
{
    luaL_dofile(L, file.c_str());
}

void LuaInterpreter::DumpStack(lua_State* L)
{
    int top = lua_gettop(L);

    for (int i = 1; i <= top; i++)
    {
        int t = lua_type(L, i);

        switch (t)
        {
        case LUA_TSTRING:
            std::cout << lua_tostring(L, i);
            break;
        case LUA_TBOOLEAN:
            std::cout << (lua_toboolean(L, i) != 0);
            break;
        case LUA_TNUMBER:
            std::cout << lua_tonumber(L, i);
            break;
        default:
            std::cout << lua_typename(L, t);
            break;
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

LuaInterpreter::LuaInterpreter(lua_State* const L, MainLoop& mainloop)
    : mL(L)
    , mMainLoop(mainloop)
{

}

LuaInterpreter::~LuaInterpreter()
{

}

MainLoop& LuaInterpreter::GetMainLoop() const
{
    return mMainLoop;
}

void LuaInterpreter::RunScript(const std::string& file)
{
    LuaInterpreter::RunScript(mL, file);
}

void LuaInterpreter::DumpStack()
{
    LuaInterpreter::DumpStack(mL);
}

namespace {

std::vector<luaL_Reg> demo_api;
std::vector<luaL_Reg> world_api;
std::vector<luaL_Reg> player_api;
std::vector<luaL_Reg> renderer_api;

void initialize_demo_api(std::vector<luaL_Reg>& api);
void initialize_world_api(std::vector<luaL_Reg>& api);
void initialize_player_api(std::vector<luaL_Reg>& api);
void initialize_renderer_api(std::vector<luaL_Reg>& api);

void initialize_api()
{
    initialize_demo_api(demo_api);
    initialize_world_api(world_api);
    initialize_player_api(player_api);
    initialize_renderer_api(renderer_api);

    demo_api.push_back({ nullptr, nullptr });
    world_api.push_back({ nullptr, nullptr });
    player_api.push_back({ nullptr, nullptr });
    renderer_api.push_back({ nullptr, nullptr });

    api_initialized = true;
}

void initialize_demo_api(std::vector<luaL_Reg>& api)
{
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
}

void initialize_world_api(std::vector<luaL_Reg>& api)
{
    api.push_back({ "set_level_size", [](lua_State* L) {
        if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2)) {
            throw "set_level_size() must specify level dimensions.";
        }
        auto& app = LuaInterpreter::GetMainLoop(L);

        const int height = static_cast<int>(lua_tointeger(L, -1));
        const int width = static_cast<int>(lua_tointeger(L, -2));
        app.GetWorld().SetLevelSize(width, height);
        return 0;
    } });

    api.push_back({ "set_level_layout", [](lua_State* L) {
        std::vector<uint32_t> level_data;

        // http://stackoverflow.com/questions/6137684/iterate-through-lua-table

        // Push another reference to the table on top of the stack (so we know
        // where it is, and this function can work for negative, positive and
        // pseudo indices
        lua_pushvalue(L, -1);
        // stack now contains: -1 => table
        lua_pushnil(L);
        // stack now contains: -1 => nil; -2 => table
        while (lua_next(L, -2))
        {
            // stack now contains: -1 => value; -2 => key; -3 => table
            // copy the key so that lua_tostring does not modify the original
            lua_pushvalue(L, -2);
            // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
            const uint32_t key = static_cast<uint32_t>(lua_tointeger(L, -1));
            const uint32_t value = static_cast<uint32_t>(lua_tointeger(L, -2));

            level_data.push_back(value);
            // pop value + copy of key, leaving original key
            lua_pop(L, 2);
            // stack now contains: -1 => key; -2 => table
        }
        // stack now contains: -1 => table (when lua_next returns 0 it pops the key
        // but does not push anything.)
        // Pop table
        lua_pop(L, 1);
        // Stack is now the same as it was on entry to this function

        auto& app = LuaInterpreter::GetMainLoop(L);
        app.GetWorld().SetLevelData(level_data.data());
        return 0;
    } });
}

void initialize_player_api(std::vector<luaL_Reg>& api)
{
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
}

void initialize_renderer_api(std::vector<luaL_Reg>& api)
{
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
}

void register_api(lua_State* const L, const char* const table, const luaL_Reg* api)
{
    lua_newtable(L);
    luaL_setfuncs(L, api, 0);
    lua_setglobal(L, table);
}

void register_api(lua_State* const L)
{
    register_api(L, "rcdemo", demo_api.data());
    register_api(L, "world", world_api.data());
    register_api(L, "player", player_api.data());
    register_api(L, "video", renderer_api.data());
}

} // unnamed namespace
