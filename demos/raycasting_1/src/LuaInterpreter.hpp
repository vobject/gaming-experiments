#ifndef LUA_INTERPRETER_HPP
#define LUA_INTERPRETER_HPP

#include <string>

#include <lua.hpp>

class RcDemo;

class LuaInterpreter
{
public:
    static LuaInterpreter& Create(RcDemo& app);
    static LuaInterpreter& Get(lua_State* L);
    static RcDemo& GetApplication(lua_State* L);

    static void RunScript(lua_State* L, const std::string& file);
    static void DumpStack(lua_State* L);

    LuaInterpreter(lua_State* L, RcDemo& app);
    ~LuaInterpreter();

    RcDemo& GetApplication() const;
    void RunScript(const std::string& file);
    void DumpStack();

private:

    LuaInterpreter(LuaInterpreter&) = delete;
    LuaInterpreter& operator=(LuaInterpreter&) = delete;

    lua_State* const  mL;
    RcDemo& mApp;
};

#endif // LUA_INTERPRETER_HPP
