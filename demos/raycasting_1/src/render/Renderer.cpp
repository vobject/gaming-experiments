#include "Renderer.hpp"
#include "../LuaInterpreter.hpp"
#include "../RcDemo.hpp"
#include "../Utils.hpp"

#include <sstream>

namespace {

const struct luaL_Reg lua_funcs[] = {
    { "show_minimap", [](lua_State* state) {
        if (!lua_isboolean(state, -1)) {
            throw "show_minimap() must specifiy a boolean.";
        }

        const auto show = static_cast<bool>(lua_toboolean(state, -1));
        RcDemo::Instance().GetRenderer().ShowMinimap(show);
        return 0;
    } },

    { nullptr, nullptr }
};

} // unnamed namespace

Renderer::Renderer(const int res_x, const int res_y,
                   const std::string& app_name,
                   const std::string& renderer_name)
    : mResX(res_x)
    , mResY(res_y)
    , mAppName(app_name)
    , mRendererName(renderer_name)
    , mCaption(mAppName)
{

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

void Renderer::RegisterLua(LuaInterpreter& lua)
{
    lua.RegisterFunctions("renderer", lua_funcs);
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
