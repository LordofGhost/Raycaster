#include "Render.h"
#include "DDA.h"
#include "Map.h"
#include "Texture.h"
#include <math.h>

int draw3dSpace(SDL_Texture* buffer, Player &player) {

    // load the buffer in the RAM and get access to it
    void* pixels;
    int pitch;
    SDL_LockTexture(buffer, NULL, &pixels, &pitch);
    Uint32* pixelArray = (Uint32*)pixels;

    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {

        // calculating the pixel column specific units
        double columnStepSize = (double)RENDER_FOV / (double)RENDER_WIDTH;
        double columnAngle = (columnStepSize * renderColumn) - (RENDER_FOV / 2);
        double columnAngleDifference = abs(degreeToRad(columnAngle));
        vd2D columnDirection = rotateVector(player.dir, degreeToRad(columnAngle));

        // get the distance to the next wall in the with the given direction
        int tileTextureID;
        bool hitOnAxisX;
        double wallDistance = dda(player.pos, columnDirection, tileTextureID, hitOnAxisX);
        // the cos value is needed to remove the fisheye effect
        double wallDistanceNoFishEye =  wallDistance * cos(columnAngleDifference);

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

        // making the height of wall proportional to the screen height
        int distanceToHeight = RENDER_HEIGHT / wallDistanceNoFishEye;
        // Prevent from drawing outside the renderer
        distanceToHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight;

        // calculating the highest and lowest pixel of the wall proportional to the screen
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);

        // loop over all pixel on the column that belong to the wall
        for (int pixel = yTop; pixel <= yBottom; pixel++) {

            // the pixel color needs to be stored in a local variable because it gets modified be the shadow effect
            int pixelColor[] = {0,0,0};
            getTextureColor(tileTextureID, floor(wallHitPosition * getTextureDimensions(tileTextureID)), floor(double(pixel - yTop) / distanceToHeight * getTextureDimensions(tileTextureID)), pixelColor);

            // add shadow effect to texture if the wall is on the x-axis
            if (hitOnAxisX) applyShadow(pixelColor);

            // modify the corresponding pixel in the buffer
            pixelArray[pixel * (pitch / 4) + renderColumn] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888), nullptr, pixelColor[0], pixelColor[1], pixelColor[2], 255);
        }
    }

    // Shift buffer from RAM to the GPU
    SDL_UnlockTexture(buffer);

    return 1;
}

double wallHitPosition(bool hitOnAxisX) {
}

void applyShadow(int* pixelColor) {
    // reduce the color intensity on all 3 color channel
    for (int i = 0; i < 3; i++) {
         pixelColor[i] > SHADOW_STRENGTH ? pixelColor[i] -= SHADOW_STRENGTH : pixelColor[i] = 0;
    }
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

    // draw line where the player looks at and end the line on the first wall hit
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
