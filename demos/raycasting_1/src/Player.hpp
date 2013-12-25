#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Ray.hpp"

#include <memory>
#include <vector>

class World;
class Input;

class Player
{
public:
    Player(const World& world);
    ~Player();

    Player(Player&) = delete;
    Player& operator=(Player&) = delete;

    Input& GetInput() const;
    void Update(long elapsed_time);

    void SetHorizontalRayCount(int ray_cnt);
    const std::vector<RaycastResult>& GetRaycastResults() const;

public: // hack
    void UpdateRays(long elapsed_time);
    void UpdateRotation(long elapsed_time);
    void UpdateMovement(long elapsed_time);

    void MoveTo(double x, double y);

    const World& mWorld;
    std::unique_ptr<Input> mInput;

    // Position vector of the player.
    double mPosX = 20.0;
    double mPosY = 20.0;

    // Direction vector of the player.
    double mDirX = -1.0;
    double mDirY = 0.0;

    // Camera plane of the player.
    // Must be perpendicular to the direction (but can change its length).
    // The ratio between direction length and the camera plane determinates FOV.
    double mPlaneX = 0.0;
    double mPlaneY = 0.66;

    int mVerticalLook = 0;

    std::vector<RaycastResult> mRays;
};

#endif // PLAYER_HPP
