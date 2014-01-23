#include "Renderer.hpp"
#include "../LuaInterpreter.hpp"
#include "../LuaInstanceMap.hpp"
#include "../LuaHelper.hpp"
#include "../Utils.hpp"

#include <lua.hpp>

#include <sstream>

namespace {

LuaInstanceMap<Renderer> instances;

} // unnamed namespace

Renderer::Renderer(LuaInterpreter& lua, const std::string& renderer_name)
    : mRendererName(renderer_name)
{
    instances.Add(lua.GetState(), *this);
    lua.RegisterAPI(GetModuleName(), GetAPI().data());
}

Renderer::~Renderer()
{
    instances.Remove(*this);
}

int Renderer::GetResX() const
{
    return mResX;
}

int Renderer::GetResY() const
{
    return mResY;
}

std::string Renderer::GetAppName() const
{
    return mAppName;
}

const std::string& Renderer::GetName() const
{
    return mRendererName;
}

const int& Renderer::getFPS() const
{
    return mFPS;
}

void Renderer::SetAppName(const std::string& name)
{
    mAppName = name;
    mCaption = name;
}

void Renderer::SetResolution(int width, int height)
{
    const auto running = mScreen != nullptr;

    if (running) {
        Shutdown();
    }

    mResX = width;
    mResY = height;

    if (running) {
        Startup();
    }
}

void Renderer::ShowMinimap(const bool show)
{
    mShowMinimap = show;
}

void Renderer::Startup()
{
    if (0 > SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        throw "Cannot init SDL video and timer subsystem.";
    }

    mScreen = SDL_CreateWindow(mAppName.c_str(),
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               mResX, mResY, 0);
    if (!mScreen) {
        throw "SDL_CreateWindow() failed.";
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    mFrameTimer = SDL_AddTimer(1000, FrameTimerCB, this);
    if (!mFrameTimer) {
        throw "SDL_AddTimer() failed.";
    }
}

void Renderer::Shutdown()
{
    SDL_SetRelativeMouseMode(SDL_FALSE);

    if (mFrameTimer) {
        SDL_RemoveTimer(mFrameTimer);
        mFrameTimer = 0;
    }

    if (mScreen) {
        SDL_DestroyWindow(mScreen);
    }
}

void Renderer::PreRender()
{

}

void Renderer::PostRender()
{
    mFrameCount++;

    if (mRefreshCaption)
    {
        // This must be called from the same thread that does the drawing.
        SDL_SetWindowTitle(mScreen, mCaption.c_str());
        mRefreshCaption = false;
    }

//    SDL_WarpMouseInWindow(mScreen, mResX / 2, mResY / 2);
}

std::string Renderer::GetModuleName()
{
    return "renderer";
}

std::vector<luaL_Reg> Renderer::GetAPI()
{
    return {
        { "set_caption", [](lua_State* L) {
            const char* const name = lua_tostring(L, -1);
            instances.Get(L).SetAppName(name);
            return 0;
        } },

        { "set_resolution", [](lua_State* L) {
            const int height = static_cast<int>(lua_tointeger(L, -1));
            const int width = static_cast<int>(lua_tointeger(L, -2));
            instances.Get(L).SetResolution(width, height);
            return 0;
        } },

        { "set_show_minimap", [](lua_State* L) {
            const bool show = lua_toboolean(L, -1) != 0;
            instances.Get(L).ShowMinimap(show);
            return 0;
        } },

        { nullptr, nullptr }
    };
}

Uint32 Renderer::FrameTimerCB(Uint32 interval, void* param)
{
    Renderer* obj = static_cast<Renderer*>(param);

    obj->mFPS = obj->mFrameCount;
    obj->mFrameCount = 0;

    std::ostringstream caption;
    caption << obj->mAppName;
#ifdef NDEBUG
    caption << " (Release-";
#else
    caption << " (Debug-";
#endif
    caption << (Utils::Is64Bit() ? "x64" : "x32") << ") - ";
    caption << "render:" << obj->GetName() << " - ";
    caption << obj->mFPS << " FPS";

    std::string captionStr(caption.str());
    obj->mCaption.swap(captionStr);
    obj->mRefreshCaption = true;
    return interval;
}
