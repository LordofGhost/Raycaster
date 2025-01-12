
#ifndef DDA_H
#define DDA_H

#include "Main.h"

double getScaleFactor(char axis, vd2D dir);
double dda(vd2D pos, vd2D dir, int &mapTileInformation);


#endif //DDA_H
