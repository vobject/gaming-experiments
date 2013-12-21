#ifndef PLAYER_HPP
#define PLAYER_HPP

class World;
class Input;

class Player
{
public:
    Player(const World& world, const Input& input);
    ~Player();

    Player(Player&) = delete;
    Player& operator=(Player&) = delete;

    void Update(int elapsed_time);

public: // hack
    void UpdateRotation(int elapsed_time);
    void UpdateMovement(int elapsed_time);

    const World& mWorld;
    const Input& mInput;

    // Position vector of the player.
    double mPosX = 20.;
    double mPosY = 20.;

    // Direction vector of the player.
    double mDirX = -1.;
    double mDirY = 0.;

    // Camera plane of the player.
    // Must be perpendicular to the direction (but can change its length).
    // The ratio between direction length and the camera plane determinates FOV.
    double mPlaneX = 0.;
    double mPlaneY = 0.66;
};

#endif // PLAYER_HPP
