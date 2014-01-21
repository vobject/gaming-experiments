#ifndef LUA_INTERPRETER_HPP
#define LUA_INTERPRETER_HPP

#include <string>

#include <lua.hpp>

class MainLoop;

class LuaInterpreter
{
public:
    static LuaInterpreter& Create(MainLoop& app);
    static LuaInterpreter& GetInterpreter(lua_State* L);
    static MainLoop& GetMainLoop(lua_State* L);

    static void RunScript(lua_State* L, const std::string& file);
    static void DumpStack(lua_State* L);

    LuaInterpreter(lua_State* L, MainLoop& app);
    ~LuaInterpreter();

    MainLoop& GetMainLoop() const;
    void RunScript(const std::string& file);
    void DumpStack();

private:

    LuaInterpreter(LuaInterpreter&) = delete;
    LuaInterpreter& operator=(LuaInterpreter&) = delete;

    lua_State* const  mL;
    MainLoop& mMainLoop;
};

#endif // LUA_INTERPRETER_HPP
