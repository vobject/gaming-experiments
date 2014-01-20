#ifndef LUA_INTERPRETER_HPP
#define LUA_INTERPRETER_HPP

#include <string>

#include <lua.hpp>

class RcDemo;

class LuaInterpreter
{
public:
    LuaInterpreter();
    ~LuaInterpreter();

    LuaInterpreter(LuaInterpreter&) = delete;
    LuaInterpreter& operator=(LuaInterpreter&) = delete;

    void RegisterFunctions(const std::string& table, const luaL_Reg funcs[]);
    void RegisterFunction(const std::string& name, const lua_CFunction func);
    void RunScript(const std::string& file);

    void DumpStack() const;

private:
    lua_State* mState = nullptr;
};

#endif // LUA_INTERPRETER_HPP
