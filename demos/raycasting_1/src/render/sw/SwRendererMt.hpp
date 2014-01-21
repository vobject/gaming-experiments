#ifndef SW_RENDERER_MT_HPP
#define SW_RENDERER_MT_HPP

#include "SwRenderer.hpp"

class SwRendererMt : public SwRenderer
{
public:
    SwRendererMt(int threads);
    virtual ~SwRendererMt();

protected:
    void DrawPlayerView(const World& world, const Player& player) override;

    const int mThreadCnt;
};

#endif // SW_RENDERER_MT_HPP
