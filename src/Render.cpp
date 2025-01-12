#include "Render.h"
#include "DDA.h"
#include "Map.h"
#include "math.h"

int draw3dSpace(SDL_Renderer* renderer, Player &player) {
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        // these variables determinate the direction of the ray for the current column
        double columnStepSize = (double)RENDER_FOV / (double)RENDER_WIDTH;
        double columnAngle = (columnStepSize * renderColumn) - (RENDER_FOV / 2);
        vd2D columnDirection = rotateVector(player.dir, degreeToRad(columnAngle));
        double angleDifference = abs(degreeToRad(columnAngle));

        int tileColor; // color of the hit wall
        bool shadowSide; // create a shadow effect on wall that face the y-axis
        double wallDistance = dda(player.pos, columnDirection, tileColor, shadowSide) * cos(angleDifference); // the cos value is needed to remove the fisheye effect
        if (tileColor != 0) setRenderColor(renderer, tileColor, shadowSide); else continue; // skip column if, it is out of map

        // making the height of wall proportional to the screen height, world scale is already 1
        int distanceToHeight = RENDER_HEIGHT / wallDistance;
        // Prevent from drawing outside the renderer
        distanceToHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight;
        // calculating both points by going in the middle of the screen and then go up or down
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);
        // loop over all pixel on the column that belong to the wall
        for (int pixel = yTop; pixel <= yBottom; pixel++) {
            SDL_RenderPoint(renderer, renderColumn, pixel);
        }
    }
    return 1;
}

int drawMiniMap(SDL_Renderer* renderer, Player &player) {
    // get dimension, then loop over the array
    vi2D mapDimensions = getMatrixDimension();
    for (int y = 0; y < mapDimensions.y; y++) {
        for (int x = 0; x < mapDimensions.x; x++) {
            setRenderColor(renderer, getTileInfo({x, y}), false);
            SDL_FRect rect;
            rect.x = x * MINI_MAP_SCALE;
            rect.y = y * MINI_MAP_SCALE;
            rect.w = MINI_MAP_SCALE;
            rect.h = MINI_MAP_SCALE;
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // draw line where the player looks at and end the line on the first wall
    int tileInfo;
    bool shadowSide;
    double distance = dda(player.pos, player.dir, tileInfo, shadowSide);
    SDL_SetRenderDrawColor(renderer, 100, 100,100,255);
    SDL_RenderLine(renderer, player.pos.x * MINI_MAP_SCALE, player.pos.y * MINI_MAP_SCALE, (player.pos.x + (player.dir.x * distance)) * MINI_MAP_SCALE, (player.pos.y + (player.dir.y * distance)) * MINI_MAP_SCALE);

    // draw player position to the mini map
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderPoint(renderer, player.pos.x * MINI_MAP_SCALE, player.pos.y * MINI_MAP_SCALE);
    return 1;
}

void setRenderColor(SDL_Renderer* renderer, int tileColor, bool shadow) {
    switch (tileColor) {
        case 0:
            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            break;
        case 1:
            shadow ? SDL_SetRenderDrawColor(renderer, 170,0,0,255) : SDL_SetRenderDrawColor(renderer, 255,0,0,255);
        break;
        case 2:
            shadow ? SDL_SetRenderDrawColor(renderer, 0,170,0,255) : SDL_SetRenderDrawColor(renderer, 0,255,0,255);
        break;
        case 3:
            shadow ? SDL_SetRenderDrawColor(renderer, 0,0,170,255) : SDL_SetRenderDrawColor(renderer, 0,0,255,255);
        break;
    }
}