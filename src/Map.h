
#ifndef MAP_H
#define MAP_H

#include "Main.h"

enum {
    FLOOR = 0,
    WALL = 1,
    CEILING = 2,
};

int getTileInfo(int type, vi2D);
int getMapMaxDistance();
vi2D getMatrixDimension();
void setStartingPoint(Player&);


#endif //MAP_H
