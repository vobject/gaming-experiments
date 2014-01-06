#include "World.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Utils.hpp"

namespace {

const int WORLD_MAP_SIZE_WIDTH = 24;
const int WORLD_MAP_SIZE_HEIGHT = 24;
const uint32_t WORLD_MAP_GRID[WORLD_MAP_SIZE_HEIGHT][WORLD_MAP_SIZE_WIDTH] =
{
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

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

World::World(const std::string& level)
{
    (void) level;

    CreatePlayer();

    // dummy sprites
    mSprites = {
        { 12.0, 12.0, 0x00 },
        { 12.0, 15.0, 0xff00ff },
        { 6.0, 12.0, 0xbfbf00 }
    };
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

int World::GetWidth() const
{
    return WORLD_MAP_SIZE_WIDTH;
}

int World::GetHeight() const
{
    return WORLD_MAP_SIZE_HEIGHT;
}

const Player& World::GetPlayer() const
{
    return *mPlayer;
}

const std::vector<Sprite>& World::GetSprites() const
{
    return mSprites;
}

uint32_t World::GetCellType(const int x, const int y) const
{
    if ((x < 0) || (x > WORLD_MAP_SIZE_WIDTH) ||
        (y < 0) || (y > WORLD_MAP_SIZE_HEIGHT))
    {
        // the level bocks for invalid coordinates
        return 1;
    }

    return WORLD_MAP_GRID[y][x];
}

bool World::IsBlocking(const int x, const int y) const
{
    if ((x < 0) || (x > WORLD_MAP_SIZE_WIDTH) ||
        (y < 0) || (y > WORLD_MAP_SIZE_HEIGHT))
    {
        // the level bocks for invalid coordinates
        return true;
    }

    // '0' means floor, we can only walk on floor cells
    return (WORLD_MAP_GRID[y][x] != 0);
}

Player& World::InternalGetPlayer() const
{
    return *mPlayer;
}

void World::CreatePlayer()
{
    // create the default player
    mPlayer = Utils::make_unique<Player>(*this);

    // default placement in this world
    mPlayer->mPosX = 12.5;
    mPlayer->mPosY = 1.5;
    mPlayer->mDirX = -1.0;
    mPlayer->mDirY = 0.0;
}
