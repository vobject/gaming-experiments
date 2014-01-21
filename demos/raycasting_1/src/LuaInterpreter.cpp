#include "LuaInterpreter.hpp"
#include "MainLoop.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "render/Renderer.hpp"
#include "Utils.hpp"

#include <map>
#include <memory>
#include <vector>
#include <iostream>

namespace {

std::map<lua_State* const, std::unique_ptr<LuaInterpreter>> interpreters;

void register_api(lua_State* L);

} // unnamed namespace

LuaInterpreter& LuaInterpreter::Create(MainLoop& mainloop)
{
    lua_State* const L = luaL_newstate();
    if (!L) {
        throw "Unable to create a new Lua state.";
    }
    luaL_openlibs(L);
    register_api(L);

    interpreters.insert({ L, Utils::make_unique<LuaInterpreter>(L, mainloop) });
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

MainLoop& LuaInterpreter::GetMainLoop(lua_State* L)
{
    return GetInterpreter(L).GetMainLoop();
}

void LuaInterpreter::RunScript(lua_State* L, const std::string& file)
{
    luaL_dofile(L, file.c_str());
}

void LuaInterpreter::DumpStack(lua_State* L)
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

LuaInterpreter::LuaInterpreter(lua_State* const L, MainLoop& mainloop)
    : mL(L)
    , mMainLoop(mainloop)
{

}

LuaInterpreter::~LuaInterpreter()
{

}

lua_State* LuaInterpreter::GetState() const
{
    return mL;
}

MainLoop& LuaInterpreter::GetMainLoop() const
{
    return mMainLoop;
}

void LuaInterpreter::RunScript(const std::string& file)
{
    LuaInterpreter::RunScript(mL, file);
}

void LuaInterpreter::DumpStack()
{
    LuaInterpreter::DumpStack(mL);
}

namespace {

void register_api(lua_State* const L, const char* const table, const luaL_Reg* api)
{
    lua_newtable(L);
    luaL_setfuncs(L, api, 0);
    lua_setglobal(L, table);
}

void register_api(lua_State* const L)
{
    register_api(L, MainLoop::GetModuleName().c_str(), MainLoop::GetAPI().data());
    register_api(L, World::GetModuleName().c_str(), World::GetAPI().data());
    register_api(L, Player::GetModuleName().c_str(), Player::GetAPI().data());
    register_api(L, Renderer::GetModuleName().c_str(), Renderer::GetAPI().data());
}

} // unnamed namespace
