#include "LuaInterpreter.hpp"
#include "RcDemo.hpp"

LuaInterpreter::LuaInterpreter(RcDemo& demo)
{
    mState = luaL_newstate();
    if (!mState) {
        throw "Unable to create a new Lua state.";
    }
    luaL_openlibs(mState);

    demo.RegisterLua(*this);
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
