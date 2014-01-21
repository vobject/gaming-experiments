#include "World.hpp"
#include "LuaHelper.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Utils.hpp"

#include <lua.hpp>

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

World* init_me_next = nullptr;

int l_init(lua_State* const L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    int level_size_x, level_size_y;
    LuaHelper::GetXYValue(L, "level_size", level_size_x, level_size_y);

    std::vector<uint32_t> level_data;
    lua_getfield(L, 1, "level_layout");
    lua_pushvalue(L, -1);
    lua_pushnil(L);
    while (lua_next(L, -2))
    {
        lua_pushvalue(L, -2);
        const uint32_t key = static_cast<uint32_t>(lua_tointeger(L, -1));
        const uint32_t value = static_cast<uint32_t>(lua_tointeger(L, -2));

        level_data.push_back(value);
        lua_pop(L, 2);
    }
    lua_pop(L, 1);

    init_me_next->SetLevelSize(level_size_x, level_size_y);
    init_me_next->SetLevelData(level_data.data());
    return 0;
}

} // unnamed namespace

World::World()
{
    init_me_next = this;
    LuaHelper::InitInstance("world", "world.lua", l_init);
    init_me_next = nullptr;

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
