#include "World.hpp"
#include "LuaInterpreter.hpp"
#include "MainLoop.hpp"

#include <lua.hpp>

namespace {

std::vector<luaL_Reg> initialize_api();
std::vector<luaL_Reg> api = initialize_api();

} // unnamed namespace

std::string World::GetModuleName()
{
    return "world";
}

std::vector<luaL_Reg> World::GetAPI()
{
    return api;
}

namespace {

std::vector<luaL_Reg> initialize_api()
{
    std::vector<luaL_Reg> api;

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

    api.push_back({ nullptr, nullptr });
    return api;
}

} // unnamed namespace
