#ifndef LUA_HELPER_HPP
#define LUA_HELPER_HPP

#include <string>

#include <lua.hpp>

class LuaHelper
{
public:
    static void InitInstance(const std::string& table, const std::string& file, lua_CFunction func);

    static void GetXYValue(lua_State* L, const std::string& table, double& x, double& y);
    static void GetXYValue(lua_State* L, const std::string& table, int& x, int& y);

private:
    LuaHelper() = delete;
    ~LuaHelper() = delete;
};

#endif // LUA_HELPER_HPP
