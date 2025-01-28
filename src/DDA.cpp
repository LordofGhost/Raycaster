#include "DDA.h"
#include "Map.h"
#include <math.h>

// dismaler for all the math functions: these are not the shortest or most optimized, but they are easy to understand

double getScaleFactor(char axis, vd2D dir) {
    double factor;
    /* the scaling factor is used to determinate the length of the hypotenuse depending on the move distance on the
     * axis. In other words this means, you get the actual move distance. */
    if (axis == 'X') {
        factor = sqrt(pow(1, 2) + pow(dir.y / dir.x, 2));
    } else if (axis == 'Y') {
        factor = sqrt(pow(1, 2) + pow(dir.x / dir.y, 2));
    }

    return factor;
}

double dda(vd2D pos, vd2D dir, int &mapTileInformation, bool &hitOnAxisX) {
    vi2D mapTile = {(int) pos.x, (int) pos.y};
    const double scaleX = getScaleFactor('X', dir);
    const double scaleY = getScaleFactor('Y', dir);

    vi2D rayDirection;
    vd2D rayLength;
    double traveledDistance;

    // this termite the distance to the border of the tile in the direction the vector points
    if (dir.x > 0) {
        // the vector points to the right
        rayDirection.x = 1;
        rayLength.x = (1 - (pos.x - mapTile.x)) * scaleX;
    } else {
        // the vector points to the left
        rayDirection.x = -1;
        rayLength.x = (pos.x - mapTile.x) * scaleX;
    }
    if (dir.y > 0) {
        // the vector points downwards
        rayDirection.y = 1;
        rayLength.y = (1 - (pos.y - mapTile.y)) * scaleY;
    } else {
        // the vector points upwards
        rayDirection.y = -1;
        rayLength.y = (pos.y - mapTile.y) * scaleY;
    }

    int maxDistance = getMapMaxDistance(); // limits the calculating distance
    bool hitWall = false;

    // the loop terminates when a wall is hit or the distance is bigger than on the map possible
    while(!hitWall && traveledDistance < maxDistance) {
        // decide which ray is shorter and then increment on that axis
        if (rayLength.x < rayLength.y) {
            mapTile.x += rayDirection.x; // update tile
            /* traveledDistance:
             * all steps added together, in this case it is not important on which axis previously was traveled on,
             * because when this ray is the shorter one, it includes the current move all prior moves */
            traveledDistance = rayLength.x;
            rayLength.x += 1 * scaleX; // extend ray for check in next loop cycle
            hitOnAxisX = true;
        } else {
            mapTile.y += rayDirection.y;
            traveledDistance = rayLength.y;
            rayLength.y += 1 * scaleY;
            hitOnAxisX = false;
        }

        if ((mapTileInformation = getTileInfo(FLOOR, mapTile)) != 0) hitWall = true;
    }

    return traveledDistance;
}

vd2D rotateVector(vd2D vec, double angle) {
    vd2D rotatedVec;
    rotatedVec.x = (cos(angle) * vec.x) + (-sin(angle) * vec.y);
    rotatedVec.y = (sin(angle) * vec.x) + (cos(angle) * vec.y);
    return rotatedVec;
}

double getRadFromVector(vd2D vec) {
    double rad = tan(vec.y/vec.x);
    return rad;
}

double degreeToRad(double degree) {
    return degree * (M_PI / 180);
}
