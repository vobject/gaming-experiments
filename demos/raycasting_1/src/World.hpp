#ifndef WORLD_HPP
#define WORLD_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class Player;
struct luaL_Reg;

// class Sprite
// {
// public:
//     double x;
//     double y;
//     int color;
// };

class World
{
public:
    World();
    ~World();

    void ProcessInput();
    void Update(long elapsed_time);

    void SetLevelSize(int width, int height);
    void SetLevelData(const uint32_t* data);

    int GetWidth() const;
    int GetHeight() const;
    Player& GetPlayer() const;
//     const std::vector<Sprite>& GetSprites() const;

    uint32_t GetCellType(int x, int y) const;
    bool IsBlocking(int x, int y) const;

    static std::string GetModuleName();
    static std::vector<luaL_Reg> GetAPI();

private:
    World(World&) = delete;
    World& operator=(World&) = delete;

    int mLevelWidth;
    int mLevelHeight;
    std::vector<uint32_t> mLevelData;

    std::unique_ptr<Player> mPlayer;
//     std::vector<Sprite> mSprites;




// public:
//     double cube_size = 64;
};

#endif // WORLD_HPP
