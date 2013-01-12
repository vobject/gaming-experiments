#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>

class Level
{
   friend class Render;
   friend class Slice;

public:
   Level();
   ~Level();

   int GetWidth() const;
   int GetHeight() const;

   bool IsBlocking(int pos_x, int pos_y) const;

private:
   std::vector<std::vector<int>> mGrid;
};

#endif // LEVEL_HPP
