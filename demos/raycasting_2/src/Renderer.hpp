#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>

class Level;
class Player;

class Renderer
{
public:
   Renderer() { }
   virtual ~Renderer() { }

   virtual void PreRender() = 0;
   virtual void PostRender() = 0;
   virtual void DoRender(const Level& level, const Player& player) = 0;

   virtual std::string GetName() const = 0;
};

#endif // RENDERER_HPP
