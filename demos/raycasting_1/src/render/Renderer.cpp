#include "Renderer.hpp"
#include "../LuaHelper.hpp"
#include "../Utils.hpp"

#include <lua.hpp>

#include <sstream>

namespace {

Renderer* init_me_next = nullptr;

int l_init(lua_State* const L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "caption");
    const std::string caption = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    int width, height;
    LuaHelper::GetXYValue(L, "resolution", width, height);

    lua_getfield(L, 1, "show_minimap");
    const bool show = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);

    init_me_next->SetAppName(caption);
    init_me_next->SetResolution(width, height);
    init_me_next->ShowMinimap(show);
    return 0;
}

} // unnamed namespace

Renderer::Renderer(const std::string& renderer_name)
    : mRendererName(renderer_name)
{
    init_me_next = this;
    LuaHelper::InitInstance("renderer", "renderer.lua", l_init);
    init_me_next = nullptr;
}

Renderer::~Renderer()
{

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
    mResX = width;
    mResY = height;
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
