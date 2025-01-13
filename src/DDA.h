
#ifndef DDA_H
#define DDA_H

#include "Main.h"

double getScaleFactor(char axis, vd2D dir);
double dda(vd2D pos, vd2D dir, int &mapTileInformation, bool &hitOnAxisX);
vd2D rotateVector(vd2D vec, double angel);
double getRadFromVector(vd2D vec);
double degreeToRad(double degree);


#endif //DDA_H
