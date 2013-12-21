#ifndef WORLD_HPP
#define WORLD_HPP

#include <cstdint>

class World
{
public:
    World();
    ~World();

    int GetWidth() const;
    int GetHeight() const;
    uint32_t GetCellType(int x, int y) const;
    bool IsBlocking(int x, int y) const;
};

#endif // WORLD_HPP
