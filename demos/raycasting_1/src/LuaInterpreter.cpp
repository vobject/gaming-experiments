#include "LuaInterpreter.hpp"
#include "Utils.hpp"

#include <map>
#include <memory>
#include <iostream>

namespace {

std::map<lua_State* const, std::unique_ptr<LuaInterpreter>> interpreters;

} // unnamed namespace

LuaInterpreter& LuaInterpreter::Create()
{
    lua_State* const L = luaL_newstate();
    if (!L) {
        throw "Unable to create a new Lua state.";
    }
    luaL_openlibs(L);

    interpreters.insert({ L, Utils::make_unique<LuaInterpreter>(L) });
    return LuaInterpreter::GetInterpreter(L);
}

LuaInterpreter& LuaInterpreter::GetInterpreter(lua_State* L)
{
    const auto iter = interpreters.find(L);
    if (iter == interpreters.end()) {
        throw "Trying to access non-existing Lua interpreter.";
    }
    return *iter->second;
}

void LuaInterpreter::RegisterAPI(lua_State* const L, const std::string& name, const luaL_Reg* api)
{
    lua_newtable(L);
    luaL_setfuncs(L, api, 0);
    lua_setglobal(L, name.c_str());
}

void LuaInterpreter::ExecuteScript(lua_State* L, const std::string& file)
{
    luaL_dofile(L, file.c_str());
}

void LuaInterpreter::ExecuteString(lua_State* L, const std::string& str)
{
    luaL_dostring(L, str.c_str());
}

void LuaInterpreter::PrintStack(lua_State* L)
{
    int top = lua_gettop(L);

    for (int i = 1; i <= top; i++)
    {
        int t = lua_type(L, i);

        switch (t)
        {
        case LUA_TSTRING:
            std::cout << lua_tostring(L, i);
            break;
        case LUA_TBOOLEAN:
            std::cout << (lua_toboolean(L, i) != 0);
            break;
        case LUA_TNUMBER:
            std::cout << lua_tonumber(L, i);
            break;
        default:
            std::cout << lua_typename(L, t);
            break;
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

void LuaInterpreter::PrintGlobals(lua_State* L)
{
    LuaInterpreter::ExecuteString(L, "for k, v in pairs(_G) do \
                                          print(k, v) \
                                      end");
}

LuaInterpreter::LuaInterpreter(lua_State* const L)
    : mL(L)
{

}

LuaInterpreter::~LuaInterpreter()
{

}

lua_State* LuaInterpreter::GetState() const
{
    return mL;
}

void LuaInterpreter::RegisterAPI(const std::string& name, const luaL_Reg* api)
{
    RegisterAPI(mL, name, api);
}

void LuaInterpreter::ExecuteScript(const std::string& file)
{
    LuaInterpreter::ExecuteScript(mL, file);
}

void LuaInterpreter::ExecuteString(const std::string& str)
{
    LuaInterpreter::ExecuteString(mL, str);
}

void LuaInterpreter::PrintStack()
{
    LuaInterpreter::PrintStack(mL);
}

void LuaInterpreter::PrintGlobals()
{
    LuaInterpreter::PrintGlobals(mL);
}
