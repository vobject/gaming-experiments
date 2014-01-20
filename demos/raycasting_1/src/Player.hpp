#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Ray.hpp"

#include <memory>
#include <vector>
#include <algorithm>
#include <SDL.h>

class LuaInterpreter;
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

    void SetPosition(double x, double y);
    void SetDirection(double x, double y);
    void SetPlane(double x, double y);

    double GetRotation() const;

    void SetHorizontalRayCount(int ray_cnt);
    const std::vector<RaycastResult>& GetRaycastResults() const;

    void RegisterLua(LuaInterpreter& lua);

public: // hack
    void UpdateRays(long elapsed_time);
    void UpdateRotation(long elapsed_time);
    void UpdateMovement(long elapsed_time);

    void MoveTo(double x, double y, bool rumble_on_wall);

    const World& mWorld;
    std::unique_ptr<Input> mInput;

    // Position vector of the player.
    double mPosX = 1.5;
    double mPosY = 1.5;

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




// public:
//     double height2 = 32;
//     double fov2 = 60.0;
// 
//     double posx2 = 1.5 * 64;
//     double posy2 = 2.5 * 64;
//     double angle2 = 60.0;
// 
//     double planex2 = 320;
//     double planey2 = 200;
// 
//     double plane_dist2 = planex2 / 2 / std::tan((fov2 / 2) * M_PI / 180);
//     double angle_per_ray2 = fov2 / planex2;
};

#endif // PLAYER_HPP
