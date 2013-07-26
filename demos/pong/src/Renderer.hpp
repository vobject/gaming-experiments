#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <functional>

class Game;

class Renderer
{
public:
   Renderer() { }
   virtual ~Renderer() { }

   virtual void PreRender() = 0;
   virtual void PostRender() = 0;
   virtual void DoRender(const Game& game) = 0;
   virtual void RegisterPostRenderHook(std::function<void(void*)> callback) = 0;

   virtual std::string GetName() const = 0;
   virtual void* GetUnderlying() const = 0;
};

#endif // RENDERER_HPP
