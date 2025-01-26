#include "Render.h"
#include "DDA.h"
#include "Map.h"
#include <math.h>

int draw3dSpace(SDL_Texture* buffer, Player &player) {

    // load the buffer in the RAM and get access to it
    void* pixels; int pitch; // the pitch is not needed but can't be replaced by a nullptr
    SDL_LockTexture(buffer, NULL, &pixels, &pitch);
    // memory layout: FF(alpha) FF(red) FF(green) FF(blue)
    Uint32* pixelArray = (Uint32*)pixels;

    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {

        // calculating the pixel column specific units
        double columnStepSize = (double)RENDER_FOV / (double)RENDER_WIDTH;
        double columnAngle = (columnStepSize * renderColumn) - (RENDER_FOV / 2);
        vd2D columnDirection = rotateVector(player.dir, degreeToRad(columnAngle));

        drawFloor(pixelArray, renderColumn);
        drawWalls(pixelArray, player, renderColumn, columnDirection, columnAngle);
    }

    // Shift buffer from RAM to the GPU
    SDL_UnlockTexture(buffer);

    return 1;
}

void drawFloor(Uint32* pixelArray, int &renderColumn) {

    for (int pixel = RENDER_HEIGHT/2; pixel < RENDER_HEIGHT; pixel++) {
        pixelArray[pixel * RENDER_WIDTH + renderColumn] = 255 + (255 << 8) + (255 << 16) + (255 << 24) ;
    }
}

void drawWalls(Uint32* pixelArray, Player &player, int &renderColumn, vd2D &columnDirection, double &columnAngle) {

        // get the distance to the next wall in the given direction
        int tileTextureID;
        bool hitOnAxisX;
        double wallDistance = dda(player.pos, columnDirection, tileTextureID, hitOnAxisX);
        // the cos value is needed to remove the fisheye effect
        double columnAngleDifference = abs(degreeToRad(columnAngle));
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

        // making the height of wall proportional to the screen height, while respecting the FOV
        double wallHeight = RENDER_HEIGHT / wallDistanceNoFishEye;
        //double wallHeight = RENDER_HEIGHT / wallDistanceNoFishEye;
        // Prevent from drawing outside the renderer
        int distanceToHeight;
        wallHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight = wallHeight;

        // calculating the highest and lowest pixel of the wall proportional to the screen
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);

        // loop over all pixel on the column that belong to the wall
        for (int pixel = yTop; pixel <= yBottom; pixel++) {

            // the pixel color needs to be stored in a local variable because it gets modified be the shadow effect
            Pixel color = {0,0,0};
            double pixelOffset = pixel - (RENDER_HEIGHT / 2 - wallHeight / 2);
            vi2D textureCoordinate;
            textureCoordinate.x = floor(wallHitPosition * TEXTURE_SIZE);
            textureCoordinate.y = floor((pixelOffset / wallHeight) * TEXTURE_SIZE);
            getTextureColor(tileTextureID, textureCoordinate.x, textureCoordinate.y, color);

            // add shadow effect to texture if the wall is on the x-axis
            if (hitOnAxisX) applyShadow(color);

            /* modify the corresponding pixel in the buffer
               by modifying the Uint32, where 8 bits stand for one color channel*/
            pixelArray[pixel * RENDER_WIDTH + renderColumn] = color.r + (color.g << 8) + (color.b << 16) + (255 << 24) ;
        }
}

void applyShadow(Pixel &color) {
    // reduce the color intensity on all 3 color channel
    color.r > SHADOW_STRENGTH ? color.r -= SHADOW_STRENGTH : color.r = 0;
    color.g > SHADOW_STRENGTH ? color.g -= SHADOW_STRENGTH : color.g = 0;
    color.b > SHADOW_STRENGTH ? color.b -= SHADOW_STRENGTH : color.b = 0;
}

int drawMiniMap(SDL_Renderer* renderer, Player &player) {
    // get dimension, then loop over the array
    vi2D mapDimensions = getMatrixDimension();
    int miniMapScale = MINI_MAP_SIZE / mapDimensions.x;

    for (int y = 0; y < mapDimensions.y; y++) {
        for (int x = 0; x < mapDimensions.x; x++) {
            // if on the current cell is a wall, draw a white pixel, otherwise a black one
            getTileInfo({x, y}) != 0 ? SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) : SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            // create a square that fits the minimap scale
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
