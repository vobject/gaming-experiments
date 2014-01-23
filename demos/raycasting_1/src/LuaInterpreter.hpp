#ifndef LUA_INTERPRETER_HPP
#define LUA_INTERPRETER_HPP

#include <lua.hpp>

#include <string>

class MainLoop;

class LuaInterpreter
{
public:
    static LuaInterpreter& Create(MainLoop& app);
    static LuaInterpreter& GetInterpreter(lua_State* L);

    static void ExecuteScript(lua_State* L, const std::string& file);
    static void ExecuteString(lua_State* L, const std::string& str);
    static void DumpStack(lua_State* L);

    LuaInterpreter(lua_State* L, MainLoop& app);
    ~LuaInterpreter();

    lua_State* GetState() const;
    void ExecuteScript(const std::string& file);
    void ExecuteString(const std::string& str);
    void DumpStack();

private:
    LuaInterpreter(LuaInterpreter&) = delete;
    LuaInterpreter& operator=(LuaInterpreter&) = delete;

    lua_State* const  mL;
    MainLoop& mMainLoop;
};

#endif // LUA_INTERPRETER_HPP
