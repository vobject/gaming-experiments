#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>

class Level
{
   friend class SwRenderer;
   friend class ClRenderer;

public:
   Level();
   ~Level();

   bool IsBlocking(int pos_x, int pos_y) const;

private:
   std::vector<std::vector<int>> mGrid;
};

#endif // LEVEL_HPP
