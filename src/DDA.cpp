#include "DDA.h"
#include "Main.h"
#include "Map.h"
#include <SDL3/SDL_render.h>
#include <math.h>

// dismaler for all the math functions: these are not the shortest or most optimized, but they are easy to understand

double getScaleFactor(char axis, vd2D dir) {
    double factor;

    /*
     * the scaling factor is used to determinate the length of the hypotenuse depending on the move distance on the
     * axis. In other words this means, you get the actual move distance.
     */
    if (axis == 'X') {
        factor = sqrt(pow(1, 2) + pow(dir.y / dir.x, 2));
    } else if (axis == 'Y') {
        factor = sqrt(pow(1, 2) + pow(dir.x / dir.y, 2));
    }

    return factor;
}

double dda(Player player, int* mapTileInformation) {
    vi2D mapTile = {(int) player.pos.x, (int) player.pos.y};
    const double scaleX = getScaleFactor('X', player.dir);
    const double scaleY = getScaleFactor('Y', player.dir);

    vi2D rayDirection;
    vd2D rayLength;
    double traveledDistance;

    /*
     * this termite the distance to the border of the tile in the direction the vector points
     */
    if (player.dir.x > 0) {
        // the vector points to the right
        rayDirection.x = 1;
        rayLength.x = (1 - (player.pos.x - mapTile.x)) * scaleX;
    } else {
        // the vector points to the left
        rayDirection.x = -1;
        rayLength.x = (player.pos.x - mapTile.x) * scaleX;
    }
    if (player.dir.y > 0) {
        // the vector points upwards
        rayDirection.y = 1;
        rayLength.y = (player.pos.y - mapTile.y) * scaleY;
    } else {
        // the vector points downwards
        rayDirection.y = -1;
        rayLength.y = (1 - (player.pos.y - mapTile.y)) * scaleY;
    }

    int maxDistance = getMapMaxDistance(); // limits the calculating distance
    bool hitWall = false;
    /*
     * the loop terminates when a wall is hit or the distance is bigger than on the map possible
     */
    while(!hitWall && traveledDistance < maxDistance) {
        // decide which ray is shorter and then increment on that axis
        if (rayLength.x < rayLength.y) {
            mapTile.x += rayDirection.x; // update tile
            /*
             * traveledDistance:
             * all steps added together, in this case it is not important on which axis previously was traveled on,
             * because when this ray is the shorter one, it includes the current move all prior moves
             */
            traveledDistance = rayLength.x;
            rayLength.x += 1 * scaleX; // extend ray for check in next loop cycle
        } else {
            mapTile.y += rayDirection.y;
            traveledDistance = rayLength.y;
            rayLength.y += 1 * scaleY;
        }

        if ((*mapTileInformation = getTileInfo(mapTile)) != 0) hitWall = true;
    }

    return traveledDistance;
}

int draw3dSpace(SDL_Renderer* renderer, Player player) {

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        int* tileColor;
        double wallDistance = dda(player, tileColor);

        // making the height of wall proportional to the screen height, world scale is already 1
        int distanceToHeight = RENDER_HEIGHT / wallDistance;
        // Prevent from drawing outside the renderer
        distanceToHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight;
        // calculating both points by going in the middle of the screen and then go up or down
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);
        SDL_RenderLine(renderer, renderColumn, yTop, renderColumn, yBottom);
    }
    return 1;
}
