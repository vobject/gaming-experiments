#ifndef WORLD_HPP
#define WORLD_HPP

#include <string>
#include <memory>
#include <cstdint>

class Player;

class World
{
public:
    World(const std::string& level);
    ~World();

    World(World&) = delete;
    World& operator=(World&) = delete;

    void ProcessInput();
    void Update(long elapsed_time);

    int GetWidth() const;
    int GetHeight() const;
    const Player& GetPlayer() const;

    uint32_t GetCellType(int x, int y) const;
    bool IsBlocking(int x, int y) const;

    // debugging and scripting interfaces
    Player& InternalGetPlayer() const;

private:
    void CreatePlayer();

    std::unique_ptr<Player> mPlayer;
};

#endif // WORLD_HPP
