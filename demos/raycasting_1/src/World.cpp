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

const struct luaL_Reg lua_funcs[] = {
    { "set_level_size", [](lua_State* state) {
        if (!lua_isnumber(state, -1) || !lua_isnumber(state, -2)) {
            throw "set_layout() must specify level dimensions.";
        }

        const int height = static_cast<int>(lua_tointeger(state, -1));
        const int width = static_cast<int>(lua_tointeger(state, -2));

        RcDemo::Instance().GetWorld().SetLevelSize(width, height);
        return 0;
    } },

    { "set_level_layout", [](lua_State* state) {
        std::vector<uint32_t> level_data;

        // http://stackoverflow.com/questions/6137684/iterate-through-lua-table
    
        // Push another reference to the table on top of the stack (so we know
        // where it is, and this function can work for negative, positive and
        // pseudo indices
        lua_pushvalue(state, -1);
        // stack now contains: -1 => table
        lua_pushnil(state);
        // stack now contains: -1 => nil; -2 => table
        while (lua_next(state, -2))
        {
            // stack now contains: -1 => value; -2 => key; -3 => table
            // copy the key so that lua_tostring does not modify the original
            lua_pushvalue(state, -2);
            // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
            const uint32_t key = static_cast<uint32_t>(lua_tointeger(state, -1));
            const uint32_t value = static_cast<uint32_t>(lua_tointeger(state, -2));

            level_data.push_back(value);
            // pop value + copy of key, leaving original key
            lua_pop(state, 2);
            // stack now contains: -1 => key; -2 => table
        }
        // stack now contains: -1 => table (when lua_next returns 0 it pops the key
        // but does not push anything.)
        // Pop table
        lua_pop(state, 1);
        // Stack is now the same as it was on entry to this function

        RcDemo::Instance().GetWorld().SetLevelData(level_data.data());
        return 0;
    } },

    { nullptr, nullptr }
};

} // unnamed namespace

World::World()
{
    mPlayer = Utils::make_unique<Player>(*this);
    mPlayer->SetHorizontalRayCount(RcDemo::Instance().GetWindowWidth());

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

void World::RegisterLua(LuaInterpreter& lua)
{
    lua.RegisterFunctions("world", lua_funcs);
    mPlayer->RegisterLua(lua);
}
