#include "LuaHelper.hpp"

void LuaHelper::InitInstance(const std::string& table, const std::string& file, lua_CFunction func)
{
    lua_State* const L = luaL_newstate();
    if (!L) {
        throw "Unable to create a new Lua state.";
    }

    lua_register(L, table.c_str(), func);

    if (luaL_loadfile(L, file.c_str()) != LUA_OK) {
        throw "Unable to load Lua script.";
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        throw "Failed to execute Lua script.";
    }

    lua_close(L);
}

void LuaHelper::GetXYValue(lua_State* const L, const std::string& table, double& x, double& y)
{
    lua_getfield(L, 1, table.c_str());
    lua_getfield(L, -1, "x");
    lua_getfield(L, -2, "y");
    x = luaL_checknumber(L, -2);
    y = luaL_checknumber(L, -1);
    lua_pop(L, 3);
}

void LuaHelper::GetXYValue(lua_State* const L, const std::string& table, int& x, int& y)
{
    lua_getfield(L, 1, table.c_str());
    lua_getfield(L, -1, "x");
    lua_getfield(L, -2, "y");
    x = static_cast<int>(luaL_checkinteger(L, -2));
    y = static_cast<int>(luaL_checkinteger(L, -1));
    lua_pop(L, 3);
}
