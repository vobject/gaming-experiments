#include "LuaInterpreter.hpp"
#include "RcDemo.hpp"

#include <iostream>

LuaInterpreter::LuaInterpreter()
{
    mState = luaL_newstate();
    if (!mState) {
        throw "Unable to create a new Lua state.";
    }
    luaL_openlibs(mState);
}

LuaInterpreter::~LuaInterpreter()
{
    if (mState) {
        lua_close(mState);
    }
}

void LuaInterpreter::RegisterFunctions(const std::string& table, const luaL_Reg funcs[])
{
    lua_newtable(mState);
    luaL_setfuncs(mState, funcs, 0);
    lua_setglobal(mState, table.c_str());
}

void LuaInterpreter::RegisterFunction(const std::string& name, const lua_CFunction func)
{
    lua_register(mState, name.c_str(), func);
}

void LuaInterpreter::RunScript(const std::string& file)
{
    luaL_dofile(mState, file.c_str());
}

void LuaInterpreter::DumpStack() const
{
    int top = lua_gettop(mState);

    for (int i = 1; i <= top; i++)
    {
        int t = lua_type(mState, i);

        switch (t)
        {
            case LUA_TSTRING:
                std::cout << lua_tostring(mState, i);
                break;
            case LUA_TBOOLEAN:
                std::cout << (bool)lua_toboolean(mState, i);
                break;
            case LUA_TNUMBER:
                std::cout << lua_tonumber(mState, i);
                break;
            default:
                std::cout << lua_typename(mState, t);
                break;
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}
