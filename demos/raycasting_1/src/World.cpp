#include "World.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "LuaInterpreter.hpp"
#include "RcDemo.hpp"
#include "Utils.hpp"

namespace {

// const int WORLD_MAP_SIZE_WIDTH = 4;
// const int WORLD_MAP_SIZE_HEIGHT = 4;
// const uint32_t WORLD_MAP_GRID[WORLD_MAP_SIZE_HEIGHT][WORLD_MAP_SIZE_WIDTH] =
// {
//     { 1, 1, 1, 1 },
//     { 1, 0, 0, 1 },
//     { 1, 0, 0, 1 },
//     { 1, 0, 1, 1 }
// };

} // unnamed namespace

World::World()
{
    mPlayer = Utils::make_unique<Player>(*this);

//     // dummy sprites
//     mSprites = {
//         { 12.0, 12.0, 0x00 },
//         { 12.0, 15.0, 0xff00ff },
//         { 6.0, 12.0, 0xbfbf00 }
//     };
}

World::~World()
{

}

void World::ProcessInput()
{
    mPlayer->GetInput().Update();
}

void World::Update(const long elapsed_time)
{
    mPlayer->Update(elapsed_time);
}

void World::SetLevelSize(const int width, const int height)
{
    mLevelWidth = width;
    mLevelHeight = height;
    mLevelData.resize(mLevelWidth * mLevelHeight);
}

void World::SetLevelData(const uint32_t* data)
{
    std::memcpy(mLevelData.data(), data, mLevelData.size() * sizeof(uint32_t));
}

int World::GetWidth() const
{
    return mLevelWidth;
}

int World::GetHeight() const
{
    return mLevelHeight;
}

Player& World::GetPlayer() const
{
    return *mPlayer;
}

// const std::vector<Sprite>& World::GetSprites() const
// {
//     return mSprites;
// }

uint32_t World::GetCellType(const int x, const int y) const
{
    if ((x < 0) || (x >= mLevelWidth) ||
        (y < 0) || (y >= mLevelHeight))
    {
        // the level bocks for invalid coordinates
        return 1;
    }

    return mLevelData[y * mLevelWidth + x];
}

bool World::IsBlocking(const int x, const int y) const
{
    if ((x < 0) || (x >= mLevelWidth) ||
        (y < 0) || (y >= mLevelHeight))
    {
        // the level bocks for invalid coordinates
        return true;
    }

    // '0' means floor, we can only walk on floor cells
    return (mLevelData[y * mLevelWidth + x] != 0);
}
