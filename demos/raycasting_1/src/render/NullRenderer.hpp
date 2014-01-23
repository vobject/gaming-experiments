#ifndef NULL_RENDERER_HPP
#define NULL_RENDERER_HPP

#include "Renderer.hpp"

class LuaInterpreter;
class World;

class NullRenderer : public Renderer
{
public:
    NullRenderer(LuaInterpreter& lua) : Renderer(lua, "NullRenderer") { Startup(); }
    virtual ~NullRenderer() { Shutdown(); }

    void Startup() override { Renderer::Startup(); }
    void Shutdown() override { Renderer::Shutdown(); }

    void PreRender() override { Renderer::PreRender(); }
    void PostRender() override { Renderer::PostRender(); }
    void DoRender(const World&) override { }
};

#endif // NULL_RENDERER_HPP
