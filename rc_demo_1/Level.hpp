#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>
#include <cstdint>

class Level
{
   friend class SwRenderer;
   friend class ClRenderer;

public:
   Level();
   ~Level();

   bool IsBlocking(int pos_x, int pos_y) const;

private:
   std::vector<std::vector<int32_t>> mGrid;
};

#endif // LEVEL_HPP
