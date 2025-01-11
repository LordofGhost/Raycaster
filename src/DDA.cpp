#include "DDA.h"
#include "Main.h"
#include <SDL3/SDL_render.h>
#include <math.h>

// dismaler for all the math functions: these are not the shortest or most optimized, but they are easy to understand

double getScaleFactor(char axis, v2D dir) {
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

double dda(Player player) {
    const double scaleX = getScaleFactor('X', player.dir);
    const double scaleY = getScaleFactor('Y', player.dir);

}

int draw3dSpace(SDL_Renderer* renderer, Player player) {

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        double distance;

        // making the height of wall proportional to the screen height, world scale is already 1
        int distanceToHeight = RENDER_HEIGHT / distance;
        // Prevent from drawing outside the renderer
        distanceToHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight;
        // calculating both points by going in the middle of the screen and then go up or down
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);
        SDL_RenderLine(renderer, renderColumn, yTop, renderColumn, yBottom);
    }
    return 1;
}
