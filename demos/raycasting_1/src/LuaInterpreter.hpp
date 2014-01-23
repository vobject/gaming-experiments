#ifndef LUA_INTERPRETER_HPP
#define LUA_INTERPRETER_HPP

#include <lua.hpp>

#include <string>

class LuaInterpreter
{
public:
    static LuaInterpreter& Create();
    static LuaInterpreter& GetInterpreter(lua_State* L);

    static void RegisterAPI(lua_State* L, const std::string& name, const luaL_Reg* api);
    static void ExecuteScript(lua_State* L, const std::string& file);
    static void ExecuteString(lua_State* L, const std::string& str);
    static void PrintStack(lua_State* L);
    static void PrintGlobals(lua_State* L);

    LuaInterpreter(lua_State* L);
    ~LuaInterpreter();

    lua_State* GetState() const;
    void RegisterAPI(const std::string& name, const luaL_Reg* api);
    void ExecuteScript(const std::string& file);
    void ExecuteString(const std::string& str);
    void PrintStack();
    void PrintGlobals();

private:
    LuaInterpreter(LuaInterpreter&) = delete;
    LuaInterpreter& operator=(LuaInterpreter&) = delete;

    lua_State* const  mL;
};

#endif // LUA_INTERPRETER_HPP
