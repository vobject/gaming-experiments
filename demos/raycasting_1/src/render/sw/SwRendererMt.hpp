#ifndef SW_RENDERER_MT_HPP
#define SW_RENDERER_MT_HPP

#include "SwRenderer.hpp"

class SwRendererMt : public SwRenderer
{
public:
    SwRendererMt(int res_x, int res_y, const std::string& app_name, int threads);
    virtual ~SwRendererMt();

protected:
    void DrawPlayerView(const Level& level, const Player& player) override;

    const int mThreadCnt;
};

#endif // SW_RENDERER_MT_HPP
