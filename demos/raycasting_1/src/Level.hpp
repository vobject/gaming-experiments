#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>
#include <cstdint>

class Level
{
public:
    Level();
    ~Level();

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    const uint32_t** GetGrid() const;
    uint32_t GetBlockType(const int pos_x, const int pos_y) const;

    bool IsBlocking(int pos_x, int pos_y) const;
};

#endif // LEVEL_HPP
