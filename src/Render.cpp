#include "Render.h"
#include "DDA.h"
#include "Map.h"
#include "math.h"
#include <iostream>

#include "Texture.h"

int draw3dSpace(SDL_Renderer* renderer, Player &player) {
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        // these variables determinate the direction of the ray for the current column
        double columnStepSize = (double)RENDER_FOV / (double)RENDER_WIDTH;
        double columnAngle = (columnStepSize * renderColumn) - (RENDER_FOV / 2);
        vd2D columnDirection = rotateVector(player.dir, degreeToRad(columnAngle));
        double angleDifference = abs(degreeToRad(columnAngle));

        int tileTextureID;
        bool hitOnAxisX; // create a shadow effect on wall that face the y-axis
        double wallDistance = dda(player.pos, columnDirection, tileTextureID, hitOnAxisX);
        double wallDistanceNoFishEye =  wallDistance * cos(angleDifference); // the cos value is needed to remove the fisheye effect

        // calculate the position where the ray hits the wall to correctly map the texture
        double wallHitPosition;
        if (hitOnAxisX) {
            // the wall goes along the x-axis
            wallHitPosition = player.pos.y + (columnDirection.y * wallDistance);
            wallHitPosition -= floor(wallHitPosition);
        } else {
            // the wall goes along the y-axis
            wallHitPosition = player.pos.x + (columnDirection.x * wallDistance);
            wallHitPosition -= floor(wallHitPosition);
        }

        // making the height of wall proportional to the screen height, world scale is already 1
        int distanceToHeight = RENDER_HEIGHT / wallDistanceNoFishEye;
        // Prevent from drawing outside the renderer
        distanceToHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight;
        // calculating both points by going in the middle of the screen and then go up or down
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);
        // loop over all pixel on the column that belong to the wall
        for (int pixel = yTop; pixel <= yBottom; pixel++) {
            int* pixelColor = getTextureColor(tileTextureID, floor(wallHitPosition * getTextureDimensions(tileTextureID)), floor(double(pixel - yTop) / distanceToHeight * getTextureDimensions(tileTextureID)));

            // these two function calls cause a huge performance problem
            SDL_SetRenderDrawColor(renderer, pixelColor[0], pixelColor[1], pixelColor[2],255);
            SDL_RenderPoint(renderer, renderColumn, pixel);
        }
    }
    return 1;
}

int drawMiniMap(SDL_Renderer* renderer, Player &player) {
    // get dimension, then loop over the array
    vi2D mapDimensions = getMatrixDimension();
    int miniMapScale = MINI_MAP_SIZE / mapDimensions.x;
    for (int y = 0; y < mapDimensions.y; y++) {
        for (int x = 0; x < mapDimensions.x; x++) {
            getTileInfo({x, y}) != 0 ? SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) : SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_FRect rect;
            rect.x = x * miniMapScale;
            rect.y = y * miniMapScale;
            rect.w = miniMapScale;
            rect.h = miniMapScale;
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // draw line where the player looks at and end the line on the first wall
    int tileInfo;
    bool shadowSide;
    double distance = dda(player.pos, player.dir, tileInfo, shadowSide);
    SDL_SetRenderDrawColor(renderer, 100, 100,100,255);
    SDL_RenderLine(renderer, player.pos.x * miniMapScale, player.pos.y * miniMapScale, (player.pos.x + (player.dir.x * distance)) * miniMapScale, (player.pos.y + (player.dir.y * distance)) * miniMapScale);

    // draw player position to the mini map
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderPoint(renderer, player.pos.x * miniMapScale, player.pos.y * miniMapScale);
    return 1;
}