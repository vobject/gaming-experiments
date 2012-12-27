#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>

class Level
{
   friend class Render;

public:
   Level();
   ~Level();

   bool IsMovementLegal(int pos_x, int pos_y) const;

private:
   std::vector<std::vector<int>> mGrid;
};

#endif // LEVEL_HPP
